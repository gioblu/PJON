
/* EthernetUDP is a Strategy for the PJON framework.
   It supports delivering PJON packets through Ethernet UDP to a registered list
   of devices on the LAN, WAN or Internet. Each device must be registered with
   its device id, IP address and listening port number.
   ___________________________________________________________________________

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

#ifdef HAS_ETHERNETUDP
  #include <interfaces/ARDUINO/UDPHelper_ARDUINO.h>
#else
  #include <interfaces/LINUX/UDPHelper_POSIX.h>
#endif

#include <PJONDefines.h>

// Timeout waiting for an ACK. This can be increased if the latency is high.
#ifndef GUDP_RESPONSE_TIMEOUT
  #define GUDP_RESPONSE_TIMEOUT         100000ul
#endif

#ifndef GUDP_MAX_REMOTE_NODES
  #define GUDP_MAX_REMOTE_NODES               10
#endif

#define GUDP_DEFAULT_PORT                    7000
#define GUDP_MAGIC_HEADER   (uint32_t) 0x0DFAC3FF

class GlobalUDP {
    bool _udp_initialized = false;
    uint16_t _port = GUDP_DEFAULT_PORT;
    bool _auto_registration = true;

    // Remote nodes
    uint8_t  _remote_node_count = 0;
    uint8_t  _remote_id[GUDP_MAX_REMOTE_NODES];
    uint8_t  _remote_ip[GUDP_MAX_REMOTE_NODES][4];
    uint16_t _remote_port[GUDP_MAX_REMOTE_NODES];

    UDPHelper udp;

    bool check_udp() {
      if(!_udp_initialized) {
        udp.set_magic_header(htonl(GUDP_MAGIC_HEADER));
        if (udp.begin(_port)) _udp_initialized = true;
      }
      return _udp_initialized;
    };

    int16_t find_remote_node(uint8_t id) {
      for(uint8_t i = 0; i < _remote_node_count; i++)
        if(_remote_id[i] == id)
          return i;
      return -1;
    };

    void autoregister_sender(const uint8_t *message, uint16_t length) {
      // Add the last sender to the node table
      if (_auto_registration && length>4) {
        // First get PJON sender id from incoming packet
        PJON_Packet_Info packet_info;
        PJONTools::parse_header(message, packet_info);
        uint8_t sender_id = packet_info.sender_id;
        if (sender_id == 0) return; // If parsing fails, it will be 0

        // Then get the IP address and port number of the sender
        uint8_t sender_ip[4];
        uint16_t sender_port;
        udp.get_sender(sender_ip, sender_port);

        // See if PJON id is already registered, add if not
        int16_t pos = find_remote_node(sender_id);
        if (pos == -1) add_node(sender_id, sender_ip, sender_port);
        else {
          // Update IP and port of existing node
          memcpy(_remote_ip[pos], sender_ip, 4);
          _remote_port[pos] = sender_port; 
        }
      }
    }

public:

    /* Register each device we want to send to */

    int16_t add_node(
      uint8_t remote_id,
      const uint8_t remote_ip[],
      uint16_t port_number = GUDP_DEFAULT_PORT
    ) {
      if (_remote_node_count == GUDP_MAX_REMOTE_NODES) return -1;
      _remote_id[_remote_node_count] = remote_id;
      memcpy(_remote_ip[_remote_node_count], remote_ip, 4);
      _remote_port[_remote_node_count] = port_number;
      _remote_node_count++;
      return _remote_node_count - 1;
    };


    /* Select if incoming packets should automatically add their sender as a node */

    void set_autoregistration(bool enabled) {
      _auto_registration = enabled;
    }


    /* Returns the suggested delay related to attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      #ifdef PJON_ESP
        return 10000ul * attempts + PJON_RANDOM(10000);
      #elif _WIN32
        (void)attempts; // Avoid "unused parameter" warning
        return 1000ul + PJON_RANDOM(1000);
      #else
        (void)attempts; // Avoid "unused parameter" warning
        return 1;
      #endif
    };


    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t /*additional_randomness*/ = 0) { return check_udp(); };


    /* Check if the channel is free for transmission */

    bool can_start() { return check_udp(); };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() { return 10; };


    /* Handle a collision (empty because handled on Ethernet level): */

    void handle_collision() { };


    /* Receive a string: */

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {
      uint16_t length = udp.receive_string(string, max_length);
      if (length != PJON_FAIL) autoregister_sender(string, length);
      return length;
    }


    /* Receive byte response */

    uint16_t receive_response() {
      /* TODO: Improve robustness by ignoring packets not from the previous
         receiver (Perhaps not that important as long as ACK/NAK responses are
         directed, not broadcast) */
      uint32_t start = PJON_MICROS();
      uint8_t result[8];
      uint16_t reply_length = 0;
      do {
        reply_length = receive_string(result, sizeof result);

        // We expect 1, if packet is larger it is not our ACK
        if(reply_length == 1)
          if (result[0] == PJON_ACK)
            return result[0];

      } while ((uint32_t)(PJON_MICROS() - start) < GUDP_RESPONSE_TIMEOUT);
      return PJON_FAIL;
    };


    /* Send byte response to package transmitter.
       We have the IP so we can reply directly. */

    void send_response(uint8_t response) { // Empty, PJON_ACK is always sent
      udp.send_response(response);
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint16_t length) {
      if(length > 0) {
        uint8_t id = string[0]; // Package always starts with a receiver id
        if (id == 0) { // Broadcast, send to all receivers
          for(uint8_t pos = 0; pos < _remote_node_count; pos++)
            udp.send_string(string, length, _remote_ip[pos], _remote_port[pos]);
        } else { // To a specific receiver
          int16_t pos = find_remote_node(id);
          if (pos != -1) {
            udp.send_string(string, length, _remote_ip[pos], _remote_port[pos]);
          }
        }
      }
    };


    /* Set the UDP port: */

    void set_port(uint16_t port = GUDP_DEFAULT_PORT) {
      _port = port;
    };
};
