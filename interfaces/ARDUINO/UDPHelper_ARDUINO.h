#pragma once

#include <Ethernet.h>
#include <EthernetUDP.h>

class UDPHelper {
  uint16_t _port;
  uint32_t _magic_header;

  EthernetUDP udp;
  const uint8_t _broadcast[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

public:
  bool begin(uint16_t port) {
    _port = port;
    return udp.begin(_port);
  }

  uint16_t receive_string(uint8_t *string, uint16_t max_length) {
    uint16_t packetSize = udp.parsePacket();
    if(packetSize > 4 && packetSize <= 4 + max_length) {
      uint32_t header = 0;
      udp.read((char *) &header, 4);
      if(header != _magic_header) return false; // Not an expected packet
      udp.read(string, packetSize - 4);
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
    //send_string((const uint8_t *)&response, 1, udp.remoteIP(), udp.remotePort());
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write((const char*) &_magic_header, 4);
    udp.write((const char*) &response, 1);
    udp.endPacket();
  }

  void send_string(uint8_t *string, uint16_t length) {
    send_string(string, length, _broadcast, _port);
  }

  void send_string(uint8_t *string, uint16_t length, uint32_t remote_ip, uint16_t remote_port) {
    IPAddress address(remote_ip);
    send_string(string, length, address, remote_port);
  }

  void set_magic_header(uint32_t magic_header) { _magic_header = magic_header; }
};
