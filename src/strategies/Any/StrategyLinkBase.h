
/* StrategyLinkBase

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

class StrategyLinkBase {
public:

    /* Returns delay related to the attempts passed as parameter: */

    virtual uint32_t back_off(uint8_t attempts) = 0;


    /* Begin method, to be called before transmission or reception */

    virtual bool begin(uint8_t additional_randomness = 0) = 0;


    /* Check if the channel is free for transmission */

    virtual bool can_start() = 0;


    /* Returns the maximum number of attempts for each transmission: */

    virtual uint8_t get_max_attempts() = 0;


    /* Handle a collision: */

    virtual void handle_collision() = 0;


    /* Receive a string: */

    virtual uint16_t receive_string(uint8_t *string, uint16_t max_length) = 0;


    /* Receive byte response: */

    virtual uint16_t receive_response() = 0;


    /* Send byte response to package transmitter: */

    virtual void send_response(uint8_t response) = 0;


    /* Send a string: */

    virtual void send_string(uint8_t *string, uint16_t length) = 0;
};
