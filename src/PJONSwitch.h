
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

PJONSwitch has been contributed by Fred Larsen.

This class does the same as the PJONSimpleSwitch but supports routing packets
between buses of different strategies using the Any strategy.
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

#include "PJONSimpleSwitch.h"

class PJONAny : public PJONBus<Any> {
public:
  PJONAny(
    StrategyLinkBase *link,
    const uint8_t id = PJON_NOT_ASSIGNED,
    const uint32_t receive_time_in = 0,
    const uint8_t num_device_id_segments = 1,
    const uint8_t device_id_segment = 0
  ) : PJONBus<Any>(
    id,
    receive_time_in,
    num_device_id_segments,
    device_id_segment
  ) {
    strategy.set_link(link);
  };

  PJONAny(
    StrategyLinkBase *link,
    const uint8_t bus_id[],
    const uint8_t id = PJON_NOT_ASSIGNED,
    const uint32_t receive_time_in = 0,
    const uint8_t num_device_id_segments = 1,
    const uint8_t device_id_segment = 0
  ) : PJONBus<Any>(
    bus_id,
    id,
    receive_time_in,
    num_device_id_segments,
    device_id_segment
  ) {
    strategy.set_link(link);
  }
};

class PJONSwitch : public PJONSimpleSwitch<Any> {
public:
  PJONSwitch() : PJONSimpleSwitch<Any>() {};

  PJONSwitch(
    uint8_t bus_count,
    PJONAny *bus_list[],
    uint8_t default_gateway = PJON_NOT_ASSIGNED
  ) : PJONSimpleSwitch<Any>(bus_count, (PJONBus<Any>**)bus_list, default_gateway) { };
};
