
/* LocalUDP is a Strategy for the PJON framework (included in version v5.2)
   It supports delivering PJON packets over Ethernet UDP on the local network (LAN).
   Compliant with the PJON protocol layer specification v0.3
   _____________________________________________________________________________

    LocalUDP strategy proposed and developed by Fred Larsen 02/10/2016

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License. */

#pragma once

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <PJONDefines.h>

#define DEFAULT_UDP_PORT             7100
#define RESPONSE_TIMEOUT  (uint32_t) 10000
#define UDP_MAGIC_HEADER             0x0DFAC3D0

/* Maximum transmission attempts */
#ifndef LUDP_MAX_ATTEMPTS
  #define LUDP_MAX_ATTEMPTS          20
#endif

/* Back-off exponential degree */
#ifndef LUDP_BACK_OFF_DEGREE
  #define LUDP_BACK_OFF_DEGREE       4
#endif

class LocalUDP {
    bool _udp_initialized = false;
    uint16_t _port = DEFAULT_UDP_PORT;
    const uint32_t _magic_header = UDP_MAGIC_HEADER;
    const uint8_t _broadcast[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

    EthernetUDP udp;

    /* Caching of incoming packet to make it possible to deliver it byte for byte */

    uint8_t incoming_packet_buf[PJON_PACKET_MAX_LENGTH];
    uint16_t incoming_packet_size = 0;
    uint16_t incoming_packet_pos = 0;

    bool receive_telegram() {
      int packetSize = udp.parsePacket();
      if (packetSize > 4 && packetSize <= PJON_PACKET_MAX_LENGTH) {
        uint32_t header = 0;
        udp.read((char *) &header, 4);
        if (header != _magic_header) return false; // Not a LocalUDP packet
        udp.read(incoming_packet_buf, PJON_PACKET_MAX_LENGTH);
        incoming_packet_size = packetSize;
        incoming_packet_pos = 0;
        return true;
      }
      return false;
    }

    void empty_buffer() {
      incoming_packet_size = incoming_packet_pos = 0;
    };

    void check_udp() {
      if (!_udp_initialized) {
        udp.begin(_port);
        _udp_initialized = true;
      }
    };

public:
    LocalUDP() { };

    /* Returns the suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < LUDP_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };


    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    boolean begin(uint8_t additional_randomness = 0) {
      return true;
    };


    /* Check if the channel is free for transmission */

    boolean can_start() {
      check_udp();
      return true;
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return LUDP_MAX_ATTEMPTS;
    };


    /* Handle a collision (empty because handled on Ethernet level): */

    void handle_collision() { };


    uint16_t receive_byte() {
      check_udp();
      // Must receive a new packet, or is there more to serve from the last one?
      if (incoming_packet_pos >= incoming_packet_size)
        receive_telegram();
      // Deliver the next byte from the last received packet if any
      if (incoming_packet_pos < incoming_packet_size)
        return incoming_packet_buf[incoming_packet_pos++];
      return PJON_FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      // This should not be needed, but empty buffer so that we are sure to pick up a new packet.
      empty_buffer();
      // TODO: Improve robustness by ignoring packets not from the previous receiver
      // (Perhaps not that important as long as ACK/NAK responses are directed, not broadcast)
      uint32_t start = micros();
      uint16_t result = PJON_FAIL;
      do {
        result = receive_byte();
        if (result == ACK || result == NAK) return result;
     } while ((uint32_t)(micros() - start) < RESPONSE_TIMEOUT);
      return result;
    };


    /* Send byte response to package transmitter.
       We have the IP so we can skip broadcasting and reply directly. */

    void send_response(uint8_t response) { // Empty, ACK is always sent
      udp.beginPacket(udp.remoteIP(), _port);
      udp.write((const char*) &_magic_header, 4);
      udp.write((const char*) &response, 1);
      udp.endPacket();
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint16_t length) {
      if (length > 0) {
        udp.beginPacket(_broadcast, _port);
        udp.write((const char*) &_magic_header, 4);
        udp.write(string, length);
        udp.endPacket();
      }
    };


    /* Set the UDP port: */

    void set_port(uint16_t port = DEFAULT_UDP_PORT) {
      _port = port;
    };
};
