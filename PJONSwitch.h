
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

PJONSwitch has been contributed by Fred Larsen.

This class does the same as the PJONSimpleSwitch but supports routing packets
between buses of different strategies using the Any strategy.

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

#include <PJONSimpleSwitch.h>

class PJONAny : public PJONBus<Any> {
public:
  PJONAny(
    StrategyLinkBase *link,
    const uint8_t id = PJON_NOT_ASSIGNED,
    const uint32_t receive_time_in = 1000,
    const uint8_t num_device_id_segments = 1,
    const uint8_t device_id_segment = 0
  ) : PJONBus<Any>(id, receive_time_in, num_device_id_segments, device_id_segment) {
    strategy.set_link(link);
  };

  PJONAny(
    StrategyLinkBase *link,
    const uint8_t bus_id[],
    const uint8_t id = PJON_NOT_ASSIGNED,
    const uint32_t receive_time_in = 1000,
    const uint8_t num_device_id_segments = 1,
    const uint8_t device_id_segment = 0
  ) : PJONBus<Any>(bus_id, id, receive_time_in, num_device_id_segments, device_id_segment) {
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
