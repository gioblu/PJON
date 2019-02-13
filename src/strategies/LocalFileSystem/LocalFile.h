#ifndef LocalFileStrategy_h
#define LocalFileStrategy_h
#endif

#include <stdint.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#include "PJON.h"

class LocalFile {
  private:
    bool receiveInProgress = false;
    int collisionCounter;
    int sentCounter;
    int receiveCounter;

    void readFileContent(unsigned char *stringdata, uint16_t length) {
        std::ifstream binFile("pjon_content", std::ios::in | std::ios::binary);
        if(binFile.is_open())
        {
            unsigned char* buf[length] = {0};
            binFile.read((char*)&buf[0], length);
            memcpy(stringdata, buf, length);
        }
    };

    void clearContent() {
      std::ofstream clearFile;
      clearFile.open("pjon_content", std::ios::trunc);
      clearFile.close(); 
      clearFileOperationSemaphore();
   }

    uint16_t writeFileContent(uint8_t *string, uint16_t length) {
      unsigned char* buf[length] = {0};
      memcpy(buf, string, length);
      std::ofstream binFile;
      binFile.open ("pjon_content", std::ios::binary);
      binFile.write((char*)&buf[0], length);
      binFile.close();
      return 0;
    };

    bool contentFilePresent() {
      std::ifstream ifile("pjon_content");
      return (bool)ifile;
      }

    bool fileOperationSemaphorePresent() {
      std::ifstream ifile("pjon_semaphore");
      return (bool)ifile;
    }

    void setFileOperationSemaphore() {
      std::ofstream file;
      file.open ("pjon_semaphore");
      file.close();
    }

    void clearFileOperationSemaphore() {
      if (fileOperationSemaphorePresent()) {
        while (remove( "pjon_semaphore")!=0) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));   
        };
      }
    }

  public:
    uint16_t last_send_result = PJON_ACK;

    uint32_t back_off(uint8_t attempts) { 
        if (attempts>=get_max_attempts()) {
          clearContent();
          last_send_result = PJON_FAIL;
        }
        return attempts;
    };  

    bool begin(uint8_t additional_randomness) {
      // create file to contain payload for all busses if not there
      if (!contentFilePresent()) {
        std::ofstream file;
        file.open ("pjon_content");
        file.close();
      } else {
        clearContent();
      }
      if (fileOperationSemaphorePresent()) {
        clearFileOperationSemaphore();
      }
      return true;
    };
    
    void handle_collision() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    };

    bool can_start(){
      return !fileOperationSemaphorePresent();
    };
    
    uint8_t  get_max_attempts() {
        return 10;
    };
    
    uint16_t receive_string(uint8_t *string, uint16_t max_length) {
      readFileContent(string, max_length);
      last_send_result = PJON_ACK;
      return max_length;
    };
    
    uint16_t receive_response() {
      return last_send_result;
    };
    
    void send_response(uint8_t response) {
      clearContent();
    };
    
    void send_string(uint8_t *string, uint16_t length) {
        setFileOperationSemaphore();
        writeFileContent(string, length); 
    };
};