
/* EthernetUDP is a Strategy for the PJON framework.
   It supports delivering PJON packets over Ethernet UDP to a registered list
   of devices on the LAN, WAN or Internet. Each device must be registered with its
   device id, IP address and listening port number.
   Compliant with the PJON protocol layer specification v0.3
   _____________________________________________________________________________

    EthernetUDP strategy proposed and developed by Fred Larsen 01/09/2017

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

#define EUDP_DEFAULT_PORT                  7000
#define EUDP_RESPONSE_TIMEOUT  (uint32_t) 20000
#define EUDP_MAGIC_HEADER            0x0DFAC3FF

#ifndef EUDP_MAX_REMOTE_NODES
  #define EUDP_MAX_REMOTE_NODES                10
#endif

class EthUDP {
    bool _udp_initialized = false;
    uint16_t _port = EUDP_DEFAULT_PORT;
    const uint32_t _magic_header = EUDP_MAGIC_HEADER;

    // Remote nodes
    uint8_t  _remote_node_count = 0;
    uint8_t  _remote_id[EUDP_MAX_REMOTE_NODES];
    uint8_t  _remote_ip[EUDP_MAX_REMOTE_NODES][4];
    uint16_t _remote_port[EUDP_MAX_REMOTE_NODES];

    EthernetUDP udp;

    bool check_udp() {
      if(!_udp_initialized) {
        udp.begin(_port);
        _udp_initialized = true;
      }
      return _udp_initialized;
    };

    int16_t find_remote_node(uint8_t id) {
      for(uint8_t i = 0; i < _remote_node_count; i++)
        if(_remote_id[i] == id)
          return i;
      return -1;
    };

public:
    EthUDP() { };


    /* Register each device we want to send to */

    int16_t add_node(
      uint8_t remote_id,
      const uint8_t remote_ip[],
      uint16_t port_number = EUDP_DEFAULT_PORT
    ) {
      if (_remote_node_count == EUDP_MAX_REMOTE_NODES) return -1;
      _remote_id[_remote_node_count] = remote_id;
      memcpy(_remote_ip[_remote_node_count], remote_ip, 4);
      _remote_port[_remote_node_count] = port_number;
      _remote_node_count++;
      return _remote_node_count - 1;
    };


    /* Returns the suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      return 1;
    };


    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t additional_randomness = 0) { return check_udp(); };


    /* Check if the channel is free for transmission */

    bool can_start() { return check_udp(); };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() { return 1; };


    /* Handle a collision (empty because handled on Ethernet level): */

    void handle_collision() { };


    /* Receive a string: */

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {
      uint16_t packetSize = udp.parsePacket();
      if(packetSize > 4 && packetSize <= 4 + max_length) {
        uint32_t header = 0;
        udp.read((char *) &header, 4);
        if(header != _magic_header) return false; // Not a EthUDP packet
        udp.read(string, packetSize - 4);
        return packetSize - 4;
      }
      return PJON_FAIL;
    }


    /* Receive byte response */

    uint16_t receive_response() {
      /* TODO: Improve robustness by ignoring packets not from the previous
         receiver (Perhaps not that important as long as ACK/NAK responses are
         directed, not broadcast) */
      uint32_t start = PJON_MICROS();
      uint8_t result[2];
      uint16_t reply_length = 0;
      do {
        reply_length = receive_string(result, 2);
        // We expect 1, if packet is larger it is not our ACK
        if(reply_length == 1)
          if(result[0] == PJON_ACK)
            return result[0];
     } while ((uint32_t)(PJON_MICROS() - start) < EUDP_RESPONSE_TIMEOUT);
      return PJON_FAIL;
    };


    /* Send byte response to package transmitter.
       We have the IP so we can reply directly. */

    void send_response(uint8_t response) { // Empty, PJON_ACK is always sent
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write((const char*) &_magic_header, 4);
      udp.write((const char*) &response, 1);
      udp.endPacket();
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint16_t length) {
      if(length > 0) {
        uint8_t id = string[0],
                pos = find_remote_node(id); // Package always starts with a receiver id byte
        if (pos != -1) {
          udp.beginPacket(_remote_ip[pos], _remote_port[pos]);
          udp.write((const char*) &_magic_header, 4);
          udp.write(string, length);
          udp.endPacket();
        }
      }
    };


    /* Set the UDP port: */

    void set_port(uint16_t port = EUDP_DEFAULT_PORT) {
      _port = port;
    };
};
