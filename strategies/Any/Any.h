#pragma once

#include <strategies/Any/StrategyLinkBase.h>
#include <strategies/Any/StrategyLink.h>

class Any {
public:
    StrategyLinkBase *s = NULL;

    /* Set a pointer to the StrategyLink to be used.
       It will not be freed by this class: */

    void set_link(StrategyLinkBase *strategy_link) { s = strategy_link; }


    /* Returns delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) { return s->back_off(attempts); }


    /* Begin method, to be called before transmission or reception: */

    bool begin(uint8_t additional_randomness = 0) {
      return s->begin(additional_randomness);
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
