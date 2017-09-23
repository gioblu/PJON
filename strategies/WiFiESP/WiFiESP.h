
/* WiFiESP is a Strategy for the PJON framework (included in version v5.2)
   It supports delivering PJON packets over Ethernet TCP.
   Compliant with the PJON protocol layer specification v0.3
   _____________________________________________________________________________

    WiFiESP strategy and EthernetLink proposed and developed by Fred Larsen
    02/10/2016

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

#include "WiFiLink.h"
#include <PJONDefines.h>

class WiFiESP {
  public:
    WiFiLink link;
    uint16_t last_send_result = PJON_FAIL;

    /* Caching incoming packet to enable byte for byte delivery */

    uint8_t *incoming_packet_buf_ptr = NULL;
    uint16_t current_buffer_size = 0;
    uint16_t incoming_packet_size = 0;

    static void static_receiver(
      uint8_t id,
      const uint8_t *payload,
      uint16_t length,
      void *callback_object
    ) {
      if(callback_object)
        ((WiFiESP*)callback_object)->receiver(id, payload, length);
    };


    void receiver(uint8_t id, const uint8_t *payload, uint16_t length) {
      if(length <= current_buffer_size && incoming_packet_buf_ptr != NULL) {
        memcpy(incoming_packet_buf_ptr, payload, length);
        incoming_packet_size = length;
      }
    };


    WiFiESP() {
      link.set_receiver(static_receiver, this);
    };


    /* Returns the suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      return 1;
    };


    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t additional_randomness = 0) {
      return true;
    };


    /* Check if the channel is free for transmission */

    bool can_start() {
      return link.device_id() != 0;
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return 1;
    };


    /* Handle a collision (empty because handled on Ethernet level): */

    void handle_collision() { };


    /* Receive a string: */

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {
      // Register supplied buffer as target for EthernetLink callback function
      incoming_packet_buf_ptr = string;
      current_buffer_size = max_length;
      incoming_packet_size = 0;

      // Receive a packet
      uint16_t result = link.receive();

      // Forget about buffer and return result size
      uint16_t received_packet_size = incoming_packet_size;
      incoming_packet_buf_ptr = NULL;
      incoming_packet_size = 0;
      return result == PJON_ACK ? received_packet_size : PJON_FAIL;
    }


    /* Receive byte response */

    uint16_t receive_response() {
      return last_send_result;
    };


    /* Send byte response to package transmitter */

    void send_response(uint8_t response) { // Empty, ACK is always sent
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint16_t length) {
      if(length > 0)
        last_send_result =
          link.send((uint8_t)string[0], (const char*)string, length);
    };
};
