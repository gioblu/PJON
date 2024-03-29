
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| 13.1
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2022 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-__________________________________________________________________

PJONVirtualBusRouter has been contributed by Fred Larsen.

This class adds functionality to the PJONSwitch, PJONRouter, PJONDynamicRouter
and potential future classes derived from them. This functionality allows a
switch or router to treat multiple attached buses with the same bus id as a
"virtual" bus, where devices can be placed anywhere independent of device id.

It will start in promiscuous mode, delivering every packet to all attached
buses except the one where the packet comes from. As it learns by looking at
the sender ids of observed packets, it will deliver each packet only to the
attached bus where the receiver device can be found, increasing precision
and reducing traffic.
 _____________________________________________________________________________

This software is experimental and it is distributed "AS IS" without any
warranty, use it at your own risk.

Copyright 2010-2022 by Giovanni Blu Mitolo gioscarab@gmail.com

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
#include "PJONDynamicRouter.h"

// If none of your devices have a device id above a specific number,
// you can save some bytes of RAM by lowering this constant.
// The constant must be higher than the highest device id that
// can be encountered, and will limit the router to this range.
#ifndef PJON_VIRTUALBUS_MAX_DEVICES
  #define PJON_VIRTUALBUS_MAX_DEVICES 255
#endif

// After having observed a device on one bus it is associated with that bus
// and packets will not be forwarded to other buses.
// This setting can activate a timeout so that if there is no traffic from a
// device for a certain time, its position is cleared and packets will be
// forwared to all buses. This allows a device to be moved from behind one
// router to behind another without restarting routers. The value is in seconds.
#ifndef PJON_VIRTUALBUS_ROUTE_TIMEOUT_S
  #define PJON_VIRTUALBUS_ROUTE_TIMEOUT_S 0
#endif

template<class RouterClass = PJONSwitch>
class PJONVirtualBusRouter : public RouterClass {
protected:
  // The array position of one (any) of the parts of the virtual bus.
  uint8_t virtual_bus = PJON_NOT_ASSIGNED;
  uint8_t device_via_attached_bus[PJON_VIRTUALBUS_MAX_DEVICES];
#if (PJON_VIRTUALBUS_ROUTE_TIMEOUT != 0)
  uint32_t device_last_activity_time[PJON_VIRTUALBUS_MAX_DEVICES];
#endif

  // Support for disabling ACK for devices with unknown location
  bool unknown_device_location = false;

  void init_vbus() {
    for (uint8_t i=0; i<PJON_VIRTUALBUS_MAX_DEVICES; i++)
      device_via_attached_bus[i] = PJON_NOT_ASSIGNED;  
  }

  uint8_t find_vbus_with_device(const uint8_t *bus_id, const uint8_t device_id) {
    if (!is_vbus(bus_id) || device_id >= PJON_VIRTUALBUS_MAX_DEVICES)
      return PJON_NOT_ASSIGNED;
#if (PJON_VIRTUALBUS_ROUTE_TIMEOUT != 0)
    // Forget the device location if no packet from it has been observed for some time
    if ((uint32_t)(PJON_MILLIS() - device_last_activity_time[device_id])
        > (uint32_t)PJON_VIRTUALBUS_ROUTE_TIMEOUT*1000)
      device_via_attached_bus[device_id] = PJON_NOT_ASSIGNED;
#endif
    return device_via_attached_bus[device_id];
  }

  bool is_vbus(const uint8_t bus_id[]) {
    return virtual_bus < RouterClass::bus_count &&
           memcmp(RouterClass::buses[virtual_bus]->tx.bus_id, bus_id, 4)==0;
  }

  void register_device_on_vbus(const uint8_t device_id, const uint8_t attached_bus) {
    if (device_id < PJON_VIRTUALBUS_MAX_DEVICES) {
      #ifdef DEBUG_PRINT
      if (attached_bus != device_via_attached_bus[device_id]) {
        Serial.print(F("Device ")); Serial.print(device_id);
        Serial.print(F(" on bus ")); Serial.println(attached_bus);
      }
      #endif
      device_via_attached_bus[device_id] = attached_bus;
#if (PJON_VIRTUALBUS_ROUTE_TIMEOUT != 0)
      device_last_activity_time[device_id] = PJON_MILLIS();
#endif
    }
  }

  virtual void send_packet(const uint8_t *payload, const uint16_t length,
                           const uint8_t receiver_bus, const uint8_t sender_bus,
                           bool &ack_sent, const PJON_Packet_Info &packet_info) {
    // Override the base class send_packet to disable requesting and sending ACK
    // if the receiver's location is not registered.
    bool disable_ack = unknown_device_location && is_vbus(RouterClass::buses[receiver_bus]->tx.bus_id);
    if (disable_ack) {
      PJON_Packet_Info info = packet_info;
      info.header &= ~PJON_ACK_REQ_BIT;

      bool disable_ack = true;
      RouterClass::send_packet(payload, length, receiver_bus, sender_bus, disable_ack, info);
      #ifdef DEBUG_PRINT_PACKETS
      Serial.print(F("FORWARD NOACK ")); Serial.print(info.receiver_id); Serial.print(F(" to bus "));
      Serial.println(receiver_bus);
      #endif
    }
    else {
      RouterClass::send_packet(payload, length, receiver_bus, sender_bus, ack_sent, packet_info);
      #ifdef DEBUG_PRINT_PACKETS
      Serial.print(F("FORWARD ")); Serial.print(packet_info.rx.id); Serial.print(F(" to bus "));
      Serial.println(receiver_bus);
      #endif
    }
  }

  void handle_send_error(uint8_t code, uint8_t packet) {
    // Find out which device id does not receive
    if (PJON_CONNECTION_LOST == code &&
        is_vbus(RouterClass::buses[RouterClass::current_bus]->tx.bus_id) &&
        (packet < PJON_MAX_PACKETS || PJON_MAX_PACKETS == 0))
    {
      PJON_Packet_Info info;
      #if PJON_MAX_PACKETS == 0
      RouterClass::buses[RouterClass::current_bus]->
        parse((RouterClass::buses[RouterClass::current_bus]->data), info);
      #else
      RouterClass::buses[RouterClass::current_bus]->
        parse((RouterClass::buses[RouterClass::current_bus]->packets[packet].content), info);
      #endif
      if (info.rx.id < PJON_VIRTUALBUS_MAX_DEVICES && is_vbus(info.rx.bus_id)) {
        // Unregister the device if we got an error trying to deliver to the attached
        // bus on which it is registered. This will step back from pointed delivery
        // to duplication on all parts of the virtual bus for this device.
        if (device_via_attached_bus[info.rx.id] == RouterClass::current_bus) {
          device_via_attached_bus[info.rx.id] = PJON_NOT_ASSIGNED;
          #ifdef DEBUG_PRINT
          Serial.print("Unregister "); Serial.print(info.rx.id);
          Serial.print(" from bus "); Serial.println(RouterClass::current_bus);
          #endif
        }
      }
    }
  }

  virtual void dynamic_receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
    // First register the sender device if it belongs to  a virtual bus.
    // If a packet is sent to this device later, it is possible to fall back
    // from delivering a copy to each part of the virtual bus, to just
    // delivering to the part where the device is actually present.
    if (is_vbus(packet_info.tx.bus_id))
      register_device_on_vbus(packet_info.tx.id, RouterClass::current_bus);

    // Search for the device on the virtual bus
    uint8_t receiver_bus = find_vbus_with_device(packet_info.rx.bus_id, packet_info.rx.id);

    // If found on part of a virtual bus, do not deliver copies to others
    if (receiver_bus != PJON_NOT_ASSIGNED) {
      bool ack_sent = false;
      if (receiver_bus != RouterClass::current_bus)
        RouterClass::forward_packet(payload, length, receiver_bus, RouterClass::current_bus, ack_sent, packet_info);
    } else {
      unknown_device_location = true;
      RouterClass::dynamic_receiver_function(payload, length, packet_info);
      unknown_device_location = false;
    }
  }

  virtual void dynamic_error_function(uint8_t code, uint16_t data) {
    handle_send_error(code, data);
  }

public:
  PJONVirtualBusRouter() : RouterClass() { init_vbus(); }
  PJONVirtualBusRouter(
    uint8_t bus_count,
    PJONAny* const *buses,
    uint8_t default_gateway = PJON_NOT_ASSIGNED)
    : RouterClass(bus_count, buses, default_gateway) { init_vbus(); }

  /* Support multiple of the attached physical buses to have the same bus id,
  / forming a "virtual bus" where devices can be in any part independent of
  / device id. Specify the array position of one of the bus parts. */
  void set_virtual_bus(uint8_t first_bus) {
    virtual_bus = first_bus;
  }
};

// Specialized class to simplify declaration when using 2 buses
template<class A, class B, class RouterClass = PJONSwitch>
class PJONVirtualBusRouter2 : public PJONVirtualBusRouter<RouterClass> {
  StrategyLink<A> linkA;
  StrategyLink<B> linkB;
  PJONAny busA, busB;
public:
  PJONVirtualBusRouter2(uint8_t default_gateway = PJON_NOT_ASSIGNED) {
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
class PJONVirtualBusRouter3 : public PJONVirtualBusRouter<RouterClass> {
  StrategyLink<A> linkA;
  StrategyLink<B> linkB;
  StrategyLink<C> linkC;
  PJONAny busA, busB, busC;
public:
  PJONVirtualBusRouter3(uint8_t default_gateway = PJON_NOT_ASSIGNED) {
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