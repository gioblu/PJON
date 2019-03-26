
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

PJONMaster has been created to let slaves connect automatically to masters
in range. Masters can be connected together through switches, routers or
application servers. Devices can move in the network; disconnect from one
router and connect to another and remain uniquely identified a 5 bytes
unique device address.

See the PJON® Dynamic addressing specification:
- v3.0 specification/PJON-dynamic-addressing-specification-v3.0.md
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

// Reference for each slave
struct PJON_Slaves_reference {
  uint32_t registration = 0;
  uint8_t  device_address[5];
  uint8_t  state = 0;
};

template<typename Strategy>
class PJONMaster : public PJON<Strategy> {
  public:
    bool debug = false;
    PJON_Slaves_reference ids[PJON_MAX_DEVICES];

    /* PJONMaster bus default initialization:
       device id: MASTER (254) */

    PJONMaster() : PJON<Strategy>(PJON_MASTER_ID) {
      set_default();
    };

    /* PJONMaster initialization passing bus and device id:
       uint8_t my_bus = {1, 1, 1, 1};
       PJONMaster master(my_bys); */

    PJONMaster(const uint8_t *b_id) : PJON<Strategy>(b_id, PJON_MASTER_ID) {
      set_default();
    };

    /* Approve id sending PJON_ID_REQUEST, device address, id approved */

    void approve_id(uint8_t *b_id, uint8_t *da) {
      uint8_t id = reserve_id(da);
      uint8_t r[7] = {PJON_ID_REQUEST, da[0], da[1], da[2], da[3], da[4], id};
      PJON<Strategy>::send(
        PJON_NOT_ASSIGNED,
        b_id,
        (char *)r,
        7,
        PJON<Strategy>::config | (_required_config & ~PJON_TX_INFO_BIT),
        0,
        PJON_DYNAMIC_ADDRESSING_PORT
      );
    };

    /* Master begin function: */

    void begin() {
      PJON<Strategy>::begin();
      enable_advertisement();
    };

    /* Confirm device id insertion in list: */

    bool confirm_id(uint8_t id, uint8_t *device_address) {
      if(
        PJONTools::address_equality(
          ids[id - 1].device_address,
          device_address
        ) && (ids[id - 1].state > 0)
      ) {
        ids[id - 1].state = 1;
        return true;
      }
      return false;
    };

    /* Count active slaves: */

    uint8_t count_active_ids() {
      uint8_t c = 0;
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++)
        if(ids[i].state == 1) c++;
      return c;
    };

    /* Delete a device id reference from buffer: */

    void delete_id_reference(uint8_t id) {
      if(id > 0 && id < PJON_MAX_DEVICES) {
        ids[id - 1].registration = 0;
        ids[id - 1].state = 0;
      }
    };

    /* Disable continuous advertisement: */

    void disable_advertisement() {
      if(_adv < PJON_MAX_PACKETS)
        PJON<Strategy>::remove(_adv);
      _adv = PJON_MAX_PACKETS;
    };

    /* Enable continuous advertisement: */

    void enable_advertisement() {
      char request[] = {PJON_ID_DISCOVERY};
      if(_adv == PJON_MAX_PACKETS)
        _adv = PJON<Strategy>::send_repeatedly(
          PJON_BROADCAST,
          this->bus_id,
          (char *)request,
          1,
          PJON_DISCOVERY_INTERVAL,
          PJON<Strategy>::config | _required_config,
          0,
          PJON_DYNAMIC_ADDRESSING_PORT
        );
    };

    /* Master error handler: */

    void error(uint8_t code, uint16_t data) {
      _master_error(code, data, _custom_pointer);
      if(code == PJON_CONNECTION_LOST)
        delete_id_reference(PJON<Strategy>::packets[data].content[0]);
    };

    static void static_error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
      ((PJONMaster<Strategy>*)custom_pointer)->error(code, data);
    };

    /* Filter addressing packets from receive callback: */

    void filter(
      uint8_t *payload,
      uint16_t length,
      const PJON_Packet_Info &packet_info
    ) {
      PJON_Packet_Info p_i;
      memcpy(&p_i, &packet_info, sizeof(PJON_Packet_Info));
      p_i.custom_pointer = _custom_pointer;
      if(!handle_addressing()) {
        if(
          (p_i.sender_id != PJON_BROADCAST) &&
          (p_i.sender_id < PJON_MAX_DEVICES) &&
          (ids[p_i.sender_id - 1].state == 0)
        ) return negate_id(p_i.sender_id);
      } else if(!debug) return;
      _master_receiver(payload, length, p_i);
    };

    /* Handle addressing procedure if required: */

    bool handle_addressing() {
      bool filter = false;
      uint8_t overhead = PJON<Strategy>::packet_overhead(this->data[1]);
      uint8_t CRC_overhead = (this->data[1] & PJON_CRC_BIT) ? 4 : 1;
      if(
        (this->last_packet_info.header & PJON_PORT_BIT) &&
        (this->last_packet_info.header & PJON_TX_INFO_BIT) &&
        (this->last_packet_info.header & PJON_CRC_BIT) &&
        (this->last_packet_info.port == PJON_DYNAMIC_ADDRESSING_PORT)
      ) {
        filter = true;
        uint8_t request = this->data[overhead - CRC_overhead];
        uint8_t sender_id = this->last_packet_info.sender_id;
        uint32_t offset = (overhead - CRC_overhead) + 1;

        if(request == PJON_ID_REQUEST)
          approve_id(
            this->last_packet_info.sender_bus_id,
            (uint8_t*) &this->data[offset]
          );

        if(request == PJON_ID_CONFIRM)
          if(
            !confirm_id(
              sender_id,
              (uint8_t*) &this->data[offset]
            )
          ) negate_id(sender_id);

        if(request == PJON_ID_NEGATE)
          if(
              PJONTools::address_equality(
                (uint8_t*) &this->data[offset],
                ids[sender_id - 1].device_address
              )
            ) delete_id_reference(sender_id);
      }
      return filter;
    };

    /* Remove reserved id which expired (never confirmed ids): */

    void free_reserved_ids_expired() {
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++)
        if(ids[i].state == 255) {
          if(
            (uint32_t)(PJON_MICROS() - ids[i].registration) <
            PJON_ADDRESSING_TIMEOUT
          ) continue;
          else delete_id_reference(i + 1);
        }
    };

    /* Get device id from RID: */

    uint8_t get_index_from_device_address(uint8_t *device_address) {
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++)
        if(
          ids[i].state == 1 &&
          PJONTools::address_equality(ids[i].device_address, device_address))
          return i;
      return PJON_MAX_DEVICES;
    };

    /* Negate a device id sending a packet to the slave containing
       PJON_ID_NEGATE forcing it to abandon its own device id. */

    void negate_id(uint8_t id) {
      char response[] = {PJON_ID_NEGATE};
      PJON<Strategy>::send(
        id,
        this->bus_id,
        (char *)response,
        1,
        PJON<Strategy>::config | _required_config,
        0,
        PJON_DYNAMIC_ADDRESSING_PORT
      );
      // Keep id busy to avoid in flight packets to reach the wrong device
      ids[id - 1].registration = PJON_MICROS();
      ids[id - 1].state = 255;
    };

    /* Reserve a device id and wait for its confirmation: */

    uint8_t reserve_id(uint8_t *device_address) {
      uint8_t index = get_index_from_device_address(device_address);
      // If already present pass its index.
      if(ids[index].state == 1) return index + 1;
      /* When using unreliable data-links this should help accomodate
      occasional losses and avoid numerous re-negotiations, consider that
      no measure is taken to avoid potential device address collisions. */
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++)
        if(ids[i].state == 0) {
          ids[i].registration = PJON_MICROS();
          PJONTools::copy_address(ids[i].device_address, device_address);
          ids[i].state = 255;
          return i + 1;
        }
      error(PJON_DEVICES_BUFFER_FULL, PJON_MAX_DEVICES);
      return 0;
    };

    /* Master receive function: */

    uint16_t receive() {
      return PJON<Strategy>::receive();
    };

    /* Try to receive a packet repeatedly for a given duration: */

    uint16_t receive(uint32_t duration) {
      return PJON<Strategy>::receive(duration);
    };

    /* Static receiver hander: */

    static void static_receiver_handler(
      uint8_t *payload,
      uint16_t length,
      const PJON_Packet_Info &packet_info
    ) {
      (
        (PJONMaster<Strategy>*)packet_info.custom_pointer
      )->filter(payload, length, packet_info);
    };

    /* Set custom pointer: */

    void set_custom_pointer(void *p) {
      _custom_pointer = p;
    };

    /* Set default configuration: */

    void set_default() {
      PJON<Strategy>::set_default();
      PJON<Strategy>::set_custom_pointer(this);
      PJON<Strategy>::set_error(static_error_handler);
      PJON<Strategy>::set_receiver(static_receiver_handler);
      for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
        ids[i].registration = 0;
        ids[i].state = 0;
      }
    };

    /* Master receiver function setter: */

    void set_receiver(PJON_Receiver r) {
      _master_receiver = r;
    };

    /* Master error receiver function: */

    void set_error(PJON_Error e) {
      _master_error = e;
    };

    /* Master packet handling update: */

    uint8_t update() {
      free_reserved_ids_expired();
      return PJON<Strategy>::update();
    };

  private:
    uint16_t        _adv = PJON_MAX_PACKETS;
    void           *_custom_pointer;
    PJON_Receiver   _master_receiver;
    PJON_Error      _master_error;
    uint8_t         _required_config =
      (PJON_TX_INFO_BIT | PJON_CRC_BIT | PJON_ACK_REQ_BIT | PJON_PORT_BIT) &
      ~(PJON_ACK_MODE_BIT | PJON_PACKET_ID_BIT)
    ;
};
