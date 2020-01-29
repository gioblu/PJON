#pragma once

#ifdef PJON_ESP
  #if defined(ESP32)
    #include <WiFi.h>
  #else
    #include <ESP8266WiFi.h>
  #endif
  typedef WiFiServer TCPHelperServer;
  typedef WiFiClient TCPHelperClient;
  #define min(a,b) (a<b?a:b)
  #define PJON_GET_MAC WiFi.macAddress
#else
  #ifdef PJON_ETHERNET2
    #include <Ethernet2.h>
  #else
    #include <Ethernet.h>
  #endif
  // #include <utility/socket.h> // May be required if using outdated IDE
  typedef EthernetServer TCPHelperServer;
  typedef EthernetClient TCPHelperClient;
  #define PJON_GET_MAC EthernetClass::MACaddress
#endif

/*
// The following classes make it posssible to use the same non-blocking connect logic
// on Arduino as on POSIX. It works, allowing better communication with POSIX devices,
// but requires that the official EthernetClient and EthernetServer classes are changed
// so that private members are made protected, making it possible to add
// functionality to them.

class TCPHelperClient : public EthernetClient {
public:
  bool prepare_connect(IPAddress ip, uint16_t port) {
    if (_sock != MAX_SOCK_NUM)
      stop();

    if (_sock != MAX_SOCK_NUM)
      return false;

    for (int i = 0; i < MAX_SOCK_NUM; i++) {
      uint8_t s = socketStatus(i);
      if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT || s == SnSR::CLOSE_WAIT) {
        _sock = i;
        break;
      }
    }

    if (_sock == MAX_SOCK_NUM)
      return false;

    _srcport++;
    if (_srcport == 0) _srcport = 49152;          //Use IANA recommended ephemeral port range 49152-65535
    socket(_sock, SnMR::TCP, _srcport, 0);

    if (!::connect(_sock, rawIPAddress(ip), port)) {
      _sock = MAX_SOCK_NUM;
      return false;
    }

    return true;
  }

  int try_connect() {
    if (status() == SnSR::ESTABLISHED) return 1;
    if (status() == SnSR::CLOSED) {
      _sock = MAX_SOCK_NUM;
      return -1;
    }
    return 0;
  }

  TCPHelperClient() : EthernetClient() { }

  TCPHelperClient(uint16_t sock) : EthernetClient(sock) { }

  TCPHelperClient(EthernetClient client) : EthernetClient(client ){ }
};

class TCPHelperServer : public EthernetServer {
public:
  TCPHelperServer(uint16_t sock) : EthernetServer(sock) { }

  TCPHelperClient available() {
    EthernetClient client = EthernetServer::available();
    return TCPHelperClient(client);
  }
};
*/
