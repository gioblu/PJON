#pragma once

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

class UDPHelper {
  uint16_t _port;
  uint32_t _magic_header;
  sockaddr_in _localaddr, _remote_receiver_addr, _remote_sender_addr;
  int _fd = -1;
public:
  ~UDPHelper() {
    if (_fd != -1) close(_fd);
  }

  bool begin(uint16_t port) {
    _port = port;

    // Close if open after previous init attempt
    if (_fd != -1) { close(_fd); _fd = -1; }

    // Prepare socket
    _fd=socket(AF_INET,SOCK_DGRAM,0);
    if (_fd==-1) {
      //printf("INIT listening socket %s\n", strerror(errno));
      return false;
    }

    // Make it non-blocking
    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 1000;
    setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

    // Bind to specific local port
    bzero(&_localaddr, sizeof(_localaddr));
    _localaddr.sin_family = AF_INET;
    _localaddr.sin_port = htons(_port);
    _localaddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_fd,(struct sockaddr *) &_localaddr,sizeof(_localaddr))==-1) {
      //printf("INIT listening bind %s\n", strerror(errno));
      return false;
    }
    bzero(&_remote_sender_addr, sizeof(_remote_sender_addr));

    // Prepare receiver address as broadcast
    bzero(&_remote_receiver_addr, sizeof(_remote_receiver_addr));
    _remote_receiver_addr.sin_family = AF_INET;
    _remote_receiver_addr.sin_port = htons(_port);
    _remote_receiver_addr.sin_addr.s_addr = INADDR_BROADCAST;

    // Allow broadcasts
    int broadcast=1;
    if (setsockopt(_fd,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast))==-1) {
      //printf("INIT send setsockopt %s\n", strerror(errno));
      return false;
    }
    return true;
  }

  uint16_t receive_string(uint8_t *string, uint16_t max_length) {
    struct sockaddr_storage src_addr;
    socklen_t src_addr_len=sizeof(src_addr);
    ssize_t count=recvfrom(_fd,string,max_length,0,(struct sockaddr*)&src_addr,&src_addr_len);
    if (count==-1) {
      return false; // Reception failed
    } else if (count==max_length || count < 4) {
      //printf("FAIL receive_string recvfrom %ld\n", count);
      return false; // Too large packet
    } else {
      //printf("OK receive_string recvfrom %ld\n", count);
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

  void send_string(const uint8_t *string, uint16_t length, const sockaddr_in &remote_addr) {
    if(length > 0) {
      uint8_t buffer[4+length];
      memcpy(buffer, &_magic_header, 4);
      memcpy(&buffer[4], string, length);
      int res = sendto(_fd,buffer,sizeof(buffer),0,(const sockaddr *)&remote_addr,sizeof(remote_addr));
      //printf("send_string %d sendto %d\n", length, res);
    }
  }

  void send_response(uint8_t response) {
    send_string((const uint8_t *)&response, 1, _remote_sender_addr);
  }

  void send_string(const uint8_t *string, uint16_t length) {
    send_string(string, length, _remote_receiver_addr);
  }

  void send_string(const uint8_t *string, uint16_t length, uint32_t remote_ip, uint16_t remote_port) {
    _remote_receiver_addr.sin_port = htons(remote_port);
    _remote_receiver_addr.sin_addr.s_addr = remote_ip;
    send_string(string, length, _remote_receiver_addr);
  }

  void set_magic_header(uint32_t magic_header) { _magic_header = magic_header; }
};
