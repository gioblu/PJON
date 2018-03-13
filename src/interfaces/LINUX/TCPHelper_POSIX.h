#pragma once

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef _WIN32
#include <winsock2.h>
#include <stdlib.h>

#define close(fd) closesocket(fd)
#define ssize_t int
#define socklen_t int
#else
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#endif

#ifndef constrain
#define constrain(x, l, u) (x<l?l:(x>u?u:x))
#endif

class TCPHelperClient {
#ifdef _WIN32
  SOCKET _fd = -1;
#else
  int _fd = -1;
#endif
  sockaddr_in _remote_addr;

public:
  TCPHelperClient() {}
  TCPHelperClient(int fd) { _fd = fd; }
  TCPHelperClient(const TCPHelperClient &another) { *this = another; }

/*
  ~TCPHelperClient() { 
#ifdef _WIN32
    if (_fd != INVALID_SOCKET) closesocket(_fd);
    WSACleanup();
#else
    if (_fd != INVALID_SOCKET) close(_fd);
#endif
  }
*/
  int available() {
#ifdef _WIN32
    WSAPOLLFD pfs;
    pfs.fd = _fd;
    pfs.events = POLLIN;
    pfs.revents = 0;
    int rv = WSAPoll(&pfs, 1, 0);
    if ((rv == -1 && WSAGetLastError() == WSAENETDOWN) || (rv > 0 && (pfs.revents & POLLNVAL) != 0)) {
#ifdef ETCP_ERROR_PRINT
      printf("poll triggered stop, rv=%d: %s\n", rv, strerror(errno));
#endif
      stop(); // Socket problem, close it
    }
    else if (rv > 0)
      return (pfs.revents & POLLRDNORM) != 0 || (pfs.revents & POLLHUP) != 0;
#else
    struct pollfd pfs;
    pfs.fd = _fd;
    pfs.events = POLLIN;
    pfs.revents = 0;
    int rv = ::poll(&pfs, 1, 0);
    if (rv == -1 || (rv > 0 && (pfs.revents & POLLNVAL) != 0)) {
      #ifdef ETCP_ERROR_PRINT
      printf("poll triggered stop, rv=%d: %s\n", rv, strerror(errno));
      #endif
      stop(); // Socket problem, close it
    }
    else if (rv > 0)
      return (pfs.revents & POLLIN) != 0 || (pfs.revents & POLLHUP) != 0;
#endif
    return 0;
  }

  bool connect(const uint8_t *address, uint16_t port) {
    if (_fd != -1) {
      #ifdef ETCP_ERROR_PRINT
      printf("connect triggered stop, _fd=%d\n", _fd);
      #endif
      stop();
    }
    _fd = ::socket(AF_INET,SOCK_STREAM,0);
    if (_fd == -1) return false;
    memset(&_remote_addr, 0, sizeof(_remote_addr));
    _remote_addr.sin_family = AF_INET;
    _remote_addr.sin_port = htons(port);
    memcpy(&_remote_addr.sin_addr.s_addr, address, 4);

    // Shorten timeout for connecting
    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 2000000;
    setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&read_timeout, sizeof read_timeout);
    setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&read_timeout, sizeof read_timeout);

    bool connected = ::connect(_fd, (struct sockaddr *) &_remote_addr,sizeof(_remote_addr)) == 0;
    if (connected) {
      // Shorten timeouts for reading and writing
      struct timeval read_timeout;
      read_timeout.tv_sec = 0;
      read_timeout.tv_usec = 1000;
      setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&read_timeout, sizeof read_timeout);
      read_timeout.tv_usec = 2000000;
      setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&read_timeout, sizeof read_timeout);

      // Disable Nagles algorith because we are sending small packets and waiting for reply
      set_nodelay(true);
    }
    return connected;
  }

  // Prepare a socket for non-blocking connect
  bool prepare_connect(const uint8_t *address, uint16_t port) {
    if (_fd != -1) {
      #ifdef ETCP_ERROR_PRINT
      printf("prepare_connect triggered stop, _fd=%d\n", _fd);
      stop();
      #endif
    }
    _fd = ::socket(AF_INET,SOCK_STREAM,0);
    if (_fd == -1) return false;
    memset(&_remote_addr, 0, sizeof(_remote_addr));
    _remote_addr.sin_family = AF_INET;
    _remote_addr.sin_port = htons(port);
    memcpy(&_remote_addr.sin_addr.s_addr, address, 4);
#ifdef _WIN32
    unsigned long ul = 1;
    int flags = ioctlsocket(_fd, FIONBIO, (unsigned long *)&ul); //Set into non blocking mode.
    if (flags != SOCKET_ERROR) ; // Failed to set?
#else
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags!=-1) fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
#endif
    else {
      #ifdef ETCP_ERROR_PRINT
      printf("prepare_connect triggered stop, _fd=%d: %s\n", _fd, strerror(errno));
      #endif
    }
    return flags != -1;
  }

  // Try a non-blocking connect, it may succeed at once or after some calls
  // Returns -1 if failure, 0 if waiting to connect, 1 when connected
  int8_t try_connect() {
    int8_t connected = 0;
    int status = ::connect(_fd, (struct sockaddr *) &_remote_addr,sizeof(_remote_addr));
    if (status == -1) {
#ifdef _WIN32
      int errno2 = WSAGetLastError();
      if (errno2 == WSAEISCONN); // Already connected, proceed with setting timeouts
      else if (errno2 == WSAEWOULDBLOCK || errno2 == WSAEINPROGRESS || errno2 == WSAEALREADY) 
        return 0; // Normal, we are waiting for connect to succeed
#else
      if (errno == EISCONN); // Already connected, proceed with setting timeouts
      else if (errno == EINPROGRESS || errno == EALREADY) return 0; // Normal, we are waiting for connect to succeed
#endif
      else {
        #ifdef ETCP_ERROR_PRINT
#ifdef _WIN32
        printf("try_connect triggered stop, _fd=%d: %d\n", _fd, errno2);
#else
        printf("try_connect triggered stop, _fd=%d: %d %s\n", _fd, errno, strerror(errno));
#endif
        #endif
        stop();
        return -1; // Some other error
      }
    }

    // Enable blocking operations
#ifdef _WIN32
    unsigned long ul = 0;
    int flags = ioctlsocket(_fd, FIONBIO, (unsigned long *)&ul); // Set into non blocking mode.
    //if (flags != SOCKET_ERROR) ; // Failed to set.
#else
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags != -1) fcntl(_fd, F_SETFL, flags & ~O_NONBLOCK);
#endif

    // Set timeouts for reading and writing
    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 1000;
    setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&read_timeout, sizeof read_timeout);
    read_timeout.tv_usec = 2000000;
    setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&read_timeout, sizeof read_timeout);

    // Disable Nagles algorith because we are sending small packets and waiting for reply
    set_nodelay(true);

    return 1;
  }

  // Enable or disable Nagles algorithm to send unfilled packets immediately or not
  void set_nodelay(bool nodelay) {
    int flag = nodelay;
    if (_fd != -1) setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof flag);
  }

  bool connected() { return _fd != -1; }

  int read(uint8_t *buffer, int buffer_size) {
    if (_fd == -1) return -1;
    int r = ::recv(_fd, (char*)buffer, buffer_size, 0); //MSG_DONTWAIT);
    if (r == -1) {
#ifdef _WIN32
      int errno2 = WSAGetLastError();
      if (errno2 == WSAEWOULDBLOCK) return 0; // Waiting for more
#ifdef ETCP_ERROR_PRINT
      printf("read triggered stop, r=%d: %d\n", r, errno2);
#endif
#else
      if (errno == EAGAIN) return 0; // Waiting for more
      #ifdef ETCP_ERROR_PRINT
      printf("read triggered stop, r=%d: %s\n", r, strerror(errno));
      #endif
#endif
      stop();
    }
    return r;
  }

  int write(const uint8_t *buffer, int size) {
    if (_fd == -1) return -1;
#ifdef _WIN32
    int w = ::send(_fd, (char*)buffer, size, 0);
#else
    int w = ::send(_fd, (char*)buffer, size, MSG_NOSIGNAL);
#endif
    if (w == -1) {
      #ifdef ETCP_ERROR_PRINT
#ifdef _WIN32
      int errno2 = WSAGetLastError();
      printf("write triggered stop, w=%d: %d\n", w, errno2);
#else
      if (errno != EPIPE) printf("write triggered stop, w=%d: %s\n", w, strerror(errno));
#endif
      #endif
      stop();
    }
    return w;
  }

  void flush() { }

  void stop() {
    if (_fd != -1) {
#ifndef _WIN32
      ::shutdown(_fd, SHUT_RDWR);
#endif
      ::close(_fd);
      _fd = -1;
    }
  }

  void operator=(const TCPHelperClient &another) { _fd = another._fd; }

  operator bool() { return connected(); }
  bool operator==(const bool value) { return bool() == value; }
  bool operator!=(const bool value) { return bool() != value; }
  bool operator==(const TCPHelperClient& rhs) {
    return _fd == rhs._fd && _fd != -1 && rhs._fd != -1;
  }
  bool operator!=(const TCPHelperClient& rhs) { return !this->operator==(rhs); }
  uint8_t getSocketNumber() { return _fd; }

  int print(const char *msg) { return write((const uint8_t*) msg, strlen(msg)); }
};


class TCPHelperServer {
  uint16_t _port = 0;
  int _fd = -1;
  sockaddr_in _localaddr, _remote_sender_addr;

public:
  TCPHelperServer(uint16_t listening_port) { _port = listening_port; }
  ~TCPHelperServer() {
    #ifdef ETCP_DEBUG_PRINT
    printf("destructor triggered stop, _fd=%d\n", _fd);
    #endif
    stop();
  }

  TCPHelperClient available() {
    socklen_t len = sizeof(_remote_sender_addr);
    memset(&_remote_sender_addr, 0, len);
    int connected_fd = _fd == -1 ? -1 : ::accept(_fd, (struct sockaddr *) &_remote_sender_addr, &len);
    if (connected_fd != -1) {
      // Shorten timeout for reading and writing
      struct timeval read_timeout;
      read_timeout.tv_sec = 0;
      read_timeout.tv_usec = 1000;
      setsockopt(connected_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&read_timeout, sizeof read_timeout);
      read_timeout.tv_usec = 2000000;
      setsockopt(connected_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&read_timeout, sizeof read_timeout);

      // Disable Nagles algorith because we are sending small packets and waiting for reply
      int flag = 1;
      setsockopt(connected_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof flag);
    }
    return TCPHelperClient(connected_fd);
  }

  bool begin() {
    // Close if open after previous init attempt
    if (_fd != -1) { close(_fd); _fd = -1; }

    // Prepare socket
    _fd=socket(AF_INET,SOCK_STREAM,0);
    if (_fd==-1) {
      #ifdef ETCP_ERROR_PRINT
      printf("failure creating socket: %s\n", strerror(errno));
      #endif
      return false;
    }

    int yes = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));

    // Bind to specific local port
    memset(&_localaddr, 0, sizeof(_localaddr));
    _localaddr.sin_family = AF_INET;
    _localaddr.sin_port = htons(_port);
    _localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_fd,(struct sockaddr *) &_localaddr,sizeof(_localaddr))==-1) {
      #ifdef ETCP_ERROR_PRINT
      printf("bind failed: %s\n", strerror(errno));
      #endif
      return false;
    }

    // Start listening
    if (listen(_fd, 5) == -1) {
      #ifdef ETCP_ERROR_PRINT
      printf("listen failed: %s\n", strerror(errno));
      #endif
      return false;
    }

    // Set the socket to nonblocking so that accept will not block
#ifdef _WIN32
    unsigned long ul = 1;
    int flags = ioctlsocket(_fd, FIONBIO, (unsigned long *)&ul); //Set into non blocking mode.
    if (flags != NO_ERROR) { // Failed to set.
#ifdef ETCP_ERROR_PRINT
      printf("ioctlsocket FIONBIO failed.");
#endif
    }
#else
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags != -1) fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
#endif
    memset(&_remote_sender_addr, 0, sizeof(_remote_sender_addr));
    return true;
  }

  void stop() { if (_fd != -1) { ::close(_fd); _fd = -1; } }
};

#undef close
