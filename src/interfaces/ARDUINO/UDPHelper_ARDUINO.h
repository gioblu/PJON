#pragma once

#ifdef PJON_ESP
  #if defined(ESP32)
    #include <WiFi.h>
  #else
    #include <ESP8266WiFi.h>
  #endif
  #include <WiFiUdp.h>
  #define PJON_ESP
#else
  #ifdef PJON_ETHERNET2
    #include <Ethernet2.h>
    #include <EthernetUdp2.h>
  #else
    #include <Ethernet.h>
    #include <EthernetUdp.h>
  #endif
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

  uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
    uint16_t result = PJON_FAIL;
    int16_t packet_size = udp.parsePacket();
    if(packet_size > 0) {
      uint32_t header = 0;
      int16_t len = udp.read((char *) &header, 4), remaining = packet_size - len;
      if (len == -1) return result;
      if(len == 4 && header == _magic_header && packet_size <= 4 + max_length) {
        len = udp.read(data, packet_size - 4);
        if (len == -1) return result;
        if (len == packet_size - 4) result = packet_size - 4;
        remaining -= len;
      }
      if (remaining > 0) { 
        // We must still read every byte because some implementations 
        // (like ESP32) will not clear the receive buffer and therefore will 
        // not receive any more packets otherwise.
        while (remaining > 0 && len > 0) {
          len = udp.read(data, remaining <= max_length ? remaining : max_length);
          if (len > 0) remaining -= len;
        }
      }
    }
    return result;
  }

  void send_frame(uint8_t *data, uint16_t length, IPAddress remote_ip, uint16_t remote_port) {
    if(length > 0) {
      udp.beginPacket(remote_ip, remote_port);
      #if defined(ESP32)
        udp.write((const unsigned char*) &_magic_header, 4);
      #else
        udp.write((const char*) &_magic_header, 4);
      #endif
      udp.write(data, length);
      udp.endPacket();
    }
  }

  void send_response(uint8_t *data, uint16_t length) {
    send_frame(data, length, udp.remoteIP(), udp.remotePort());
  }

  void send_response(uint8_t response) {
    send_frame(&response, 1, udp.remoteIP(), udp.remotePort());
  }

  void send_frame(uint8_t *data, uint16_t length, uint8_t remote_ip[], uint16_t remote_port) {
    IPAddress address(remote_ip);
    send_frame(data, length, address, remote_port);
  }

  void send_frame(uint8_t *data, uint16_t length) {
    // Broadcast on local subnet, global broadcast may not be accepted
    IPAddress broadcastIp;
#ifdef PJON_ESP
    // 255.255.255.255 may not work, be more specific
    broadcastIp = ~(uint32_t)WiFi.subnetMask() | (uint32_t)WiFi.localIP();
#else
    broadcastIp = _broadcast;
#endif
    send_frame(data, length, broadcastIp, _port);
  }

  void set_magic_header(uint32_t magic_header) { _magic_header = magic_header; }

  void get_sender(uint8_t *ip, uint16_t &port) {
    uint32_t ip_address = udp.remoteIP();
    memcpy(ip, &ip_address, 4);
    port = udp.remotePort();
  }
};
