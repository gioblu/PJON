
/* SoftwareBitBang
   1 or 2 wires software emulated digital communication data link layer
   used as a Strategy by the PJON framework (included in version v3.0)
   Compliant with the Padded jittering data link layer specification v0.1
   _____________________________________________________________________________

    Copyright 2010-2017 Giovanni Blu Mitolo gioscarab@gmail.com

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

/* STANDARD transmission mode performance:
   Transfer speed: 16.944kBb or 2.12kB/s
   Absolute  communication speed: 1.81kB/s (data length 20 of characters)
   Data throughput: 1.51kB/s (data length 20 of characters)
   Promiscuous architecture/clock compatible */
#define SWBB_STANDARD  1

/* FAST transmission mode performance:
   Transfer speed: 25.157kBd or 3.15kB/s
   Absolute  communication speed: 2.55kB/s (data length 20 of characters)
   Data throughput: 2.13kB/s (data length 20 of characters)
   Promiscuous architecture/clock compatible */
#define SWBB_FAST      2

/* OVERDRIVE transmission mode performance:
   Architecture / setup dependant, see Timing.h */
#define SWBB_OVERDRIVE 3

/* Set here the selected transmission mode - default STANDARD */
#ifndef SWBB_MODE
  #define SWBB_MODE SWBB_STANDARD
#endif

#include "Timing.h"

class SoftwareBitBang {
  public:
    /* Returns the suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < SWBB_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };


    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t additional_randomness = 0) {
      delay(PJON_RANDOM(SWBB_INITIAL_DELAY) + additional_randomness);
      PJON_IO_PULL_DOWN(_input_pin);
      if(_output_pin != _input_pin)
        PJON_IO_PULL_DOWN(_output_pin);
      return true;
    };


    /* Check if the channel is free for transmission:
       If receiving 10 bits no 1s are detected there is no active transmission */

    bool can_start() {
      PJON_IO_MODE(_input_pin, INPUT);
      PJON_DELAY_MICROSECONDS(SWBB_BIT_SPACER / 2);
      if(PJON_IO_READ(_input_pin)) return false;
      PJON_DELAY_MICROSECONDS((SWBB_BIT_SPACER / 2));
      if(PJON_IO_READ(_input_pin)) return false;
      PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH / 2);
      for(uint8_t i = 0; i < 9; i++) {
        if(PJON_IO_READ(_input_pin))
          return false;
        PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH);
      }
      if(PJON_IO_READ(_input_pin)) return false;
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(SWBB_COLLISION_DELAY));
      if(PJON_IO_READ(_input_pin)) return false;
      return true;
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return SWBB_MAX_ATTEMPTS;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(SWBB_COLLISION_DELAY));
    };


    /* Read a byte from the pin */

    uint8_t read_byte() {
      uint8_t byte_value = 0B00000000;
      /* Delay until the center of the first bit */
      PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH / 2);
      for(uint8_t i = 0; i < 7; i++) {
        /* Read in the center of the n one */
        byte_value += PJON_IO_READ(_input_pin) << i;
        /* Delay until the center of the next one */
        PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH);
      }
      /* Read in the center of the last one */
      byte_value += PJON_IO_READ(_input_pin) << 7;
      /* Delay until the end of the bit */
      PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH / 2);
      return byte_value;
    };


    /* Check if a byte is coming from the pin:
     This function is looking for padding bits before a byte.
     If value is 1 for more than ACCEPTANCE and after
     that comes a 0 probably a byte is coming:
      ________
     |  Init  |
     |--------|
     |_____   |
     |  |  |  |
     |1 |  |0 |
     |__|__|__|
        |
      ACCEPTANCE */

    uint16_t receive_byte() {
      /* Initialize the pin and set it to LOW to reduce interference */
      PJON_IO_PULL_DOWN(_input_pin);
      if(_output_pin != _input_pin && _output_pin != PJON_NOT_ASSIGNED)
        PJON_IO_PULL_DOWN(_output_pin);

      uint32_t time = micros();
      /* Do nothing until the pin goes LOW or passed more time than
         SWBB_BIT_SPACER duration */
      while(
        PJON_IO_READ(_input_pin) &&
        (uint32_t)(micros() - time) <= SWBB_BIT_SPACER
      );
      /* Save how much time passed */
      time = micros() - time;
      /* is for sure equal or less than SWBB_BIT_SPACER, and if is more
         than ACCEPTANCE (a minimum HIGH duration) and what is coming after is
         a LOW bit probably a byte is coming so try to receive it. */
      if(time >= SWBB_ACCEPTANCE) {
        PJON_DELAY_MICROSECONDS((SWBB_BIT_WIDTH / 2) - SWBB_READ_DELAY);
        if(!PJON_IO_READ(_input_pin)) {
          PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH / 2);
          return (uint8_t)read_byte();
        }
      }
      return PJON_FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      if(_output_pin != _input_pin && _output_pin != PJON_NOT_ASSIGNED)
        PJON_IO_WRITE(_output_pin, LOW);

      uint16_t response = PJON_FAIL;
      uint32_t time = micros();
      /* Transmitter emits a bit SWBB_BIT_WIDTH / 4 long and tries
         to get a response cyclically for SWBB_RESPONSE_TIMEOUT microseconds.
         Receiver synchronizes to the falling edge of the last incoming
         bit and transmits PJON_ACK or PJON_NAK */
      while(
        response == PJON_FAIL &&
        (uint32_t)(micros() - SWBB_RESPONSE_TIMEOUT) <= time
      ) {
        PJON_IO_WRITE(_input_pin, LOW);
        response = receive_byte();
        if(response == PJON_FAIL) {
          PJON_IO_MODE(_output_pin, OUTPUT);
          PJON_IO_WRITE(_output_pin, HIGH);
          PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH / 4);
          PJON_IO_PULL_DOWN(_output_pin);
        }
      }
      return response;
    };


    /* Receive a string: */

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {
      uint16_t result = receive_byte();
      if(result == PJON_FAIL) return PJON_FAIL;
      *string = result;
      return 1;
    };


    /* Every byte is prepended with 2 synchronization padding bits. The first
       is a longer than standard logic 1 followed by a standard logic 0.
       __________ ___________________________
      | SyncPad  | Byte                      |
      |______    |___       ___     _____    |
      | |    |   |   |     |   |   |     |   |
      | | 1  | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
      |_|____|___|___|_____|___|___|_____|___|
        |
       ACCEPTANCE

    The reception tecnique is based on finding a logic 1 as long as the
    first padding bit within a certain threshold, synchronizing to its
    falling edge and checking if it is followed by a logic 0. If this
    pattern is recognised, reception starts, if not, interference,
    synchronization loss or simply absence of communication is
    detected at byte level. */

    void send_byte(uint8_t b) {
      PJON_IO_WRITE(_output_pin, HIGH);
      PJON_DELAY_MICROSECONDS(SWBB_BIT_SPACER);
      PJON_IO_WRITE(_output_pin, LOW);
      PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH);
      for(uint8_t mask = 0x01; mask; mask <<= 1) {
        PJON_IO_WRITE(_output_pin, b & mask);
        PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH);
      }
    };


    /* Send byte response to package transmitter */

    void send_response(uint8_t response) {
      PJON_IO_PULL_DOWN(_input_pin);
      uint32_t time = micros();
      /* Transmitter emits a bit SWBB_BIT_WIDTH / 4 long and tries
         to get a response cyclically for SWBB_RESPONSE_TIMEOUT microseconds.
         Here Receiver synchronizes to the falling edge of the last incoming
         bit and transmits response variable */

      // If initially low Wait for the next high
      while(
        (uint32_t)(micros() - time) < (SWBB_BIT_WIDTH) &&
        !PJON_IO_READ(_input_pin)
      );
      time = micros();
      // If high Wait for low
      while(
        (uint32_t)(micros() - time) < (SWBB_BIT_WIDTH / 2.25) &&
        PJON_IO_READ(_input_pin)
      );
      PJON_IO_MODE(_output_pin, OUTPUT);
      send_byte(response);
      PJON_IO_PULL_DOWN(_output_pin);
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint16_t length) {
      PJON_IO_MODE(_output_pin, OUTPUT);
      for(uint16_t b = 0; b < length; b++)
        send_byte(string[b]);
      PJON_IO_PULL_DOWN(_output_pin);
    };


    /* Set the communicaton pin: */

    void set_pin(uint8_t pin) {
      _input_pin = pin;
      _output_pin = pin;
    };


    /* Set a pair of communication pins: */

    void set_pins(
      uint8_t input_pin = PJON_NOT_ASSIGNED,
      uint8_t output_pin = PJON_NOT_ASSIGNED
    ) {
      _input_pin = input_pin;
      _output_pin = output_pin;
    };

  private:
    uint8_t _input_pin;
    uint8_t _output_pin;
};
