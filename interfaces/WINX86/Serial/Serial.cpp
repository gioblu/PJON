/** Serial.cpp
 *
 * A very simple serial port control class that does NOT require MFC/AFX.
 *
 * @author Hans de Ruiter, Zbigniew Zasieczny
 *
 * @version 0.1 -- 28 October 2008
 * @version 0.2 -- 20 April 2017 (Zbigniew Zasieczny)
 *  - added methods needed by PJON to handle WINX86 abstraction interface
 *  - modified flush method to use WIN API
 *  - added initial tests
 *  - changed timeouts to 1s
 */

#include <iostream>
using namespace std;

#include "Serial.h"
#include <sstream>


Serial::Serial(tstring &commPortName, int bitRate, bool testOnStartup, bool cycleDtrOnStartup) {
  commHandle =
    CreateFile(
      commPortName.c_str(),
      GENERIC_READ | GENERIC_WRITE,
      0,
      NULL,
      OPEN_EXISTING,
      0,
      NULL
    );

  if(commHandle == INVALID_HANDLE_VALUE)
    throw("ERROR: Could not open com port");
  else {
    // set timeouts
    COMMTIMEOUTS timeouts;

    /* Blocking:
        timeouts.ReadIntervalTimeout = MAXDWORD;
        timeouts.ReadTotalTimeoutConstant = 0;
        timeouts.ReadTotalTimeoutMultiplier = 0;
       Non-blocking:
        timeouts = { MAXDWORD, 0, 0, 0, 0}; */

    // Non-blocking with short timeouts
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 1;

    DCB dcb;
    if(!SetCommTimeouts(commHandle, &timeouts)) {
      Serial::~Serial();
      throw("ERROR: Could not set com port time-outs");
    }

    // set DCB; disabling harware flow control; setting 1N8 mode
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    dcb.BaudRate = bitRate;
    dcb.fBinary = 1;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.ByteSize = 8;

    if(!SetCommState(commHandle, &dcb)) {
      Serial::~Serial();
      throw("ERROR: Could not set com port parameters");
    }
  }

  if(cycleDtrOnStartup) {
    if(!EscapeCommFunction(commHandle, CLRDTR))
      throw("ERROR: clearing DTR");
    Sleep(200);
    if(!EscapeCommFunction(commHandle, SETDTR))
      throw("ERROR: setting DTR");
  }

  if(testOnStartup) {
    DWORD numWritten;
    char init[] = "PJON-python init";
    if(!WriteFile(commHandle, init, sizeof(init), &numWritten, NULL))
      throw("writing initial data to port failed");
    if(numWritten != sizeof(init))
      throw("ERROR: not all test data written to port");
  }
};


Serial::~Serial() {
  CloseHandle(commHandle);
};


int Serial::write(const char *buffer) {
  DWORD numWritten;
  WriteFile(commHandle, buffer, strlen(buffer), &numWritten, NULL);
  return numWritten;
};


int Serial::write(char *buffer, int buffLen) {
  DWORD numWritten;
  WriteFile(commHandle, buffer, buffLen, &numWritten, NULL);
  return numWritten;
};


int Serial::putChar(char *buffer) {
  return write(buffer, 1);
};


char Serial::getChar() {
  char buff;
  read(&buff, 1, false);
  return buff;
};


int Serial::read(char *buffer, int buffLen, bool nullTerminate) {
  DWORD numRead;
  if(nullTerminate) --buffLen;
  BOOL ret = ReadFile(commHandle, buffer, buffLen, &numRead, NULL);
  if(!ret) return 0;
  if(nullTerminate) buffer[numRead] = '\0';
  return numRead;
};


bool Serial::serialDataAvail() {
  COMSTAT stats;
  DWORD error;
  ClearCommError(commHandle, &error, &stats);
  if(stats.cbInQue > 0) return true;
  return false;
};


void Serial::flush() {
  PurgeComm(commHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
  return;
};
