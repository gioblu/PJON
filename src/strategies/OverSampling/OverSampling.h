
/* OverSampling 1 or 2 wires software-emulated data link
   used as a Strategy by PJON (included in version v3.0)
   Compliant with PJDLR (Padded Jittering Data Link Radio) specification v2.0

   It uses the over-sampling method to receive data, that is generally
   implemented on physical layers characterized by low bandwidth and high
   noise such as ASK/FSK radio transceivers.
   ____________________________________________________________________________

   Copyright 2015-2018 Giovanni Blu Mitolo gioscarab@gmail.com

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

/* MODE 1:
   Medium: STX882/SRX882 433MHz ASK/FSK modules or 315/433 MHz modules (green)
    RX http://nicerf.com/manage/upfile/indexbanner/635331970881921250.pdf
    TX http://nicerf.com/manage/upfile/indexbanner/635169453223382155.pdf
    Timing for other hardware can be easily implemented in Timing.h

   Performance:
    Transfer speed: 1620Bb or 202B/s
    Absolute  communication speed: 180B/s (data length 20 of characters)
    Data throughput: 150B/s (data length 20 of characters)
    Range: 250m with no line of sight, 5 km with direct line of sight */

#ifndef OS_MODE
  #define OS_MODE 1
#endif

// Used to signal communication failure
#define OS_FAIL       65535

// Used for pin handling
#define OS_NOT_ASSIGNED 255

#include "Timing.h"

class OverSampling {
  public:

    /* Returns the suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < OS_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };


    /* Begin method, to be called on initialization:
       (returns always true) */

    bool begin(uint8_t did = 0) {
      PJON_DELAY(PJON_RANDOM(OS_INITIAL_DELAY) + did);
      return true;
    };


    /* Check if the channel is free for transmission:
    If receiving 10 bits no 1s are detected
    there is no active transmission */

    bool can_start() {
      float value = 0.5;
      uint32_t time = PJON_MICROS();
      PJON_IO_MODE(_input_pin, INPUT);
      while((uint32_t)(PJON_MICROS() - time) < OS_TIMEOUT)
        value = (value * 0.999)  + (PJON_IO_READ(_input_pin) * 0.001);
      if(value > 0.5) return false;
      if(PJON_IO_READ(_input_pin)) return false;
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(OS_COLLISION_DELAY));
      if(PJON_IO_READ(_input_pin)) return false;
      return true;
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return OS_MAX_ATTEMPTS;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(OS_COLLISION_DELAY));
    };


    /* Read a byte from the pin */

    uint8_t read_byte() {
      uint8_t byte_value = 0B00000000;
      for(uint8_t i = 0; i < 8; i++) {
        uint32_t time = PJON_MICROS();
        float value = 0.5;
        while((uint32_t)(PJON_MICROS() - time) < OS_BIT_WIDTH)
          value = ((value * 0.999) + (PJON_IO_READ(_input_pin) * 0.001));
        byte_value += (value > 0.5) << i;
      }
      return byte_value;
    };


    /* Try to receive a byte: */

    uint16_t receive_byte() {
      if(sync()) return read_byte();
      return OS_FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      if(_output_pin != OS_NOT_ASSIGNED && _output_pin != _input_pin)
        PJON_IO_WRITE(_output_pin, LOW);
      uint16_t response = OS_FAIL;
      uint32_t time = PJON_MICROS();
      while(
        (response != PJON_ACK) &&
        ((uint32_t)(PJON_MICROS() - OS_TIMEOUT) <= time)
      ) response = receive_byte();
      return response;
    };


    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      uint16_t result;
      if(max_length == PJON_PACKET_MAX_LENGTH) {
        uint32_t time = PJON_MICROS();
        // Look for frame initializer
        if(!sync() || !sync() || !sync()) return OS_FAIL;
        if( // Check its timing consistency
          (uint32_t)(PJON_MICROS() - time) <
          ((OS_BIT_WIDTH * 3) + (OS_BIT_SPACER * 3))
        ) return OS_FAIL;
      } // Receive incoming byte
      result = receive_byte();
      if(result == OS_FAIL) return OS_FAIL;
      *data = result;
      return 1;
    };


    /* Every byte is prepended with 2 synchronization padding bits. The first
       is a shorter than standard logic 1 followed by a standard logic 0.
       _____ ___________________________
      |Sync | Byte                      |
      |_    |___       ___     _____    |
      | |   |   |     |   |   |     |   |
      |1| 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
      |_|___|___|_____|___|___|_____|___|

    The reception tecnique is based on finding a logic 1 as long as the
    first padding bit, synchronizing to its falling edge and checking if
    it is followed by a logic 0. If this pattern is recognised, reception
    starts, if not, interference, synchronization loss or simply absence
    of communication is detected at byte level. */

    void send_byte(uint8_t b) {
      PJON_IO_WRITE(_output_pin, HIGH);
      PJON_DELAY_MICROSECONDS(OS_BIT_SPACER);
      PJON_IO_WRITE(_output_pin, LOW);
      PJON_DELAY_MICROSECONDS(OS_BIT_WIDTH);
      for(uint8_t mask = 0x01; mask; mask <<= 1) {
        PJON_IO_WRITE(_output_pin, b & mask);
        PJON_DELAY_MICROSECONDS(OS_BIT_WIDTH);
      }
    };


    /* Send preamble with a requested number of pulses: */

    void send_preamble() {
      #if OS_PREAMBLE_PULSE_WIDTH > 0
        PJON_IO_WRITE(_output_pin, HIGH);
        uint32_t time = PJON_MICROS();
        while((uint32_t)(PJON_MICROS() - time) < OS_PREAMBLE_PULSE_WIDTH);
        PJON_IO_WRITE(_output_pin, LOW);
        PJON_DELAY_MICROSECONDS(OS_TIMEOUT - OS_BIT_WIDTH);
      #endif
    };


    /* Send byte response to package transmitter */

    void send_response(uint8_t response) {
      PJON_IO_PULL_DOWN(_input_pin);
      PJON_IO_MODE(_output_pin, OUTPUT);
      // Send initial transmission preamble if required
      if(OS_PREAMBLE_PULSE_WIDTH) send_preamble();
      send_byte(response);
      PJON_IO_PULL_DOWN(_output_pin);
    };


    /* Send a frame: */

    void send_frame(uint8_t *data, uint16_t length) {
      PJON_IO_MODE(_output_pin, OUTPUT);
      // Send initial transmission preamble if required
      if(OS_PREAMBLE_PULSE_WIDTH) send_preamble();
      // Send frame inititializer
      for(uint8_t i = 0; i < 3; i++) {
        PJON_IO_WRITE(_output_pin, HIGH);
        PJON_DELAY_MICROSECONDS(OS_BIT_SPACER);
        PJON_IO_WRITE(_output_pin, LOW);
        PJON_DELAY_MICROSECONDS(OS_BIT_WIDTH);
      } // Send data
      for(uint16_t b = 0; b < length; b++)
        send_byte(data[b]);
      PJON_IO_PULL_DOWN(_output_pin);
    };


    bool sync() {
      PJON_IO_PULL_DOWN(_input_pin);
      if(_output_pin != OS_NOT_ASSIGNED && _output_pin != _input_pin)
        PJON_IO_PULL_DOWN(_output_pin);
      float value = 0.5;
      uint32_t time = PJON_MICROS();
      /* Average pin value until the pin stops to be HIGH or passed more
         time than BIT_SPACER duration */
      while(
        ((uint32_t)(PJON_MICROS() - time) < OS_BIT_SPACER) &&
        PJON_IO_READ(_input_pin)
      ) value = (value * 0.999) + (PJON_IO_READ(_input_pin) * 0.001);
      /* Save how much time passed */
      time = PJON_MICROS();
      /* If the pin value is in average more than 0.5, is a 1, and if lasted
         more than ACCEPTANCE (a minimum HIGH duration) and what is coming
         after is a LOW bit probably a byte is coming so try to receive it. */
      if(value > 0.5) {
        value = 0.5;
        while((uint32_t)(PJON_MICROS() - time) < OS_BIT_WIDTH)
          value = (value * 0.999) + (PJON_IO_READ(_input_pin) * 0.001);
        if(value < 0.5) return true;
        return false;
      }
      return false;
    };


    /* Set the communicaton pin: */

    void set_pin(uint8_t pin) {
      PJON_IO_PULL_DOWN(pin);
      _input_pin = pin;
      _output_pin = pin;
    };


    /* Set a pair of communication pins: */

    void set_pins(
      uint8_t input_pin = OS_NOT_ASSIGNED,
      uint8_t output_pin = OS_NOT_ASSIGNED
    ) {
      PJON_IO_PULL_DOWN(input_pin);
      PJON_IO_PULL_DOWN(output_pin);
      _input_pin = input_pin;
      _output_pin = output_pin;
    };

  private:
    uint8_t _input_pin;
    uint8_t _output_pin;
};
