
/* StrategyLink

   Proposed and developed by Fred Larsen
   ___________________________________________________________________________

    Copyright 2010-2021 Giovanni Blu Mitolo gioscarab@gmail.com

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

template<typename Strategy>
class StrategyLink : public StrategyLinkBase {
public:
    Strategy strategy;

    /* Returns delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      return strategy.back_off(attempts);
    }


    /* Begin method, to be called on initialization: */

    bool begin(uint8_t did = 0) {
      return strategy.begin(did);
    }


    /* Check if the channel is free for transmission: */

    bool can_start() { return strategy.can_start(); }


    /* Returns the maximum number of attempts for each transmission: */

    uint8_t get_max_attempts() { return strategy.get_max_attempts(); }


    /* Returns the recommended receive time for this strategy: */

    uint16_t get_receive_time() { return strategy.get_receive_time(); }


    /* Handle a collision: */

    void handle_collision() { strategy.handle_collision(); };


    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      return strategy.receive_frame(data, max_length);
    }


    /* Receive byte response: */

    uint16_t receive_response() { return strategy.receive_response(); }


    /* Send byte response to package transmitter: */

    void send_response(uint8_t response) { strategy.send_response(response); }


    /* Send a frame: */

    void send_frame(uint8_t *data, uint16_t length) {
      strategy.send_frame(data, length);
    }
};
