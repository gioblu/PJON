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
 *
 *
 * 11/05/2020 - getByte function simplified, now returns -1 in case of failure
 */

#if defined(_WIN32)

#include <iostream>
using namespace std;

#include "Serial.h"
#include <sstream>

std::wstring s2ws(const std::string& s){
  int len;
  int slength = (int)s.length() + 1;
  len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
  wchar_t* buf = new wchar_t[len];
  MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
  std::wstring r(buf);
  delete[] buf;
  return r;
};


Serial::Serial(
  std::string &commPortName,
  int bitRate,
  bool testOnStartup,
  bool cycleDtrOnStartup
) {
  std::wstring com_name_ws = s2ws(commPortName);

  commHandle =
    CreateFileW(
      com_name_ws.c_str(),
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

int Serial::writeByte(uint8_t *buffer) {
  DWORD numWritten;
  WriteFile(commHandle, buffer, 1, &numWritten, NULL);
  return numWritten;
};

int16_t Serial::getByte() {
  uint8_t buff;
  if(read(&buff, 1) == -1) return -1;
  return buff;
};


int Serial::read(uint8_t *buffer, int buffLen) {
  DWORD numRead;
  BOOL ret = ReadFile(commHandle, buffer, buffLen, &numRead, NULL);
  if(!ret) return -1;
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

#endif
