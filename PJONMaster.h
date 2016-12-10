
 /*-O//\             __     __
   |-gfo\           |__| | |  | |\ | ™
   |!y°o:\          |  __| |__| | \| v6.1
   |y"s§+`\         multi-master, multi-media communications bus system framework
  /so+:-..`\        Copyright 2010-2016 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-___________________________________________________________________

PJONMaster has been created by Giovanni Blu Mitolo with the support
of Fred Larsen and is inspired by the work of Thomas Snaidero:
"Modular components for eye tracking, in the interest of helping persons with severely impaired motor skills."
Master Thesis, IT University of Copenhagen, Denmark, September 2016

PJON™ Protocol specification:
- v0.1 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.1.md
- v0.2 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.2.md
- v0.3 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.3.md
- v1.0 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md

PJON™ Acknowledge specification:
- v0.1 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md

PJON™ Dynamic addressing specification:
- v0.1 https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md

PJON™ Standard compliant tools:
- https://github.com/aperepel/saleae-pjon-protocol-analyzer Logic analyzer by Andrew Grande
- https://github.com/Girgitt/PJON-python PJON running on Python by Zbigniew Zasieczny

PJON™ is a self-funded, no-profit project created and mantained by Giovanni Blu Mitolo
with the support ot the internet community if you want to see the PJON project growing
with a faster pace, consider a donation at the following link: https://www.paypal.me/PJON
 ______________________________________________________________________________

Copyright 2012-2016 by Giovanni Blu Mitolo gioscarab@gmail.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#ifndef PJONMaster_h
  #define PJONMaster_h
  #include <PJON.h>

  /* Reference to device */
  struct Device_reference {
    uint8_t  packet_index = 0;
    uint32_t registration = 0;
    uint32_t rid          = 0;
    bool     state        = 0;
  };

  template<typename Strategy = SoftwareBitBang>
  class PJONMaster : public PJON<Strategy> {
    public:
      Device_reference ids[MAX_DEVICES];

      /* PJONMaster bus default initialization:
         State: Local (bus_id: 0.0.0.0)
         Acknowledge: true (Acknowledge is requested)
         device id: MASTER (254)
         Mode: HALF_DUPLEX
         Sender info: true (Sender info are included in the packet)
         Strategy: SoftwareBitBang */

      PJONMaster() : PJON<Strategy>(MASTER_ID) {
        PJON<Strategy>::set_error(static_error_handler);
        set_error(dummy_error_handler);
        set_receiver(dummy_receiver_handler);
        delete_id_reference();
      };

      /* PJONMaster initialization passing bus and device id:
         uint8_t my_bus = {1, 1, 1, 1};
         PJONMaster master(my_bys); */

      PJONMaster(const uint8_t *b_id) : PJON<Strategy>(b_id, MASTER_ID) {
        PJON<Strategy>::set_error(static_error_handler);
        set_error(dummy_error_handler);
        set_receiver(dummy_receiver_handler);
        delete_id_reference();
      };


      /* Add a device reference: */

      bool add_id(uint8_t id, uint32_t rid, bool state) {
        if(!ids[id - 1].state && !ids[id - 1].rid) {
          ids[id - 1].rid = rid;
          ids[id - 1].state = state;
          return true;
        }
        return false;
      };


      /* Confirm a device id sending a repeated broadcast containing:
         ID_REQUEST - RID (4 byte random id) - DEVICE ID (the new one assigned) */

      void approve_id(uint8_t id, uint8_t *b_id, uint32_t rid) {
        char response[6];
        uint16_t state = reserve_id(rid);
        if(state == DEVICES_BUFFER_FULL) return;
        if(state == FAIL) return negate_id(NOT_ASSIGNED, b_id, rid);

        response[0] = ID_REQUEST;
        response[1] = (uint32_t)(rid) >> 24;
        response[2] = (uint32_t)(rid) >> 16;
        response[3] = (uint32_t)(rid) >>  8;
        response[4] = (uint32_t)(rid);
        response[5] = state;

        ids[response[5] - 1].packet_index = PJON<Strategy>::send_repeatedly(
          BROADCAST,
          b_id,
          response,
          6,
          ID_REQUEST_INTERVAL,
          PJON<Strategy>::get_header() | ADDRESS_BIT
        );
      };


      /* Master begin function: */

      void begin() {
        PJON<Strategy>::begin();
        list_ids();
      };


      /* Confirm device ID insertion in list: */

      bool confirm_id(uint32_t rid, uint8_t id) {
        if(ids[id - 1].rid == rid && !ids[id - 1].state) {
          if(micros() - ids[id - 1].registration < ADDRESSING_TIMEOUT) {
            ids[id - 1].state = true;
            PJON<Strategy>::remove(ids[id - 1].packet_index);
            return true;
          }
        }
        return false;
      };


      /* Count active devices: */

      uint8_t count_active_ids() {
        uint8_t result = 0;
        for(uint8_t i = 0; i < MAX_DEVICES; i++)
          if(ids[i].state) result++;
        return result;
      };


      /* Empty a single element or the whole buffer: */

      void delete_id_reference(uint8_t id = 0) {
        if(!id) {
          for(uint8_t i = 0; i < MAX_DEVICES; i++) {
            ids[i].packet_index = 0;
            ids[i].registration = 0;
            ids[i].rid = 0;
            ids[i].state = false;
          }
        } else if(id > 0 && id < MAX_DEVICES) {
          ids[id - 1].packet_index = 0;
          ids[id - 1].registration = 0;
          ids[id - 1].rid   = 0;
          ids[id - 1].state = false;
        }
      };


      /* Master error handler: */

      void error_handler(uint8_t code, uint8_t data) {
        if(code == CONNECTION_LOST)
          delete_id_reference(data);
        _master_error(code, data);
      };

      static void static_error_handler(uint8_t code, uint8_t data) {
        PJONMaster<Strategy> *master = _current_pjon_master;
        if(master != NULL) master->error_handler(code, data);
      };


      /* Remove reserved id which expired (Remove never confirmed id requests): */

      void free_reserved_ids_expired() {
        for(uint8_t i = 0; i < MAX_DEVICES; i++)
          if(!ids[i].state && ids[i].rid)
            if((uint32_t)(micros() - ids[i].registration) < ADDRESSING_TIMEOUT)
              continue;
            else delete_id_reference(i + 1);
      };


      /* Check for device rid uniqueness in the reference buffer: */

      bool unique_rid(uint32_t rid) {
        for(uint8_t i = 0; i < MAX_DEVICES; i++)
          if(ids[i].rid == rid) return false;
        return true;
      };


      /* Broadcast a ID_LIST request to all devices: */

      void list_ids() {
        uint32_t time = micros();
        char request = ID_LIST;
        while(micros() - time < ADDRESSING_TIMEOUT) {
          PJON<Strategy>::send_packet_blocking(
            BROADCAST, this->bus_id, &request, 1, PJON<Strategy>::get_header() | ADDRESS_BIT
          );
          receive(LIST_IDS_RECEPTION_TIME);
        }
      };


      /* Negate a device id request sending a packet to the device containing ID_NEGATE
         forcing the slave to make a new request. */

      void negate_id(uint8_t id, uint8_t *b_id, uint32_t rid) {
        char response[5] = { ID_NEGATE, rid >> 24, rid >> 16, rid >> 8, rid};
        PJON<Strategy>::send(
          id,
          b_id,
          response,
          5,
          PJON<Strategy>::get_header() | ACK_REQUEST_BIT | ADDRESS_BIT
        );
      };


      /* Reserve a device id and wait for its confirmation: */

      uint16_t reserve_id(uint32_t rid) {
        if(!unique_rid(rid)) return FAIL;
        for(uint8_t i = 0; i < MAX_DEVICES; i++)
          if(!ids[i].state && !ids[i].rid) {
            ids[i].registration = micros();
            ids[i].rid = rid;
            ids[i].state = false;
            return i + 1;
          }
        _master_error(DEVICES_BUFFER_FULL, MAX_DEVICES);
        return DEVICES_BUFFER_FULL;
      };


  /* Master receive function: */

      uint16_t receive() {
        _current_pjon_master = this;
        uint16_t received_data = PJON<Strategy>::receive();
        if(received_data != ACK) return received_data;

        uint8_t overhead = PJON<Strategy>::packet_overhead(this->data[1]);
        uint8_t CRC_overhead = (this->data[1] & CRC_BIT) ? 4 : 1;

        if(this->last_packet_info.header & ADDRESS_BIT && this->data[2] > 4) {
          uint8_t request = this->data[overhead - CRC_overhead];
          uint32_t rid =
            (uint32_t)(this->data[(overhead - CRC_overhead) + 1] << 24) |
            (uint32_t)(this->data[(overhead - CRC_overhead) + 2] << 16) |
            (uint32_t)(this->data[(overhead - CRC_overhead) + 3] <<  8) |
            (uint32_t)(this->data[(overhead - CRC_overhead) + 4]);

          if(request == ID_REQUEST)
            approve_id(this->last_packet_info.sender_id, this->last_packet_info.sender_bus_id, rid);

          if(request == ID_CONFIRM)
            if(!confirm_id(rid, this->data[(overhead - CRC_overhead) + 5]))
              negate_id(this->last_packet_info.sender_id, this->last_packet_info.sender_bus_id, rid);

          if(request == ID_REFRESH)
            if(!add_id(this->data[(overhead - CRC_overhead) + 5], rid, 1))
              negate_id(this->last_packet_info.sender_id, this->last_packet_info.sender_bus_id, rid);

          if(request == ID_NEGATE)
            if(this->data[(overhead - CRC_overhead) + 5] == this->last_packet_info.sender_id)
              if(rid == ids[this->last_packet_info.sender_id - 1].rid)
                if(bus_id_equality(this->last_packet_info.sender_bus_id, this->bus_id))
                  delete_id_reference(this->last_packet_info.sender_id);

        }

        _master_receiver(this->data + (overhead - CRC_overhead), this->data[2] - overhead, this->last_packet_info);
        return ACK;
      };


      /* Try to receive a packet repeatedly with a maximum duration: */

      uint16_t receive(uint32_t duration) {
        uint32_t time = micros();
        while((uint32_t)(micros() - time) <= duration)
          if(receive() == ACK) return ACK;
        return FAIL;
      };


      /* Master receiver function setter: */

      void set_receiver(receiver r) {
        _master_receiver = r;
      };


      /* Master error receiver function: */

      void set_error(error e) {
        _master_error = e;
      };


      /* Master packet handling update: */

      uint8_t update() {
        free_reserved_ids_expired();
        _current_pjon_master = this;
        return PJON<Strategy>::update();
      };

    private:
      receiver _master_receiver;
      error _master_error;
      static PJONMaster<Strategy> *_current_pjon_master;
  };

  /* Shared callback function definition: */
  template<typename Strategy>
  PJONMaster<Strategy> * PJONMaster<Strategy>::_current_pjon_master = NULL;

#endif
