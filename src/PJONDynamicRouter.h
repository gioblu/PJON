
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

PJONDynamicRouter has been contributed by Fred Larsen.

It performs the same as PJONRouter, but populates the routing table
dynamically based on observed packets from remote buses.
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

#ifndef PJON_ROUTER_TABLE_SIZE
  #define PJON_ROUTER_TABLE_SIZE 100
#endif

#include "PJONRouter.h"

class PJONDynamicRouter : public PJONRouter {
protected:

  void add_sender_to_routing_table(
    const PJON_Packet_Info &packet_info,
    uint8_t sender_bus
  ) {
    uint8_t start_search = 0;
    uint8_t found_bus = find_bus_with_id(
      packet_info.tx.bus_id,
      packet_info.tx.id,
      start_search
    );
    // Not found among attached buses or in routing table. Add to table.
    if(found_bus == PJON_NOT_ASSIGNED)
      add(packet_info.tx.bus_id, sender_bus);
  };

  virtual void dynamic_receiver_function(
    uint8_t *payload,
    uint16_t length,
    const PJON_Packet_Info &packet_info
  ) {
    // Do standard routing but also add unknown remote buses to routing table
    add_sender_to_routing_table(packet_info, current_bus);
    PJONSwitch::dynamic_receiver_function(payload, length, packet_info);
  };

public:
  PJONDynamicRouter() { };

  PJONDynamicRouter(
    uint8_t bus_count,
    PJONAny * const buses[],
    uint8_t default_gateway = PJON_NOT_ASSIGNED
  ) : PJONRouter(bus_count, buses, default_gateway) { };
};

// Specialized class to simplify declaration when using 2 buses
template<class A, class B>
class PJONDynamicRouter2 : public PJONDynamicRouter {
  StrategyLink<A> linkA;
  StrategyLink<B> linkB;
  PJONAny busA, busB;
public:
  PJONDynamicRouter2(uint8_t default_gateway = PJON_NOT_ASSIGNED) {
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
template<class A, class B, class C>
class PJONDynamicRouter3 : public PJONDynamicRouter {
  StrategyLink<A> linkA;
  StrategyLink<B> linkB;
  StrategyLink<C> linkC;
  PJONAny busA, busB, busC;
public:
  PJONDynamicRouter3(uint8_t default_gateway = PJON_NOT_ASSIGNED) {
    PJON<Any> *buses[3] = { &busA, &busB, &busC };
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
