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
