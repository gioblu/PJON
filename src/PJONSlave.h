
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| v11.2
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2019 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-__________________________________________________________________

 PJONSlave class implements the dynamic addressing specification:
 - v3.0 specification/PJON-dynamic-addressing-specification-v3.0.md

 Slaves can move through the network; waive a device id obtained by a master
 and request another from a reachable master while remaining uniquely
 identified a 5 bytes unique device address.
 _____________________________________________________________________________

This software is experimental and it is distributed "AS IS" without any
warranty, use it at your own risk.

Copyright 2010-2019 by Giovanni Blu Mitolo gioscarab@gmail.com

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
#include "PJON.h"

template<typename Strategy>
class PJONSlave : public PJON<Strategy> {
  public:
    bool connected = false;

    /* PJONSlave bus default initialization:
       State:     Shared (bus_id: 0.0.0.0)
       device id: PJON_NOT_ASSIGNED (255)
       Mode:      PJON_HALF_DUPLEX */

    PJONSlave(uint8_t *device_address) : PJON<Strategy>() {
      set_default();
      PJONTools::copy_address(_da, device_address);
    };

    /* Basic single byte request to master: */

    bool send_to_master(uint8_t req) {
      connected = false;
      uint8_t r[] = {req, _da[0], _da[1], _da[2], _da[3], _da[4]};
      if(this->send_packet_blocking(
        PJON_MASTER_ID,
        this->bus_id,
        r,
        6,
        this->config | _required_config,
        0,
        PJON_DYNAMIC_ADDRESSING_PORT
      ) == PJON_ACK) return true;
      error(PJON_ID_ACQUISITION_FAIL, req);
      return false;
    };

    /* Begin function to be called in setup: */

    void begin() {
      PJON<Strategy>::begin();
    };

    /* Release device id (Master-slave only): */

    bool discard_device_id() {
      if(!send_to_master(PJON_ID_NEGATE)) return false;
      this->device_id = PJON_NOT_ASSIGNED;
      return true;
    };

    /* Error callback: */

    void error(uint8_t code, uint16_t data) {
      _slave_error(code, data, _custom_pointer);
    };

    /* Filter incoming addressing packets callback: */

    void filter(
      uint8_t *payload,
      uint16_t length,
      const PJON_Packet_Info &packet_info
    ) {
      if(!handle_addressing()) {
         PJON_Packet_Info p_i;
         memcpy(&p_i, &packet_info, sizeof(PJON_Packet_Info));
         p_i.custom_pointer = _custom_pointer;
        _slave_receiver(payload, length, p_i);
      }
    };

    /* Handle dynamic addressing requests and responses: */

    bool handle_addressing() {
      if( // Handle master-slave dynamic addressing
        (this->last_packet_info.header & PJON_PORT_BIT) &&
        (this->last_packet_info.header & PJON_CRC_BIT) &&
        (this->last_packet_info.port == PJON_DYNAMIC_ADDRESSING_PORT) &&
        (this->last_packet_info.sender_id == PJON_MASTER_ID)
      ) {
        uint8_t overhead =
          this->packet_overhead(this->last_packet_info.header);
        uint8_t CRC_overhead =
          (this->last_packet_info.header & PJON_CRC_BIT) ? 4 : 1;
        overhead = overhead - CRC_overhead;
        if(!connected) {
          if(this->data[overhead] == PJON_ID_DISCOVERY) {
            PJONTools::copy_bus_id(
              this->bus_id,
              this->last_packet_info.receiver_bus_id
            );
            send_to_master(PJON_ID_REQUEST);
            return true;
          }
          if(
            (this->data[overhead] == PJON_ID_REQUEST) &&
            PJONTools::address_equality(&this->data[(overhead + 1)], _da)
          ) {
            // slave acks if finds its own device address
            this->strategy.send_response(PJON_ACK);
            if(this->data[overhead + 6] > 0) {
              this->set_id(this->data[overhead + 6]);
              if(!send_to_master(PJON_ID_CONFIRM)) {
                this->set_id(PJON_NOT_ASSIGNED);
                error(PJON_ID_ACQUISITION_FAIL, PJON_ID_CONFIRM);
              } else {
                this->set_router(false);
                connected = true;
              }
            } else error(PJON_ID_ACQUISITION_FAIL, PJON_DEVICES_BUFFER_FULL);
          }
        } else if(this->data[overhead] == PJON_ID_NEGATE) set_default();
        return true;
      }
      return false;
    };

    /* Slave receive function: */

    uint16_t receive() {
      return PJON<Strategy>::receive();
    };

    /* Try to receive a packet repeatedly with a maximum duration: */

    uint16_t receive(uint32_t duration) {
      return PJON<Strategy>::receive(duration);
    };

    /* Set custom pointer: */

    void set_custom_pointer(void *p) {
      _custom_pointer = p;
    }

    /* Set default configuration: */

    void set_default() {
      /* All packets in buffer are deleted by the next line because they may
         be addressed erroneously from now on */
      PJON<Strategy>::set_default();
      PJON<Strategy>::set_shared_network(true);
      PJON<Strategy>::set_custom_pointer(this);
      PJON<Strategy>::set_receiver(static_receiver_handler);
      PJON<Strategy>::set_error(static_error_handler);
      PJON<Strategy>::set_router(true);
      connected = false;
      PJONTools::copy_bus_id(
        this->bus_id,
        this->localhost
      );
      this->set_id(PJON_NOT_ASSIGNED);
    };

    /* Slave receiver function setter: */

    void set_receiver(PJON_Receiver r) {
      _slave_receiver = r;
    };

    /* Master error receiver function: */

    void set_error(PJON_Error e) {
      _slave_error = e;
    };

    /* Static receiver hander: */

    static void static_receiver_handler(
      uint8_t *payload,
      uint16_t length,
      const PJON_Packet_Info &packet_info
    ) {
      (
        (PJONSlave<Strategy>*)packet_info.custom_pointer
      )->filter(payload, length, packet_info);
    };

    /* Static error hander: */

    static void static_error_handler(
      uint8_t code,
      uint16_t data,
      void *custom_pointer
    ) {
      ((PJONSlave<Strategy>*)custom_pointer)->error(code, data);
    };

    /* Slave packet handling update: */

    uint8_t update() {
      return PJON<Strategy>::update();
    };

  private:
    uint8_t       _da[5];
    void         *_custom_pointer;
    PJON_Error    _slave_error;
    PJON_Receiver _slave_receiver;
    /* Required configuration for the dynamic addressing procedure */
    uint8_t       _required_config =
      (PJON_TX_INFO_BIT | PJON_CRC_BIT | PJON_ACK_REQ_BIT | PJON_PORT_BIT) &
      ~(PJON_ACK_MODE_BIT | PJON_PACKET_ID_BIT)
    ;
};
