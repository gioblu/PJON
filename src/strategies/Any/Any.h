
/* Any strategy
   This strategy includes virtual inheritance and let a PJON object switch
   from a strategy to another when required or a collection of PJON objects
   with different strategies to be treated dynamically.

   Proposed and developed by Fred Larsen
   ___________________________________________________________________________

    Copyright 2010-2019 Giovanni Blu Mitolo gioscarab@gmail.com

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

#include "StrategyLinkBase.h"
#include "StrategyLink.h"

class Any {
public:
    StrategyLinkBase *s = NULL;

    /* Set a pointer to the StrategyLink to be used.
       It will not be freed by this class: */

    void set_link(StrategyLinkBase *strategy_link) { s = strategy_link; }


    /* Returns delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) { return s->back_off(attempts); }


    /* Begin method, to be called on initialization: */

    bool begin(uint8_t did = 0) {
      return s->begin(did);
    }


    /* Check if the channel is free for transmission */

    bool can_start() { return s->can_start(); }


    /* Returns the maximum number of attempts for each transmission: */

    uint8_t get_max_attempts() { return s->get_max_attempts(); }


    /* Handle a collision: */

    void handle_collision() { s->handle_collision(); };


    /* Receive a string: */

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {
      return s->receive_string(string, max_length);
    }


    /* Receive byte response: */

    uint16_t receive_response() { return s->receive_response(); }


    /* Send byte response to package transmitter: */

    void send_response(uint8_t response) { s->send_response(response); }


    /* Send a string: */

    void send_string(uint8_t *string, uint16_t length) {
      s->send_string(string, length);
    }
};
