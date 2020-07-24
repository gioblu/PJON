
/* SoftwareBitBang
   1 or 2 wires software-defined asynchronous serial data link layer
   used as a Strategy by PJON (included in version v3.0)
   Compliant with PJDL (Padded Jittering Data Link) specification v5.0
   ___________________________________________________________________________

    Copyright 2010-2020 Giovanni Blu Mitolo gioscarab@gmail.com

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

/* Set here the selected transmission mode - default STANDARD */
#ifndef SWBB_MODE
  #define SWBB_MODE         1
#endif

// Used to signal communication failure
#define SWBB_FAIL       65535
// Used to signal acknowledgment
#define SWBB_ACK            6
// Used for pin handling
#define SWBB_NOT_ASSIGNED 255

/* Transmission speed modes (see Timing.h)
  MODE   1: 1.95kB/s - 15625Bd
  MODE   2: 2.21kB/s - 17696Bd
  MODE   3: 2.94kB/s - 23529Bd
  MODE   4: 3.40kB/s - 27210Bd */
#include "Timing.h"

// Recommended receive time for this strategy, in microseconds
#ifndef SWBB_RECEIVE_TIME
  #define SWBB_RECEIVE_TIME 1000
#endif

class SoftwareBitBang {
  public:
    /* Returns the delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < SWBB_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };


    /* Begin method, to be called on initialization:
       (returns always true) */

    bool begin(uint8_t did = 0) {
      PJON_DELAY(PJON_RANDOM(SWBB_INITIAL_DELAY) + did);
      return true;
    };


    /* Check if the channel is free for transmission:
       If reading 10 bits no 1 is detected there is no active transmission */

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


    /* Returns the recommended receive time for this strategy: */

    static uint16_t get_receive_time() {
      return SWBB_RECEIVE_TIME;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(SWBB_COLLISION_DELAY));
    };


    /* Read a byte from the pin */

    uint8_t read_byte() {
      uint8_t byte_value = 0B00000000;
      // Delay until the center of the first bit
      PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH / 2);
      for(uint8_t i = 0; i < 7; i++) {
        // Read in the center of the bit
        byte_value += PJON_IO_READ(_input_pin) << i;
        // Delay until the center of the next one
        PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH);
      }
      // Read in the center of the last one
      byte_value += PJON_IO_READ(_input_pin) << 7;
      // Delay until the end of the last bit
      PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH / 2);
      return byte_value;
    };


    /* Receive byte if in sync: */

    uint16_t receive_byte() {
      if(sync()) return read_byte();
      return SWBB_FAIL;
    };


    /* Receive byte response:
       Transmitter emits a SWBB_BIT_WIDTH / 4 long bit and tries
       to get a response cyclically for SWBB_RESPONSE_TIMEOUT microseconds.
       Receiver synchronizes to the falling edge of the last incoming
       bit and transmits PJON_ACK */

    uint16_t receive_response() {
      uint16_t response = SWBB_FAIL;
      uint32_t time = PJON_MICROS();
      PJON_IO_PULL_DOWN(_input_pin);
      while( // Wait for the receiver's high to arrive
        ((uint32_t)(PJON_MICROS() - time) < (SWBB_LATENCY * 2)) &&
        !PJON_IO_READ(_input_pin)
      );
      time = PJON_MICROS();
      while( // Wait for low
        ((uint32_t)(PJON_MICROS() - time) < _timeout) &&
        PJON_IO_READ(_input_pin)
      );
      if((uint32_t)(PJON_MICROS() - time) < _timeout) {
        time = PJON_MICROS();
        while( // Wait for incoming sync
          (
            (uint32_t)(PJON_MICROS() - time) <
            (SWBB_BIT_WIDTH + SWBB_DEVIATION)
          ) && !PJON_IO_READ(_input_pin)
        );
        if(sync()) response = receive_byte();
      }
      return response;
    };


    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      uint16_t result;
      if(max_length == PJON_PACKET_MAX_LENGTH) {
        uint32_t time = PJON_MICROS();
        // Look for a frame initializer
        if(!sync() || !sync() || !sync()) return SWBB_FAIL;
        // Check its timing consistency
        if(
          (uint32_t)(PJON_MICROS() - time) <
          (((SWBB_BIT_WIDTH * 3) + (SWBB_BIT_SPACER * 3)) - SWBB_ACCEPTANCE)
        ) return SWBB_FAIL;
      }
      // Receive incoming bytes
      result = receive_byte();
      if(result == SWBB_FAIL) return SWBB_FAIL;
      *data = result;
      return 1;
    };

    /* Function called at the end of a frame reception: */

    void frame_end(bool ack) {
      if(!ack) return;
      PJON_IO_MODE(_output_pin, OUTPUT);
      PJON_IO_WRITE(_output_pin, HIGH);
    };

    /* Every byte is prepended with a synchronization pad made by 2
       padding bits. The first is a longer than standard logic 1 followed
       by a standard logic 0.
       __________ ___________________________
      | SyncPad  | Byte                      |
      |______    |___       ___     _____    |
      |  |   |   |   |     |   |   |     |   |
      |  | 1 | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
      |__|___|___|___|_____|___|___|_____|___|
         |
      Minimum acceptable HIGH padding bit duration

    The reception tecnique is based on finding a logic 1 as long as the
    first padding bit within a certain threshold, synchronizing to its
    falling edge and checking if it is followed by a logic 0. If this
    pattern is recognised, reception starts, if not, interference,
    synchronization loss or simply absence of communication is
    detected at byte level. */

    void send_byte(uint8_t b) {
      pulse(1);
      for(uint8_t mask = 0x01; mask; mask <<= 1) {
        PJON_IO_WRITE(_output_pin, b & mask);
        PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH);
      }
    };


    /* Send byte response:
       Transmitter sends a SWBB_BIT_WIDTH / 4 microseconds long HIGH bit and
       tries to receive a response cyclically for SWBB_RESPONSE_TIMEOUT
       microseconds. Receiver synchronizes to the falling edge of the last
       incoming bit and transmits its response */

    void send_response(uint8_t response) {
      PJON_IO_WRITE(_output_pin, LOW);
      PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH);
      if(response == SWBB_ACK) {
        pulse(1);
        send_byte(response);
        PJON_IO_PULL_DOWN(_output_pin);
      }
    };


    /* The data is prepended with a frame initializer composed by 3
       synchronization pads to signal the start of a frame.
     _________________ __________________________________
    |   FRAME INIT    | DATA 1-65535 bytes               |
    |_____ _____ _____|________________ _________________|
    |Sync |Sync |Sync |Sync | Byte     |Sync | Byte      |
    |___  |___  |___  |___  |     __   |___  |      _   _|
    |   | |   | |   | |   | |    |  |  |   | |     | | | |
    | 1 |0| 1 |0| 1 |0| 1 |0|0000|11|00| 1 |0|00000|1|0|1|
    |___|_|___|_|___|_|___|_|____|__|__|___|_|_____|_|_|_|

    Send a frame: */

    void send_frame(uint8_t *data, uint16_t length) {
      _timeout = (length * SWBB_RESPONSE_OFFSET) + SWBB_LATENCY;
      PJON_IO_MODE(_output_pin, OUTPUT);
      pulse(3); // Send frame initializer
      for(uint16_t b = 0; b < length; b++)
        send_byte(data[b]); // Send data
      PJON_IO_PULL_DOWN(_output_pin);
    };


    /* Check if a synchronization pad is incoming:
     __________
    | SyncPad  |
    |______    |
    |  |   |   |
    |  | 1 | 0 |
    |__|___|___|
       |
    Minimum acceptable HIGH padding bit duration

    The reception tecnique is based on finding a logic 1 as long as the
    first padding bit within a certain threshold, synchronizing to its
    falling edge and checking if it is followed by a logic 0. If this
    pattern is recognised, synchronization may have been obtained, if
    not, interference, synchronization loss or simply absence of
    communication is detected at byte level: */

    bool sync() {
      PJON_IO_PULL_DOWN(_input_pin);
      if((_output_pin != _input_pin) && (_output_pin != SWBB_NOT_ASSIGNED))
        PJON_IO_PULL_DOWN(_output_pin);
      uint32_t time = PJON_MICROS();
      while(
        PJON_IO_READ(_input_pin) &&
        ((uint32_t)(PJON_MICROS() - time) <= SWBB_BIT_SPACER)
      );
      time = PJON_MICROS() - time;
      if(time < SWBB_ACCEPTANCE)
        return false;
      else {
        PJON_DELAY_MICROSECONDS((SWBB_BIT_WIDTH / 2) - SWBB_READ_DELAY);
        if(!PJON_IO_READ(_input_pin)) {
          PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH / 2);
          return true;
        }
      }
      return false;
    };

    /* Emit synchronization pulse: */

    void pulse(uint8_t n) {
      while(n--) {
        PJON_IO_WRITE(_output_pin, HIGH);
        PJON_DELAY_MICROSECONDS(SWBB_BIT_SPACER);
        PJON_IO_WRITE(_output_pin, LOW);
        PJON_DELAY_MICROSECONDS(SWBB_BIT_WIDTH);
      }
    };

    /* Set the communicaton pin: */

    void set_pin(uint8_t pin) {
      PJON_IO_PULL_DOWN(pin);
      _input_pin = pin;
      _output_pin = pin;
    };


    /* Set a pair of communication pins: */

    void set_pins(
      uint8_t input_pin = SWBB_NOT_ASSIGNED,
      uint8_t output_pin = SWBB_NOT_ASSIGNED
    ) {
      PJON_IO_PULL_DOWN(input_pin);
      PJON_IO_PULL_DOWN(output_pin);
      _input_pin = input_pin;
      _output_pin = output_pin;
    };

  private:
    uint16_t _timeout;
    uint8_t  _input_pin;
    uint8_t  _output_pin;
};
