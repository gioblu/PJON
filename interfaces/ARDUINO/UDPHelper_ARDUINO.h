#pragma once

#ifdef ARDUINO_ESP8266_WEMOS_D1MINI // WeMos mini and D1 R2
  #define PJON_ESP
#elif ARDUINO_ESP8266_ESP01     // Generic ESP's use for 01's
  #define PJON_ESP
#elif ARDUINO_ESP8266_NODEMCU   // Wio Link and NodeMCU 1.0 (also 0.9), use for ESP12
  #define PJON_ESP
#endif

#ifdef PJON_ESP
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#define PJON_ESP
#else
  #include <Ethernet.h>
  #include <EthernetUDP.h>
#endif

class UDPHelper {
  uint16_t _port;
  uint32_t _magic_header;

#ifdef PJON_ESP
  WiFiUDP udp;
#else
  EthernetUDP udp;
#endif
  uint8_t _broadcast[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

public:

  bool begin(uint16_t port) {
    _port = port;
    return udp.begin(_port);
  }

  int16_t receive_string(uint8_t *string, uint16_t max_length) {
    uint16_t packetSize = udp.parsePacket();
    if(packetSize > 0) {
      uint32_t header = 0;
      uint16_t len = udp.read((char *) &header, 4);
      if(len != 4 || header != _magic_header) return PJON_FAIL; // Not an expected packet
      if (packetSize > 4 + max_length) return PJON_FAIL;
      len = udp.read(string, packetSize - 4);
      if (len != packetSize - 4) return PJON_FAIL;
      return packetSize - 4;
    }
    return PJON_FAIL;
  }

  void send_string(uint8_t *string, uint16_t length, IPAddress remote_ip, uint16_t remote_port) {
    if(length > 0) {
      udp.beginPacket(remote_ip, remote_port);
      udp.write((const char*) &_magic_header, 4);
      udp.write(string, length);
      udp.endPacket();
    }
  }

  void send_response(uint8_t response) {
    send_string(&response, 1, udp.remoteIP(), udp.remotePort());
  }

  void send_string(uint8_t *string, uint16_t length, uint8_t remote_ip[], uint16_t remote_port) {
    IPAddress address(remote_ip);
    send_string(string, length, address, remote_port);
  }

  void send_string(uint8_t *string, uint16_t length) {
    // Broadcast on local subnet, global broadcast may not be accepted
    IPAddress broadcastIp;
#ifdef PJON_ESP
    // 255.255.255.255 may not work, be more specific
    broadcastIp = ~WiFi.subnetMask() | WiFi.localIP();
#else
    broadcastIp = _broadcast;
#endif
    send_string(string, length, broadcastIp, _port);
  }

  void set_magic_header(uint32_t magic_header) { _magic_header = magic_header; }
};
