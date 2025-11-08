
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| 13.1
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2025 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-__________________________________________________________________

PJONInteractiveRouter has been contributed by Fred Larsen.

This class adds functionality to the PJONSwitch, PJONRouter, PJONDynamicRouter
and potential future classes derived from them. This functionality allows a
switch or router to have it's own device id and send and receive packets as a
normal device, but to and from multiple buses.

It also allows the device to listen to all packets passing through between
buses.

Probably it is wise to use this functionality only on routers using
strategies that are not timing-critical, for example on buffered media like
serial or Ethernet. If used on timing-critical strategies like SWBB, the
receiver callback should be really fast.
 _____________________________________________________________________________

This software is experimental and it is distributed "AS IS" without any
warranty, use it at your own risk.

Copyright 2010-2025 by Giovanni Blu Mitolo gioscarab@gmail.com

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
#include <PJONDynamicRouter.h>
#include <PJONVirtualBusRouter.h>

typedef void (* PJON_Send_Notification)(
  const uint8_t * const payload,
  const uint16_t length,
  const uint8_t receiver_bus,
  const uint8_t sender_bus,
  const PJON_Packet_Info &packet_info
);

template<class RouterClass = PJONSwitch>
class PJONInteractiveRouter : public RouterClass {
protected:
  void *custom_pointer = NULL;
  PJON_Receiver receiver = NULL;
  PJON_Error error = NULL;
  PJON_Send_Notification send_notification = NULL;
  bool router = false;

  virtual void dynamic_receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
    // Handle packets to this device, with user-supplied callback and custom ptr
    // (If this device has a device id on the source bus, and it is equal to
    // the packets receiver_id, the packet is for this device.)
    bool packet_is_for_me = (
      RouterClass::buses[RouterClass::current_bus]->tx.id != PJON_NOT_ASSIGNED &&
      memcmp(RouterClass::buses[RouterClass::current_bus]->tx.bus_id, packet_info.rx.bus_id, 4) == 0 &&
      RouterClass::buses[RouterClass::current_bus]->tx.id == packet_info.rx.id
    );

    // Take care of other's packets
    if(!packet_is_for_me)
      RouterClass::dynamic_receiver_function(payload, length, packet_info);
    else if(packet_info.header & PJON_ACK_REQ_BIT)
      RouterClass::buses[RouterClass::current_bus]->send_acknowledge();
    // Call the receive callback _after_ the packet has been delivered
    if(router || packet_is_for_me) {
      // The packet is for ME :-)
      PJON_Packet_Info p_i;
      memcpy(&p_i, &packet_info, sizeof(PJON_Packet_Info));
      p_i.custom_pointer = custom_pointer;
      if(receiver) receiver(payload, length, p_i);
    }
  }

  virtual void dynamic_error_function(uint8_t code, uint16_t data) {
    RouterClass::dynamic_error_function(code, data);

    // Call any user registered error function
    if(error) error(code, data, custom_pointer);
  }

  virtual void send_packet(const uint8_t *payload, const uint16_t length,
                           const uint8_t receiver_bus, const uint8_t sender_bus,
                           bool &ack_sent, const PJON_Packet_Info &packet_info) {
    RouterClass::send_packet(payload, length, receiver_bus, sender_bus, ack_sent, packet_info);

    // Call any user registered send notification function
    if (send_notification) send_notification(payload, length, receiver_bus, sender_bus, packet_info);
  }

public:
  PJONInteractiveRouter() : RouterClass() {}
  PJONInteractiveRouter(
    uint8_t bus_count,
    PJONAny* const buses[],
    uint8_t default_gateway = PJON_NOT_ASSIGNED)
    : RouterClass(bus_count, buses, default_gateway) {}

  void set_receiver(PJON_Receiver r) { receiver = r; };

  void set_error(PJON_Error e) { error = e; };

  void set_send_notification(PJON_Send_Notification s) { send_notification = s; };

  void set_custom_ptr(void *custom_ptr) { custom_pointer = custom_ptr; };

  void send_packet(
    const uint8_t *payload,
    uint16_t length,
    const PJON_Packet_Info &packet_info
  ) {
    dynamic_receiver_function(payload, length, packet_info);
  };

  // Deliver every packet to receiver callback, or just for this device?
  void set_router(bool on) { router = on; };
};

// Specialized class to simplify declaration when using 2 buses
template<class A, class B, class RouterClass = PJONSwitch>
class PJONInteractiveRouter2 : public PJONInteractiveRouter<RouterClass> {
  StrategyLink<A> linkA;
  StrategyLink<B> linkB;
  PJONAny busA, busB;
public:
  PJONInteractiveRouter2(uint8_t default_gateway = PJON_NOT_ASSIGNED) {
    PJON<Any>* buses[2] = { &busA, &busB };
    PJONSimpleSwitch<Any>::connect_buses(2, buses, default_gateway);
    busA.set_link(&linkA);
    busB.set_link(&linkB);
  };

  PJONAny &get_bus(const uint8_t ix) { return ix == 0 ? busA : busB; }

  A &get_strategy_0() { return linkA.strategy; }
  B &get_strategy_1() { return linkB.strategy; }
};

// Specialized class to simplify declaration when using 3 buses
template<class A, class B, class C, class RouterClass = PJONSwitch>
class PJONInteractiveRouter3 : public PJONInteractiveRouter<RouterClass> {
  StrategyLink<A> linkA;
  StrategyLink<B> linkB;
  StrategyLink<C> linkC;
  PJONAny busA, busB, busC;
public:
  PJONInteractiveRouter3(uint8_t default_gateway = PJON_NOT_ASSIGNED) {
    PJON<Any>* buses[3] = { &busA, &busB, &busC };
    PJONSimpleSwitch<Any>::connect_buses(3, buses, default_gateway);
    busA.set_link(&linkA);
    busB.set_link(&linkB);
    busC.set_link(&linkC);
  };

  PJONAny &get_bus(const uint8_t ix) { return ix == 0 ? busA : (ix == 1 ? busB : busC); }

  A &get_strategy_0() { return linkA.strategy; }
  B &get_strategy_1() { return linkB.strategy; }
  C &get_strategy_2() { return linkC.strategy; }
};
