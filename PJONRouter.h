
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| v10.1
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

PJONNetworkRouter has been contributed by Fred Larsen.

It routes packets between buses with different bus ids.

For installations not using bus ids but simply segmenting one bus over multiple
media to keep the packet size minimized, please have a look at the
PJONLocalRouter class instead.

If you believe in this project and you appreciate our work, please, make a
donation. The PJON Foundation is entirely financed by contributions of wise
people like you and its resources are solely invested to cover the development
and maintainance costs.
- Paypal:   https://www.paypal.me/PJON
- Bitcoin:  1FupxAyDTuAMGz33PtwnhwBm4ppc7VLwpD
- Ethereum: 0xf34AEAF3B149454522019781668F9a2d1762559b
Thank you and happy tinkering!
 _____________________________________________________________________________

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

class PJONAny : public PJON<Any> {
public:
  PJONAny(const uint8_t          id,
                StrategyLinkBase *link,
          const uint32_t         receive_time_in = 1000,
          const uint8_t          num_device_id_segments = 1,
          const uint8_t          device_id_segment = 0) : PJON<Any>(id) {
    strategy.set_link(link);
    receive_time = receive_time_in;
    segment_count = num_device_id_segments;
    segment = device_id_segment;
  }

  PJONAny(const uint8_t          bus_id[],
          const uint8_t          id,
                StrategyLinkBase *link,
          const uint32_t         receive_time_in = 1000,
          const uint8_t          num_device_id_segments = 1,
          const uint8_t          device_id_segment = 0) : PJON<Any>(bus_id, id) {
    strategy.set_link(link);
    receive_time = receive_time_in;
    segment_count = num_device_id_segments;
    segment = device_id_segment;
  }
  
  uint32_t receive_time;
  uint8_t segment_count, segment;
};

#ifndef PJON_ROUTER_MAX_BUSES
  #define PJON_ROUTER_MAX_BUSES 10
#endif

class PJONRouter {
  uint8_t bus_count = 0,
          default_gateway = PJON_NOT_ASSIGNED,
          current_bus = PJON_NOT_ASSIGNED;
  PJONAny *buses[PJON_ROUTER_MAX_BUSES];
public:

  PJONRouter() {}
  PJONRouter(uint8_t bus_count,
             PJONAny *buses[],
             uint8_t default_gateway = PJON_NOT_ASSIGNED)
  {
    connect_buses(bus_count, buses, default_gateway);
  }

  void begin() {
    for (uint8_t i=0; i<bus_count; i++) buses[i]->begin();
  }

  void loop() {
    for (uint8_t i=0; i<bus_count; i++) {
      current_bus = i;
      uint16_t code = buses[i]->receive(buses[i]->receive_time);
      if (PJON_MAX_PACKETS < bus_count && code == PJON_ACK) break;
    }
    current_bus = PJON_NOT_ASSIGNED;
    for (uint8_t i=0; i<bus_count; i++) buses[i]->update();
  }

  void connect_buses(uint8_t bus_count_in,
                     PJONAny *buses_in[],
                     uint8_t default_gateway_in = PJON_NOT_ASSIGNED)
  {
    bus_count = bus_count_in > PJON_ROUTER_MAX_BUSES ? PJON_ROUTER_MAX_BUSES : bus_count_in;
    default_gateway = default_gateway_in;
    for (uint8_t i=0; i<bus_count; i++) {
      buses[i] = buses_in[i];
      buses[i]->set_receiver(PJONRouter::receiver_function);
      buses[i]->set_custom_pointer(this);
      buses[i]->set_router(true);
    }
  }

  uint8_t find_bus_with_id(const uint8_t *bus_id, const uint8_t device_id, const uint8_t start_bus) {
    for (uint8_t i=start_bus; i<bus_count; i++) {
      if (PJONAny::bus_id_equality(bus_id, buses[i]->bus_id)) {
        // Check if the bus is segmented and if the device belongs to the bus's segment
        if (buses[i]->segment_count <= 1 // No segments
          || device_id == PJON_BROADCAST // Broadcast to all segments of same bus id
          || ((device_id/(256/buses[i]->segment_count)) == buses[i]->segment))
          return i; // Explicit bus id match
      }
    }
    return PJON_NOT_ASSIGNED;
  }

  void dynamic_receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {  
    uint8_t sender_bus = current_bus, start_search = 0;
    bool ack_sent = false; // Send ACK only once even if delivering copies to multiple buses
    do {
      uint8_t receiver_bus = find_bus_with_id((packet_info.header & PJON_MODE_BIT) != 0 ? 
                                              packet_info.receiver_bus_id : (const uint8_t[]){0,0,0,0}, 
                                              packet_info.receiver_id, start_search);
      start_search = receiver_bus == PJON_NOT_ASSIGNED ? PJON_NOT_ASSIGNED : receiver_bus + 1;
      if (receiver_bus == PJON_NOT_ASSIGNED) receiver_bus = default_gateway;

      // If we have a matching receiving bus, and it is not the same as the sending bus, route the packet
      if (receiver_bus != PJON_NOT_ASSIGNED && receiver_bus != sender_bus) {

        // Send an ACK once to notify that we will take care of delivering the packet
        if (!ack_sent && packet_info.header & PJON_ACK_REQ_BIT && packet_info.receiver_id != PJON_BROADCAST) {
          buses[sender_bus]->strategy.send_response(PJON_ACK);
          ack_sent = true;
        }

        // Forward the packet
        if (PJON_MAX_PACKETS > 0)
          buses[receiver_bus]->send_from_id(packet_info.sender_id, packet_info.sender_bus_id,
           packet_info.receiver_id, packet_info.receiver_bus_id, (const char*)payload, length,
           packet_info.header, packet_info.id, packet_info.port);
        else
          buses[receiver_bus]->send_packet_blocking(packet_info.receiver_id, packet_info.receiver_bus_id,
           (const char*)payload, length, packet_info.header, packet_info.port);
// TODO: Need send_from_id_blocking functions!!!
      }
    } while (start_search != PJON_NOT_ASSIGNED);
  }

  static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
    ((PJONRouter*) packet_info.custom_pointer)->dynamic_receiver_function(payload, length, packet_info);
  }
};
