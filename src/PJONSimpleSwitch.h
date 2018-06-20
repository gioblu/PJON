
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| v11.0
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2018 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-__________________________________________________________________

PJONSimpleSwitch has been contributed by Fred Larsen.

It routes packets between buses with different bus ids, and between segmented
buses as well, even for local buses with no bus ids. It is limited to one
single strategy in contrast to its descendant PJONSwitch.

A default gateway can be specified, identifying one of the attached buses to
receive packets to other target buses than any of the attached buses. It is
possible to use a PJONSimpleSwitch to handle leaf buses in a tree structure.

A segmented bus is a "virtual" bus where ranges of its devices are  located
in separate physical buses.

The PJON project is entirely financed by contributions of people like you and
its resources are solely invested to cover the development and maintainance
costs, consider to make donation:
- Paypal:   https://www.paypal.me/PJON
- Bitcoin:  1FupxAyDTuAMGz33PtwnhwBm4ppc7VLwpD
- Ethereum: 0xf34AEAF3B149454522019781668F9a2d1762559b
Thank you and happy tinkering!
 _____________________________________________________________________________

This software is experimental and it is distributed "AS IS" without any
warranty, use it at your own risk.

Copyright 2010-2018 by Giovanni Blu Mitolo gioscarab@gmail.com

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
#include <PJON.h>

template<class Strategy>
class PJONBus : public PJON<Strategy> {
public:
  PJONBus(
    const uint8_t id = PJON_NOT_ASSIGNED,
    const uint32_t receive_time_in = 1000,
    const uint8_t num_device_id_segments = 1,
    const uint8_t device_id_segment = 0
  ) : PJON<Strategy>(id) {
    receive_time = receive_time_in;
    segment_count = num_device_id_segments;
    segment = device_id_segment;
  };

  PJONBus(
    const uint8_t bus_id[],
    const uint8_t id = PJON_NOT_ASSIGNED,
    const uint32_t receive_time_in = 1000,
    const uint8_t num_device_id_segments = 1,
    const uint8_t device_id_segment = 0
  ) : PJON<Strategy>(bus_id, id) {
    receive_time = receive_time_in;
    segment_count = num_device_id_segments;
    segment = device_id_segment;
  }

  uint32_t receive_time;
  uint8_t segment_count, segment;
};

#ifndef PJON_ROUTER_MAX_BUSES
  #define PJON_ROUTER_MAX_BUSES 5
#endif

template<class Strategy>
class PJONSimpleSwitch {
protected:
  uint8_t bus_count = 0;
  uint8_t default_gateway = PJON_NOT_ASSIGNED;
  uint8_t current_bus = PJON_NOT_ASSIGNED;
  PJONBus<Strategy> *buses[PJON_ROUTER_MAX_BUSES];

  void connect(
    uint8_t bus_count_in,
    PJONBus<Strategy> *buses_in[],
    uint8_t default_gateway_in,
    void *custom_pointer,
    PJON_Receiver receiver,
    PJON_Error error
  ) {
    bus_count = (bus_count_in > PJON_ROUTER_MAX_BUSES) ?
      PJON_ROUTER_MAX_BUSES : bus_count_in;
    default_gateway = default_gateway_in;
    for(uint8_t i = 0; i < bus_count; i++) {
      buses[i] = buses_in[i];
      buses[i]->set_receiver(receiver);
      buses[i]->set_error(error);
      buses[i]->set_custom_pointer(custom_pointer);
      buses[i]->set_router(true);
    }
  };

  uint8_t find_attached_bus_with_id(
    const uint8_t *bus_id,
    const uint8_t device_id,
    uint8_t &start_bus
  ) {
    for(uint8_t i=start_bus; i<bus_count; i++) {
      if(PJONBus<Strategy>::bus_id_equality(bus_id, buses[i]->bus_id)) {
        // Check if bus is segmented and if device belongs to bus's segment
        if(
          (buses[i]->segment_count <= 1) || // Not segmented
          (device_id == PJON_BROADCAST) ||  // Broadcast to all segments
          ((device_id / (256 / buses[i]->segment_count)) == buses[i]->segment)
        ) { // Segment match
          start_bus = i + 1; // Continue searching for more matches after this
          return i; // Explicit bus id match
        }
      }
    }
    start_bus = PJON_NOT_ASSIGNED;
    return PJON_NOT_ASSIGNED;
  };

  #ifdef PJON_ROUTER_NEED_INHERITANCE
  virtual
  #endif
  void send_packet(const uint8_t *payload, const uint16_t length, 
                   const uint8_t receiver_bus, const uint8_t sender_bus, 
                   bool &ack_sent, const PJON_Packet_Info &packet_info) {          
    // Send an ACK once to notify that the packet will be delivered
    if(
      !ack_sent &&
      (packet_info.header & PJON_ACK_REQ_BIT) &&
      (packet_info.receiver_id != PJON_BROADCAST)
    ) {
      buses[sender_bus]->strategy.send_response(PJON_ACK);
      ack_sent = true;
    }
    // Forward the packet
    buses[receiver_bus]->send_from_id(
      packet_info.sender_id,
      packet_info.sender_bus_id,
      packet_info.receiver_id,
      packet_info.receiver_bus_id,
      (const char*)payload,
      length,
      packet_info.header,
      packet_info.id,
      packet_info.port
    );
  }
  
  void forward_packet(const uint8_t *payload, const uint16_t length, 
                      const uint8_t receiver_bus, const uint8_t sender_bus, 
                      bool &ack_sent, const PJON_Packet_Info &packet_info) {
    // If receiving bus matches and not equal to sending bus, then route packet
    if(receiver_bus != PJON_NOT_ASSIGNED && receiver_bus != sender_bus) {
      send_packet(payload, length, receiver_bus, sender_bus, ack_sent, packet_info);
    }                        
  }  
  
  #ifdef PJON_ROUTER_NEED_INHERITANCE
  virtual
  #endif
  uint8_t find_bus_with_id(
    const uint8_t bus_id[],
    const uint8_t device_id,
    uint8_t &start_bus
  ) {
    return find_attached_bus_with_id(bus_id, device_id, start_bus);
  };

  #ifdef PJON_ROUTER_NEED_INHERITANCE
  virtual
  #endif
  void dynamic_receiver_function(
    uint8_t *payload,
    uint16_t length,
    const PJON_Packet_Info &packet_info
  ) {    
    uint8_t start_search = 0;
    bool ack_sent = false; // Send ACK only once even if delivering copies to multiple buses
    do {
      uint8_t receiver_bus = find_bus_with_id((const uint8_t*)
          ((packet_info.header & PJON_MODE_BIT) != 0 ?
          packet_info.receiver_bus_id : buses[0]->localhost),
          packet_info.receiver_id, start_search
      );

      if(receiver_bus == PJON_NOT_ASSIGNED) receiver_bus = default_gateway;
      
      forward_packet(payload, length, receiver_bus, current_bus, ack_sent, packet_info);

    } while(start_search != PJON_NOT_ASSIGNED);
  };

  #ifdef PJON_ROUTER_NEED_INHERITANCE
  virtual
  #endif
  void dynamic_error_function(uint8_t code, uint16_t data) { }

public:

  PJONSimpleSwitch() {};

  PJONSimpleSwitch(
    uint8_t bus_count,
    PJONBus<Strategy> *buses[],
    uint8_t default_gateway = PJON_NOT_ASSIGNED
  ) {
    connect_buses(bus_count, buses, default_gateway);
  };

  void begin() {
    for(uint8_t i = 0; i < bus_count; i++) buses[i]->begin();
  };

  void loop() {
    for(current_bus = 0; current_bus < bus_count; current_bus++) {
      uint16_t code =
        buses[current_bus]->receive(buses[current_bus]->receive_time);
      if(PJON_MAX_PACKETS < bus_count && code == PJON_ACK) break;
    }
    for(current_bus = 0; current_bus < bus_count; current_bus++)
      buses[current_bus]->update();
    current_bus = PJON_NOT_ASSIGNED;
  };

  void connect_buses(
    uint8_t bus_count_in,
    PJONBus<Strategy> *buses_in[],
    uint8_t default_gateway_in = PJON_NOT_ASSIGNED
  ) {
    connect(
      bus_count_in,
      buses_in,
      default_gateway_in,
      this,
      PJONSimpleSwitch<Strategy>::receiver_function,
      PJONSimpleSwitch<Strategy>::error_function
    );
  };
  
  uint8_t get_current_bus() const { return current_bus; }
  
  static void receiver_function(
    uint8_t *payload,
    uint16_t length,
    const PJON_Packet_Info &packet_info
  ) {
    (
      (PJONSimpleSwitch<Strategy>*)packet_info.custom_pointer
    )->dynamic_receiver_function(
        payload,
        length,
        packet_info
      );
  }

  static void error_function(uint8_t code, uint16_t data, void *custom_pointer) {
    ((PJONSimpleSwitch<Strategy>*)custom_pointer)->dynamic_error_function(code, data);
  }
};
