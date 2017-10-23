#pragma once

#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>

#define close(fd) closesocket(fd)
#define ssize_t int
#define socklen_t int
#else
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

class UDPHelper {
  uint16_t _port;
  uint32_t _magic_header;
  sockaddr_in _localaddr, _remote_receiver_addr, _remote_sender_addr;
  int _fd = -1;
public:
  ~UDPHelper() {
    if (_fd != -1)

#ifdef _WIN32
    closesocket(_fd);
	  WSACleanup();
#else
    close(_fd);
#endif
  }

  bool begin(uint16_t port) {
#ifdef _WIN32
	  // Initialize Winsock
	  WSAData wsaData;
	  int iResult;
	  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	  if (iResult != 0) return false;
#endif
    _port = port;

    // Close if open after previous init attempt
    if (_fd != -1) {
#ifdef _WIN32
      closesocket(_fd);
#else
	  close(_fd);
#endif
      _fd = -1;
    }

    // Prepare socket
    _fd=socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP);
    if (_fd==-1) {
      //printf("INIT listening socket %s\n", strerror(errno));
      return false;
    }

    // Make it non-blocking
	#ifdef _WIN32
	DWORD read_timeout = 10; // millis
	#else
    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 1000;
	#endif
	setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&read_timeout, sizeof read_timeout);

    // Bind to specific local port
    memset(&_localaddr, 0, sizeof(_localaddr));
    _localaddr.sin_family = AF_INET;
    _localaddr.sin_port = htons(_port);
    _localaddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_fd,(struct sockaddr *) &_localaddr,sizeof(_localaddr))==-1) {
      //printf("INIT listening bind %s\n", strerror(errno));
      return false;
    }
    memset(&_remote_sender_addr, 0, sizeof(_remote_sender_addr));

    // Prepare receiver address as broadcast
    memset(&_remote_receiver_addr, 0, sizeof(_remote_receiver_addr));
    _remote_receiver_addr.sin_family = AF_INET;
    _remote_receiver_addr.sin_port = htons(_port);
    _remote_receiver_addr.sin_addr.s_addr = INADDR_BROADCAST;

    // Allow broadcasts
    int broadcast=1;
    if (setsockopt(_fd,SOL_SOCKET,SO_BROADCAST,(const char*)&broadcast,sizeof(broadcast))==-1) {
      //printf("INIT send setsockopt %s\n", strerror(errno));
      return false;
    }
    return true;
  }

  uint16_t receive_string(uint8_t *string, uint16_t max_length) {
    struct sockaddr_storage src_addr;
    socklen_t src_addr_len=sizeof(src_addr);
    ssize_t count=recvfrom(_fd,(char*)string,max_length,0,(struct sockaddr*)&src_addr,&src_addr_len);
    if (count==-1) {
#ifdef _WIN32
		//int error = WSAGetLastError();
		//if (error != WSAETIMEDOUT) printf("recvfrom error %d, bufsize=%d\n", error, max_length);
#endif
      return false; // Reception failed
    } else if (count==max_length || count < 4) {
      //printf("FAIL receive_string recvfrom %d\n", count);
      return false; // Too large packet
    } else {
      //printf("OK receive_string recvfrom %d, maxize %d\n", count, max_length);
      // Remember sender's address
      memcpy(&_remote_sender_addr, &src_addr, sizeof(_remote_sender_addr));

      // Get header
      uint32_t header = 0;
      memcpy(&header, string, 4);
      if(header != _magic_header) return false; // Not a LocalUDP packet

      // Shift contents to remove header
      for (uint16_t i=0; i<count-4; i++) string[i] = string[i+4];
      return count - 4;
    }
    return PJON_FAIL;
  }

  class Buf {
    char *buf = NULL;
	uint16_t len = 0;
  public:
	  Buf(uint16_t size) { len = size;  buf = new char[size]; }
    ~Buf() { if (buf) delete buf;  }
    char* operator()() { return buf; }
	uint16_t size() { return len; }
  };


  void send_string(const uint8_t *string, uint16_t length, const sockaddr_in &remote_addr) {
    if(length > 0) {
      Buf buffer(4 + length);
      memcpy(buffer(), &_magic_header, 4);
      memcpy(&(buffer()[4]), string, length);
      int res = sendto(_fd,buffer(),buffer.size(),0,(const sockaddr *)&remote_addr,sizeof(remote_addr));
      //printf("send_string %d sendto %d\n", length, res);
    }
  }

  void send_response(uint8_t response) {
    send_string((const uint8_t *)&response, 1, _remote_sender_addr);
  }

  void send_string(const uint8_t *string, uint16_t length) {
	_remote_receiver_addr.sin_port = htons(_port);
    _remote_receiver_addr.sin_addr.s_addr = INADDR_BROADCAST;
    send_string(string, length, _remote_receiver_addr);
  }

  void send_string(const uint8_t *string, uint16_t length, uint8_t *remote_ip, uint16_t remote_port) {
	_remote_receiver_addr.sin_port = htons(remote_port);
	_remote_receiver_addr.sin_addr.s_addr = *(uint32_t*)remote_ip;
    send_string(string, length, _remote_receiver_addr);
  }

  void set_magic_header(uint32_t magic_header) { _magic_header = magic_header; }
};

#undef close
