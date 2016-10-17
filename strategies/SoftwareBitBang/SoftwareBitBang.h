
/* SoftwareBitBang 1 or 2 wires interrupts-less digital communication data link layer
   used as a Strategy by the PJON framework (included in version v3.0)
   Compliant with the Padded jittering data link layer specification v0.1
   _____________________________________________________________________________

    Copyright 2012-2016 Giovanni Blu Mitolo gioscarab@gmail.com

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

STANDARD transmission mode performance:
   Transfer speed: 16.944kBb or 2.12kB/s
   Absolute  communication speed: 1.81kB/s (data length 20 of characters)
   Data throughput: 1.51kB/s (data length 20 of characters)
   Promiscuous architecture/clock compatible */
#define _SWBB_STANDARD  0

/* FAST transmission mode performance:
   Transfer speed: 25.157kBd or 3.15kB/s
   Absolute  communication speed: 2.55kB/s (data length 20 of characters)
   Data throughput: 2.13kB/s (data length 20 of characters)
   Promiscuous architecture/clock compatible */
#define _SWBB_FAST      1

/* OVERDRIVE transmission mode performance:
   Architecture / setup dependant, see Timing.h */
#define _SWBB_OVERDRIVE 2

/* Set here the selected transmission mode - default STANDARD */
#define _SWBB_MODE _SWBB_STANDARD

#include "Timing.h"
#include "../../utils/digitalWriteFast.h"

class SoftwareBitBang {
  public:

    /* Check if the channel is free for transmission:
    If receiving 10 bits no 1s are detected
    there is no active transmission */

    boolean can_start() {
      pinModeFast(_input_pin, INPUT);
      for(uint8_t i = 0; i < 9; i++) {
        if(digitalReadFast(_input_pin))
          return false;
        delayMicroseconds(SWBB_BIT_WIDTH);
      }
      if(digitalReadFast(_input_pin)) return false;
      return true;
    };


    /* Read a byte from the pin */

    uint8_t read_byte() {
      uint8_t byte_value = B00000000;
      /* Delay until the center of the first bit */
      delayMicroseconds(SWBB_BIT_WIDTH / 2);
      for(uint8_t i = 0; i < 7; i++) {
        /* Read in the center of the n one */
        byte_value += digitalReadFast(_input_pin) << i;
        /* Delay until the center of the next one */
        delayMicroseconds(SWBB_BIT_WIDTH);
      }
      /* Read in the center of the last one */
      byte_value += digitalReadFast(_input_pin) << 7;
      /* Delay until the end of the bit */
      delayMicroseconds(SWBB_BIT_WIDTH / 2);
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
      pullDownFast(_input_pin);

      if(_output_pin != _input_pin && _output_pin != NOT_ASSIGNED)
        pullDownFast(_output_pin);

      uint32_t time = micros();
      /* Do nothing until the pin goes LOW or passed more time than SWBB_BIT_SPACER duration */
      while(digitalReadFast(_input_pin) && (uint32_t)(micros() - time) <= SWBB_BIT_SPACER);
      /* Save how much time passed */
      time = micros() - time;
      /* is for sure equal or less than SWBB_BIT_SPACER, and if is more than ACCEPTANCE
         (a minimum HIGH duration) and what is coming after is a LOW bit
         probably a byte is coming so try to receive it. */
      if(time >= SWBB_ACCEPTANCE && !syncronization_bit())
        return (uint8_t)read_byte();
      return FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      if(_output_pin != _input_pin && _output_pin != NOT_ASSIGNED)
        digitalWriteFast(_output_pin, LOW);

      uint16_t response = FAIL;
      uint32_t time = micros();
      while(response == FAIL && (uint32_t)((micros() - SWBB_BIT_SPACER) - SWBB_BIT_WIDTH) <= time)
        response = receive_byte();
      return response;
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
      digitalWriteFast(_output_pin, HIGH);
      delayMicroseconds(SWBB_BIT_SPACER);
      digitalWriteFast(_output_pin, LOW);
      delayMicroseconds(SWBB_BIT_WIDTH);
      for(uint8_t mask = 0x01; mask; mask <<= 1) {
        digitalWriteFast(_output_pin, b & mask);
        delayMicroseconds(SWBB_BIT_WIDTH);
      }
    };


    /* Send byte response to package transmitter */

    void send_response(uint8_t response) {
      pinModeFast(_output_pin, OUTPUT);
      send_byte(response);
      pullDownFast(_output_pin);
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint8_t length) {
      pinModeFast(_output_pin, OUTPUT);
      for(uint8_t b = 0; b < length; b++)
        send_byte(string[b]);
      pullDownFast(_output_pin);
    };


    /* Syncronize with transmitter:
     This function is used only in byte syncronization.
     READ_DELAY has to be tuned to correctly send and
     receive transmissions because this variable shifts
     in which portion of the bit, the reading will be
     executed by the next read_byte function */

    uint8_t syncronization_bit() {
      delayMicroseconds((SWBB_BIT_WIDTH / 2) - SWBB_READ_DELAY);
      uint8_t bit_value = digitalReadFast(_input_pin);
      delayMicroseconds(SWBB_BIT_WIDTH / 2);
      return bit_value;
    };


    /* Set the communicaton pin: */

    void set_pin(uint8_t pin) {
      _input_pin = pin;
      _output_pin = pin;
    };


    /* Set a pair of communication pins: */

    void set_pins(uint8_t input_pin = NOT_ASSIGNED, uint8_t output_pin = NOT_ASSIGNED) {
      _input_pin = input_pin;
      _output_pin = output_pin;
    };

  private:
    uint8_t _input_pin;
    uint8_t _output_pin;
};
