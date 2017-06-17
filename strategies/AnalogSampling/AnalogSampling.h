
/* AnalogSampling data link layer 18/02/2017
   used as a Strategy by the PJON framework
   Compliant with the Padded jittering data link layer specification v0.1

   It is designed to sample digital data using analog readings.
   It can be effectively used to communicate data wirelessly, through
   any sort of radiation transceiver. The most basic example is to use a couple
   of visible light LEDs connected to the A0 pin used as wireless transceivers
   infact, leveraging the duality of LEDs:
   - Ability to emit photons if electrons are travelling through the junction
   - Ability to emit electrons if photons are hitting the junction
     (photo-electric effect) it is possibile to use them as
     wireless (bidirectional) transceivers!

   The obtained range is related to:
   - Analog reference (voltage reading resolution)
   - LED sensitivity to the signal
   - Available current for transmitter

   For a long range use case a couple of photodiodes and laser emitters is
   suggested. It may be necessary to teak timing constants in Timing.h.
   ____________________________________________________________________________

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

#include "Timing.h"

/* Default reading state threshold: */

#ifndef AS_THRESHOLD
  #define AS_THRESHOLD    1
#endif

/* _AS_STANDARD transmission mode performance:
   Transfer speed: 1024Bb or 128B/s
   Absolute  communication speed: 128B/s (data length 20 of characters)
   Data throughput: 100B/s (data length 20 of characters) */
#define AS_STANDARD  1

/* Set here the selected transmission mode - default STANDARD */
#ifndef AS_MODE
  #define AS_MODE AS_STANDARD
#endif

class AnalogSampling {
  public:

    /* Returns the suggested delay related to attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < AS_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };


    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t additional_randomness = 0) {
      PJON_DELAY_MICROSECONDS(
        (PJON_RANDOM(AS_INITIAL_DELAY) + additional_randomness) * 1000
      );
      PJON_IO_PULL_DOWN(_input_pin);
      if(_output_pin != _input_pin)
        PJON_IO_PULL_DOWN(_output_pin);
      uint32_t time = PJON_MICROS();
      compute_analog_read_duration();
      return true;
    };


    /* Check if the channel is free for transmission:
    If receiving 10 bits no 1s are detected
    there is no active transmission */

    bool can_start() {
      if(read_byte() != 0B00000000) return false;
      PJON_DELAY_MICROSECONDS(AS_BIT_SPACER / 2);
      if(PJON_ANALOG_READ(_input_pin) > threshold) return false;
      PJON_DELAY_MICROSECONDS(AS_BIT_SPACER / 2);
      if(PJON_ANALOG_READ(_input_pin) > threshold) return false;
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(AS_COLLISION_DELAY));
      if(PJON_ANALOG_READ(_input_pin) > threshold) return false;
      return true;
    };


    /* compute PJON_ANALOG_READ duration: */

    void compute_analog_read_duration() {
      uint32_t time = PJON_MICROS();
      PJON_ANALOG_READ(_input_pin);
      _analog_read_time = (uint32_t)(PJON_MICROS() - time);
      for(uint8_t i = 0; i < 10; i++) {
        time = PJON_MICROS();
        PJON_ANALOG_READ(_input_pin);
        _analog_read_time =
          (_analog_read_time * 0.75) +
          ((uint32_t)(PJON_MICROS() - time) * 0.25);
        // TODO - check for granularity
      }
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return AS_MAX_ATTEMPTS;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(AS_COLLISION_DELAY));
    };


    /* Read a byte from the pin */

    uint8_t read_byte() {
      int bit_value;
      int high_bit = 0;
      int low_bit = 0;
      uint8_t byte_value = 0;
      for(int i = 0; i < 8; i++) {
        long time = PJON_MICROS();
        PJON_DELAY_MICROSECONDS((AS_BIT_WIDTH / 2) - AS_READ_DELAY);
        bit_value = PJON_ANALOG_READ(_input_pin);
        byte_value += (bit_value > threshold) << i;

        high_bit =
          (((bit_value > threshold) ? bit_value : high_bit) + high_bit) / 2;

        low_bit  =
          (((bit_value < threshold) ? bit_value : low_bit) + low_bit) / 2;

        PJON_DELAY_MICROSECONDS(
          AS_BIT_WIDTH - (uint32_t)(PJON_MICROS() - time)
        );
      }
      threshold = (high_bit + low_bit) / 2;
      _last_update = PJON_MICROS();
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
      PJON_IO_PULL_DOWN(_input_pin);
      if(_output_pin != PJON_NOT_ASSIGNED && _output_pin != _input_pin)
        PJON_IO_PULL_DOWN(_output_pin);
      uint32_t time = PJON_MICROS();

      if(
        (
          (uint32_t)(PJON_MICROS() - _last_update) >
          AS_THRESHOLD_DECREASE_INTERVAL
        ) && threshold
      ) {
        threshold *= 0.25;
        _last_update = PJON_MICROS();
      }

      while(
        (PJON_ANALOG_READ(_input_pin) > threshold) &&
        ((uint32_t)(PJON_MICROS() - time) <= AS_BIT_SPACER)
      ); // Do nothing

      time  = PJON_MICROS() - time;
      if(time >= AS_BIT_SPACER * 0.75 && time <= AS_BIT_SPACER * 1.25) {
        PJON_DELAY_MICROSECONDS(AS_BIT_WIDTH);
        return read_byte();
      }
      return PJON_FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      PJON_IO_PULL_DOWN(_input_pin);
      if(_output_pin != PJON_NOT_ASSIGNED && _output_pin != _input_pin)
        PJON_IO_WRITE(_output_pin, LOW);
      uint16_t response = PJON_FAIL;
      uint32_t time = PJON_MICROS();
      while(
        (response != PJON_ACK) &&
        (uint32_t)(PJON_MICROS() - AS_RESPONSE_TIMEOUT) <= time
      ) response = receive_byte();
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
      PJON_DELAY_MICROSECONDS(AS_BIT_SPACER);
      PJON_IO_WRITE(_output_pin, LOW);
      PJON_DELAY_MICROSECONDS(AS_BIT_WIDTH);
      for(uint8_t mask = 0x01; mask; mask <<= 1) {
        PJON_IO_WRITE(_output_pin, b & mask);
        PJON_DELAY_MICROSECONDS(AS_BIT_WIDTH);
      }
    };


    /* Send byte response to package transmitter */

    void send_response(uint8_t response) {
      PJON_DELAY_MICROSECONDS(AS_BIT_WIDTH);
      PJON_IO_PULL_DOWN(_input_pin);
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


    /* Set the threshold analog value between a LOW and a HIGH read: */

    void set_threshold(uint16_t value) {
      threshold = value;
    };


    uint16_t threshold = AS_THRESHOLD;
  private:
    uint16_t _analog_read_time;
    uint8_t  _input_pin;
    uint8_t  _output_pin;
    uint32_t _last_update;
};
