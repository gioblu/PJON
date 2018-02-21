
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

PJONRouterExtended has been contributed by Fred Larsen.

It performs the same routing as the PJONRouter for locally attached buses,
but supports a static routing table to enable traversing multiple levels of 
buses.

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
#include <PJONRouter.h>

#ifndef PJON_ROUTER_TABLE_SIZE
  #define PJON_ROUTER_TABLE_SIZE 10
#endif

class PJONRouterExtended : public PJONRouter {
protected:
  uint8_t remote_bus_ids[PJON_ROUTER_MAX_BUSES][4];
  uint8_t remote_bus_via_attached_bus[PJON_ROUTER_MAX_BUSES];
  uint8_t table_size = 0;

  uint8_t find_bus_in_table(const uint8_t *bus_id, const uint8_t device_id, uint8_t &start_bus) {
    uint8_t start = start_bus - bus_count;
    for (uint8_t i=start; i<bus_count; i++) {
      if (PJONAny::bus_id_equality(bus_id, remote_bus_ids[i])) {
        start_bus = bus_count + i + 1; // Continue searching for more matches after this
        return i; // Explicit bus id match
      }
    }
    start_bus = PJON_NOT_ASSIGNED;
    return PJON_NOT_ASSIGNED;
  }

  virtual uint8_t find_bus_with_id(const uint8_t *bus_id, const uint8_t device_id, uint8_t &start_bus) {
    // Search for a locally attached bus first
    uint8_t receiver_bus = PJON_NOT_ASSIGNED;
    if (start_bus < bus_count) {
      receiver_bus = find_attached_bus_with_id(bus_id, device_id, start_bus);
      if (receiver_bus == PJON_NOT_ASSIGNED) start_bus = bus_count; // Not found among attached
    }

    // Then search in the routing table
    if (receiver_bus == PJON_NOT_ASSIGNED && start_bus >= bus_count && start_bus != PJON_NOT_ASSIGNED) {
      receiver_bus = find_bus_in_table(bus_id, device_id, start_bus);
    }
    if (receiver_bus == PJON_NOT_ASSIGNED) start_bus = PJON_NOT_ASSIGNED;
    return receiver_bus;
  }                   

public:  
  PJONRouterExtended() {}
  PJONRouterExtended(uint8_t bus_count,
                     PJONAny *buses[],
                     uint8_t default_gateway = PJON_NOT_ASSIGNED) 
                     : PJONRouter(bus_count, buses, default_gateway) { }
                     
  void add(const uint8_t bus_id[], uint8_t via_attached_bus) {
    if (table_size < PJON_ROUTER_TABLE_SIZE) {
      PJONAny::copy_bus_id(remote_bus_ids[table_size], bus_id);
      remote_bus_via_attached_bus[table_size] = via_attached_bus;
      table_size++;
    }
  }
};
