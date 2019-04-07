
/* ESPNOW is a Strategy for the PJON framework.
   It supports delivering PJON packets through the Espressif ESPNOW 802.11
   protocol to a registered list of devices on the 802.11 ESPNOW network. Each
   device must be registered with its device id and mac address.
   ___________________________________________________________________________

    ESPNOW strategy proposed and developed by xlfe in September 2018

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
#include "../../interfaces/ARDUINO/ESPNOWHelper.h"

// Timeout waiting for an ACK. This can be increased if the latency is high.
#ifndef EN_RESPONSE_TIMEOUT
  #define EN_RESPONSE_TIMEOUT         100000ul
#endif

#ifndef EN_MAX_REMOTE_NODES
  #define EN_MAX_REMOTE_NODES               10
#endif

#define EN_MAGIC_HEADER (uint8_t*)"\xEE\xFE\x0E\xEF"

class ESPNOW {
    bool _espnow_initialised = false;
    bool _auto_registration = true;
    uint8_t _channel = 14;
    char _espnow_pmk[17] =
      "\xdd\xdb\xdd\x44\x34\xd5\x6a\x0b\x7e\x9f\x4e\x27\xd6\x5b\xa2\x81";

    // Remote nodes
    uint8_t  _remote_node_count = 0;
    uint8_t  _remote_id[EN_MAX_REMOTE_NODES];
    uint8_t  _remote_mac[EN_MAX_REMOTE_NODES][ESP_NOW_ETH_ALEN];

    ENHelper en;

    bool check_en() {
      if(!_espnow_initialised) {
        en.set_magic_header(EN_MAGIC_HEADER);
        if(en.begin(_channel,(uint8_t*)_espnow_pmk))
          _espnow_initialised = true;
      }
      return _espnow_initialised;
    };

    int16_t find_remote_node(uint8_t id) {
      for(uint8_t i = 0; i < _remote_node_count; i++)
        if(_remote_id[i] == id)
          return i;
      return -1;
    };

    void autoregister_sender(const uint8_t *message, uint16_t length) {
      // Add the last sender to the node table
      if(_auto_registration && length>4) {
        // First get PJON sender id from incoming packet
        PJON_Packet_Info packet_info;
        PJONTools::parse_header(message, packet_info);
        uint8_t sender_id = packet_info.sender_id;
        if(sender_id == 0) {
          ESP_LOGE("ESPNOW", "AutoRegister parsing failed");
          return; // If parsing fails, it will be 0
        }

        // Then get the mac address of the sender
        uint8_t sender_mac[ESP_NOW_ETH_ALEN];
        en.get_sender(sender_mac);

        // See if PJON id is already registered, add if not
        int16_t pos = find_remote_node(sender_id);
        if(pos == -1) {
          ESP_LOGI("ESPNOW", "Autoregister new sender %d",sender_id);
          add_node(sender_id, sender_mac);
        }
        else if(memcmp(_remote_mac[pos], sender_mac, ESP_NOW_ETH_ALEN) != 0) {
          // Update mac of existing node
          ESP_LOGI(
            "ESPNOW",
            "Update sender mac %d %d:%d:%d",
            sender_id,
            sender_mac[1],
            sender_mac[2],
            sender_mac[3]
          );
          memcpy(_remote_mac[pos], sender_mac, ESP_NOW_ETH_ALEN);
        }
      }
    };

public:

    void set_pmk(char *espnow_pmk) {
      memcpy(_espnow_pmk, espnow_pmk, 16);
    }

    void set_channel(uint8_t channel) {
      _channel = channel;
    }

    /* Register each device we want to send to */

    int16_t add_node(
      uint8_t remote_id,
      uint8_t remote_mac[]
    ) {
      if(_remote_node_count == EN_MAX_REMOTE_NODES) return -1;
      _remote_id[_remote_node_count] = remote_id;
      memcpy(_remote_mac[_remote_node_count], remote_mac, ESP_NOW_ETH_ALEN);
      en.add_node_mac(remote_mac);
      _remote_node_count++;
      return _remote_node_count - 1;
    };


    /* Set if each packet's sender is automatically added as a node */

    void set_autoregistration(bool enabled) {
      _auto_registration = enabled;
    };


    /* Returns the suggested delay related to attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      return 10000ul * attempts + PJON_RANDOM(10000);
    };


    /* Begin method, to be called on initialization:
       (returns always true) */

    bool begin(uint8_t /*did*/ = 0) { return check_en(); };


    /* Check if the channel is free for transmission */

    bool can_start() { return check_en(); };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() { return 10; };


    /* Handle a collision (empty because handled on Ethernet level): */

    void handle_collision() { };


    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      uint16_t length = en.receive_frame(data, max_length);
      if(length != PJON_FAIL) autoregister_sender(data, length);
      return length;
    }


    /* Receive byte response */

    uint16_t receive_response() {
      /* TODO: Improve robustness by ignoring packets not from the previous
         receiver (Perhaps not that important as long as ACK/NAK responses are
         directed, not broadcast) */
      uint32_t start = PJON_MICROS();
      uint8_t result[PJON_PACKET_MAX_LENGTH];
      uint16_t reply_length = 0;
      do {
        reply_length = receive_frame(result, sizeof result);
        // We expect 1, if packet is larger it is not our ACK
        if(reply_length == 1)
          if(result[0] == PJON_ACK)
            return result[0];

      } while ((uint32_t)(PJON_MICROS() - start) < EN_RESPONSE_TIMEOUT);
      return PJON_FAIL;
    };


    /* Send byte response to package transmitter.
       We have the IP so we can reply directly. */

    void send_response(uint8_t response) { // Empty, PJON_ACK is always sent
      en.send_response(response);
    };


    /* Send a frame: */

    void send_frame(uint8_t *data, uint16_t length) {
      if(length > 0) {
        uint8_t id = data[0]; // Package always starts with a receiver id
        if(id == 0) { // Broadcast, send to all receivers
          en.send_frame(data, length);
        } else { // To a specific receiver
          int16_t pos = find_remote_node(id);
          if(pos != -1)
            en.send_frame(data, length, _remote_mac[pos]);
          else //Broadcast - any replies will get registered
            en.send_frame(data, length);
        }
      }
    };
};
