
/* AnalogSampling data link layer 18/02/2017
   used as a Strategy by the PJON framework
   It complies with PJDLS (Padded Jittering Data Link byte Stuffed)
   specification v2.0

   AnalogSampling is designed to sample digital data using analog readings.
   It can be effectively used to communicate data wirelessly through any
   sort of radiation transceiver.

   The most basic example is to use a couple of visible light LEDs as wireless
   transceivers connecting to the A0 pin of 2 Arduino boards.

   Leveraging the interesting characteristics of LEDs:
   - Emit photons if electrons are travelling through the junction
   - Emit electrons if photons are hitting the junction, thanks to the
     photo-electric effect

   It is possibile to use them as wireless (bidirectional) transceivers!

   The obtained range is related to:
   - Analog reference (voltage reading resolution)
   - LED sensitivity to the signal
   - Available current for transmitter

   It is possible to use this strategy with a couple of LEDs and an optic
   fiber cable to have a safe EM interference free data link.

   It is possible to use this strategy to also communicate long range
   wirelessly using a couple of photodiodes and laser emitters. It may be
   necessary to tweak timing constants in Timing.h.
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

/* Default reading state threshold: */

#ifndef AS_THRESHOLD
  #define AS_THRESHOLD  1
#endif

/* MODE 1 performance (default):
   Transfer speed: 1024Bb or 128B/s

   MODE 2 performance:
   Transfer speed: 1361Bb or 170B/s

   MODE 3 performance:
   Transfer speed: 3352Bb or 419B/s

   MODE 4 performance:
   Transfer speed: 5069Bb or 633B/s

   MODE 5 performance:
   Transfer speed: 12658Bb or 1582B/s
   ADC prescale 8 (Caution out of specification)

  Set default transmission mode */
#ifndef AS_MODE
  #define AS_MODE 1
#endif

// Used to signal communication failure
#define AS_FAIL         65535
// Used for pin handling
#define AS_NOT_ASSIGNED   255
// START frame symbol 10010101 - 0x95 - 
#define AS_START          149
// END   frame symbol 11101010 - 0xea - ê
#define AS_END            234
// ESCAPE      symbol 10111011 - 0xBB - »
#define AS_ESC            187

#include "Timing.h"

// Recommended receive time for this strategy, in microseconds
#ifndef AS_RECEIVE_TIME
  #define AS_RECEIVE_TIME 1000
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


    /* Begin method, to be called on initialization:
       (returns always true) */

    bool begin(uint8_t did = 0) {
      #ifdef ARDUINO
        // Set ADC clock prescale
        #if AS_PRESCALE ==  8
          cbi(ADCSRA, ADPS2);
          sbi(ADCSRA, ADPS1);
          sbi(ADCSRA, ADPS0);
        #elif AS_PRESCALE == 16
          sbi(ADCSRA, ADPS2);
          cbi(ADCSRA, ADPS1);
          cbi(ADCSRA, ADPS0);
        #elif AS_PRESCALE == 32
          sbi(ADCSRA, ADPS2);
          cbi(ADCSRA, ADPS1);
          sbi(ADCSRA, ADPS0);
        #endif
      #endif
      PJON_DELAY(PJON_RANDOM(AS_INITIAL_DELAY) + did);
      compute_analog_read_duration();
      _last_byte = receive_byte();
      return true;
    };


    /* Check if the channel is free for transmission:
    If receiving 10 bits no 1s are detected
    there is no active transmission */

    bool can_start() {
      uint32_t time = PJON_MICROS();
      while(
        (uint32_t)(PJON_MICROS() - time) <=
        (AS_BIT_SPACER + (AS_BIT_WIDTH * 9))
      ) if(receive_byte() != PJON_FAIL) return false;
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(AS_COLLISION_DELAY));
      if((uint16_t)PJON_ANALOG_READ(_input_pin) > threshold) return false;
      return true;
    };


    /* compute PJON_ANALOG_READ duration: */

    void compute_analog_read_duration() {
      uint32_t time = PJON_MICROS();
      PJON_ANALOG_READ(_input_pin);
      _analog_read_time = (uint32_t)(PJON_MICROS() - time);
    };


    /* Function called at the end of a frame reception: */

    void frame_end(bool ack) {
      (void)ack; // Avoid unused parameter warning
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return AS_MAX_ATTEMPTS;
    };


    /* Returns the recommended receive time for this strategy: */

    static uint16_t get_receive_time() {
      return AS_RECEIVE_TIME;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(AS_COLLISION_DELAY));
    };


    /* Read a byte from the pin */

    uint8_t read_byte() {
      uint16_t bit_value = 0;
      uint16_t high_bit = 0;
      uint16_t low_bit = 0;
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
      if(_output_pin != AS_NOT_ASSIGNED && _output_pin != _input_pin)
        PJON_IO_PULL_DOWN(_output_pin);
      uint32_t time = PJON_MICROS();

      if(
        (uint32_t)(PJON_MICROS() - _last_update) >
        AS_THRESHOLD_DECREASE_INTERVAL
      ) {
        threshold *= 0.9;
        _last_update = PJON_MICROS();
      }

      while(
        ((uint16_t)(PJON_ANALOG_READ(_input_pin)) > threshold) &&
        ((uint32_t)(PJON_MICROS() - time) <= AS_BIT_SPACER)
      ); // Do nothing

      time = PJON_MICROS() - time;
      if(time < AS_BIT_SPACER * 0.75) return AS_FAIL;
      if(time <= AS_BIT_SPACER * 1.25) {
        PJON_DELAY_MICROSECONDS(AS_BIT_WIDTH);
        return read_byte();
      }
      return AS_FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      PJON_IO_PULL_DOWN(_input_pin);
      if(_output_pin != AS_NOT_ASSIGNED && _output_pin != _input_pin)
        PJON_IO_WRITE(_output_pin, LOW);
      uint16_t response = AS_FAIL;
      uint32_t time = PJON_MICROS();
      while(
        (response != PJON_ACK) &&
        (uint32_t)(PJON_MICROS() - AS_RESPONSE_TIMEOUT) <= time
      ) response = receive_byte();
      return response;
    };


    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      uint16_t result;
      // No initial flag, byte-stuffing violation
      if(max_length == PJON_PACKET_MAX_LENGTH)
        if(
          (receive_byte() != AS_START) ||
          (_last_byte == AS_ESC)
        ) return AS_FAIL;
      result = receive_byte();
      if(result == AS_FAIL) return AS_FAIL;
      // Unescaped START byte stuffing violation
      if(result == AS_START) return AS_FAIL;
      if(result == AS_ESC) {
        result = receive_byte();
        // Escaping byte-stuffing violation
        if((result != AS_START) && (result != AS_ESC) && (result != AS_END))
          return AS_FAIL;
        result ^= AS_ESC;
      }
      // No end flag, byte-stuffing violation
      if(max_length == 1 && receive_byte() != AS_END)
        return AS_FAIL;
      *data = result;
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


    /* Send a frame: */

    void send_frame(uint8_t *data, uint16_t length) {
      PJON_IO_MODE(_output_pin, OUTPUT);
      // Add frame flag
      send_byte(AS_START);
      for(uint16_t b = 0; b < length; b++)
        if( // Byte-stuffing
          (data[b] == AS_START) ||
          (data[b] == AS_ESC) ||
          (data[b] == AS_END)
        ) {
          send_byte(AS_ESC);
          send_byte(data[b] ^ AS_ESC);
        } else send_byte(data[b]);
      send_byte(AS_END);
      PJON_IO_PULL_DOWN(_output_pin);
    };


    /* Set the communicaton pin: */

    void set_pin(uint8_t pin) {
      PJON_IO_PULL_DOWN(pin);
      _input_pin = pin;
      _output_pin = pin;
    };


    /* Set a pair of communication pins: */

    void set_pins(
      uint8_t input_pin = AS_NOT_ASSIGNED,
      uint8_t output_pin = AS_NOT_ASSIGNED
    ) {
      PJON_IO_PULL_DOWN(input_pin);
      PJON_IO_PULL_DOWN(output_pin);
      _input_pin = input_pin;
      _output_pin = output_pin;
    };


    /* Set the threshold analog value between a LOW and a HIGH read: */

    void set_threshold(uint16_t value) {
      threshold = value;
    };


    uint16_t threshold = AS_THRESHOLD;
  private:
    uint8_t  _last_byte;
    uint16_t _analog_read_time;
    uint8_t  _input_pin;
    uint8_t  _output_pin;
    uint32_t _last_update;
};
