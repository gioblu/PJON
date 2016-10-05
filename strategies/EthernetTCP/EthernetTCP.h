
/* EthernetTCP is a Strategy for the PJON framework (included in version v6.0)
   It supports delivering PJON packets over Ethernet TCP.
   Compliant with the Padded jittering data link layer specification v0.4
   _____________________________________________________________________________

    Copyright 2012-2016 Giovanni Blu Mitolo gioscarab@gmail.com
    EthernetTCP strategy and EthernetLink added by Fred Larsen

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

#include "EthernetLink.h"
#include <PJONDefines.h>

class EthernetTCP {
  public:
    EthernetLink link;
    uint16_t last_send_result = FAIL;

    /* Caching of incoming packet to make it possible to deliver it byte for byte */
    
    uint8_t incoming_packet_buf[PACKET_MAX_LENGTH];
    uint16_t incoming_packet_size = 0;
    uint16_t incoming_packet_pos = 0;
    static void static_receiver(uint8_t id, const uint8_t *payload, uint8_t length, void *callback_object) {
      if (callback_object) ((EthernetTCP*)callback_object)->receiver(id, payload, length);
    }
    void receiver(uint8_t id, const uint8_t *payload, uint8_t length) {
      if (length <= PACKET_MAX_LENGTH) {
        memcpy(incoming_packet_buf, payload, length);
        incoming_packet_size = length;
        incoming_packet_pos = 0;
      }
    }
    
    EthernetTCP() {
      link.set_receiver(static_receiver, this);
    }
    
    /* Check if the channel is free for transmission */

    boolean can_start() {
      return link.device_id() != 0;
    };


    uint16_t receive_byte() {
      // Must receive a new packet, or is there more to serve from the last one?
      if (incoming_packet_pos >= incoming_packet_size) link.receive();

      // Deliver the next byte from the last received packet if any
      if (incoming_packet_pos < incoming_packet_size) {
        return incoming_packet_buf[incoming_packet_pos++];
      }
      return FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      return last_send_result;
    };


    /* Send byte response to package transmitter */

    void send_response(uint8_t response) { // Empty, ACK is always sent
    };
    

    /* Send a string: */

    void send_string(uint8_t *string, uint8_t length) {    
      if (length > 0)
        last_send_result = link.send((uint8_t)string[0], (const char*)string, length);
    };
};
