
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| 13.0
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2021 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-__________________________________________________________________

PJONSwitch has been contributed by Fred Larsen.

This class does the same as the PJONSimpleSwitch but supports routing packets
between buses of different strategies using the Any strategy.
 _____________________________________________________________________________

This software is experimental and it is distributed "AS IS" without any
warranty, use it at your own risk.

Copyright 2010-2021 by Giovanni Blu Mitolo gioscarab@gmail.com

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

#include "PJONSimpleSwitch.h"
#include "strategies/Any/Any.h"

class PJONAny : public PJON<Any> {
public:
  PJONAny(
    const uint8_t id = PJON_NOT_ASSIGNED
  ) : PJON<Any>(id) {};

  PJONAny(
    StrategyLinkBase *link,
    const uint8_t id = PJON_NOT_ASSIGNED
  ) : PJON<Any>(id) {
    strategy.set_link(link);
  };

  PJONAny(
    StrategyLinkBase *link,
    const uint8_t bus_id[],
    const uint8_t id = PJON_NOT_ASSIGNED
  ) : PJON<Any>(
    bus_id,
    id
  ) {
    strategy.set_link(link);
  }

  void set_link(StrategyLinkBase *link) { strategy.set_link(link); }
};

class PJONSwitch : public PJONSimpleSwitch<Any> {
public:
  PJONSwitch() : PJONSimpleSwitch<Any>() {};

  PJONSwitch(
    uint8_t bus_count,
    PJONAny * const bus_list[],
    uint8_t default_gateway = PJON_NOT_ASSIGNED
  ) : PJONSimpleSwitch<Any>(bus_count, (PJON<Any>* const *)bus_list, default_gateway) { };
};

// Specialized class to simplify declaration when using 2 buses
template<class A, class B>
class PJONSwitch2 : public PJONSwitch {
  StrategyLink<A> linkA;
  StrategyLink<B> linkB;
  PJONAny busA, busB;
public:
  PJONSwitch2(uint8_t default_gateway = PJON_NOT_ASSIGNED) {
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
class PJONSwitch3 : public PJONSwitch {
  StrategyLink<A> linkA;
  StrategyLink<B> linkB;
  StrategyLink<C> linkC;
  PJONAny busA, busB, busC;
public:
  PJONSwitch3(uint8_t default_gateway = PJON_NOT_ASSIGNED) {
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