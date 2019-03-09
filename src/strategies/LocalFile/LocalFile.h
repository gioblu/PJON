
#pragma once

#include "FileLockFunctions.h"

#include "PJON.h"

// The maximum number of messages in the content file
#ifndef LF_QUEUESIZE
  #define LF_QUEUESIZE 20
#endif

// The name of the content file
#ifndef LF_FILENAME
  #define LF_FILENAME "../PJONLocalFile.dat"
#endif

// Delay in ms between each check for new packets on the disk
#ifndef LF_POLLDELAY
  #define LF_POLLDELAY 10
#endif

#define PJON_LF_DEBUG

class LocalFile {
  private:
    int fn = -1;

    /* The last record read from file, this is remembered to decide which
       records have been read and which are unread. Note that record number 0
       is never used, even when overflowing/wrapping around. */
    uint16_t lastRecordIdRead = 0;

    uint16_t last_send_result = PJON_ACK;

    struct Record {
      uint16_t length;
      uint8_t message[PJON_PACKET_MAX_LENGTH];
      Record() { memset(this, 0, sizeof(Record)); }
    };

    void doOpen(const bool create = false) {
      if(fn != -1) return; // Already open
      int mode = O_RDWR,
          permissions = S_IREAD | S_IWRITE;
      if(create) mode |= O_CREAT;
      #ifdef _WIN32
        mode |= O_RANDOM | O_BINARY;
        _sopen_s(&fn, LF_FILENAME, mode, _SH_DENYNO, permissions);
      #else
        permissions |= S_IRGRP | S_IWGRP | S_IROTH;
        // rw for owner+group, r for others
        fn = open(LF_FILENAME, mode, permissions);
      #endif
    };

    bool openContentFile() {
      if(fn != -1) return true;
      bool file_exists = CheckIfFile(LF_FILENAME);
      if(!file_exists) {
        // Create and initialize the file
        uint16_t lastRecordId = 0, index[LF_QUEUESIZE];
        memset(index, 0, LF_QUEUESIZE * sizeof(uint16_t));
        doOpen(true);
        if(fn != -1) {
          lock();
          write(fn, (const char*)&lastRecordId, sizeof(lastRecordId));
          write(fn, (const char*)index, LF_QUEUESIZE*sizeof(uint16_t));
          Record record;
          for(uint8_t i=0; i<LF_QUEUESIZE; i++)
            write(fn, (const char*)&record, sizeof(Record));
          unlock();
        }
      }
      if(fn == -1) doOpen();
      return true;
    };

    void closeContentFile() {
      if(fn != -1) { close(fn); fn = -1; }
    };

    bool readIndex(uint16_t &lastRecordId, uint16_t index[LF_QUEUESIZE]) {
      lseek(fn, 0, SEEK_SET);
      read(fn, (char*)&lastRecordId, sizeof(uint16_t));
      read(fn, (char *)index, LF_QUEUESIZE*sizeof(uint16_t));
      return true;
    };

    bool updateIndex(uint16_t recordId, uint8_t indexPos) {
      lseek(fn, 0, SEEK_SET);
      write(fn, (const char*)&recordId, sizeof(uint16_t));
      lseek(fn, sizeof recordId + indexPos*sizeof(uint16_t), SEEK_SET);
      write(fn, (const char*)&recordId, sizeof(uint16_t));
      return true;
    };

    void lock() {
      if(fn != -1) LockFileSection(fn, 0, 1, 1, 1, 0);
    };

    void unlock() {
      if(fn != -1) LockFileSection(fn, 0, 1, 0, 1, 0);
    };

    bool writePacketToFile(const Record &record) {
      bool success = false;
      lock();
      uint16_t lastRecordId, index[LF_QUEUESIZE];
      if(readIndex(lastRecordId, index)) {
        // Find a position then write the record to the file
        uint8_t recordPos = findReusablePosition(lastRecordId, index);
        if(recordPos != PJON_NOT_ASSIGNED) {
          #ifdef PJON_LF_DEBUG
            uint16_t diff = (uint16_t)(lastRecordId - index[recordPos]);
            bool rollover = lastRecordId < index[recordPos];
            if(
              (!rollover && (diff+1 != LF_QUEUESIZE)) ||
              (rollover && (diff != LF_QUEUESIZE))
            ) printf(
                "countermismatch=%d/%d/%d/(%d) ",
                recordPos,
                index[recordPos],
                lastRecordId,
                diff
              );
          #endif
          uint32_t filePos =
            sizeof(uint16_t) +
            LF_QUEUESIZE * sizeof(uint16_t) +
            recordPos * sizeof(Record)
          ;
          lseek(fn, filePos, SEEK_SET);
          write(fn, (const char*)&record, sizeof(Record));
          uint16_t nextRecordId = (uint16_t)(lastRecordId + 1);
          if(nextRecordId == 0)
            nextRecordId = 1; // Avoid record id 0, used to mark unused slot
          success = updateIndex(nextRecordId, recordPos);
        }
      }
      unlock();
      return success;
    };

    bool readNextPacketFromFile(Record &record) {
      bool success = false;
      uint16_t lastRecordId, index[LF_QUEUESIZE];
      lock();
      if(readIndex(lastRecordId, index)) {
        // Find a position then read the record from the file
        if(lastRecordId != 0) {
          uint8_t recordPos = findNextUnreadPosition(lastRecordId, index);
          if(recordPos != PJON_NOT_ASSIGNED) {
            uint32_t filePos =
              sizeof(uint16_t) +
              LF_QUEUESIZE * sizeof(uint16_t) +
              recordPos * sizeof(Record)
            ;
            lseek(fn, filePos, SEEK_SET);
            read(fn, (char*)&record, sizeof(Record));
            lastRecordIdRead = index[recordPos];
            success = true;
          }
        }
      }
      unlock();
      return success;
    };

    uint8_t findReusablePosition(
      const uint16_t lastRecordId,
      const uint16_t index[LF_QUEUESIZE]
    ) {
      uint16_t maxDiff = 0;
      uint8_t pos = PJON_NOT_ASSIGNED;
      for(uint8_t i = 0; i < LF_QUEUESIZE; i++) {
        if(index[i] == 0) return i; // Never used
        uint16_t diff = (uint16_t)(lastRecordId - index[i]);
        if(diff > maxDiff) {
          maxDiff = diff;
          pos = i;
        }
      }
      return pos;
    };

    uint8_t findNextUnreadPosition(
      const uint16_t lastRecordId,
      const uint16_t index[LF_QUEUESIZE]
    ) {
      uint8_t pos = PJON_NOT_ASSIGNED;
      if(lastRecordIdRead == 0)
        lastRecordIdRead = lastRecordId - LF_QUEUESIZE;
      if(lastRecordId == lastRecordIdRead)
        return pos; // Nothing new has arrived
      uint16_t minDiff = 0xFFFF;
      for(uint8_t i = 0; i < LF_QUEUESIZE; i++) {
        if(index[i] == 0 || index[i] == lastRecordIdRead)
          continue; // Never used or already read
        uint16_t diff = (uint16_t)(index[i] - lastRecordIdRead);
        if(diff < 0x8FFF && diff < minDiff) {
          minDiff = diff;
          pos = i;
        }
      }
      return pos;
    };

  public:

    ~LocalFile() {
      closeContentFile();
    };

    uint32_t back_off(uint8_t attempts) {
      return 1000 * attempts;
    };

    bool begin(uint8_t additional_randomness) {
      return openContentFile();
    };

    void handle_collision() {
      PJON_DELAY(10);
    };

    bool can_start(){
      if(fn == -1) openContentFile();
      return fn != -1;
    };

    uint8_t get_max_attempts() {
      return 10;
    };

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {
      Record record;
      if(readNextPacketFromFile(record)) {
        uint16_t length =
          record.length < max_length ? record.length : max_length;
        memcpy(string, record.message, length);
        return length;
        // Relax polling to avoid stressing the disk and CPU too much
      }  else PJON_DELAY(LF_POLLDELAY);
      return PJON_FAIL;
    };

    uint16_t receive_response() {
      return last_send_result;
    };

    void send_response(uint8_t response) {
    };

    void send_string(uint8_t *string, uint16_t length) {
      Record record;
      memcpy(&record.message, string, length);
      record.length = length;
      bool ok = writePacketToFile(record);
      last_send_result = ok ? PJON_ACK : PJON_FAIL;
    };
};
