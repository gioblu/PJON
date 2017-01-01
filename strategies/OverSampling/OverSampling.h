
/* OverSampling 1 or 2 wires interrupts-less digital communication data link layer
   used as a Strategy by the PJON framework (included in version v3.0)
   Compliant with the Padded jittering data link layer specification v0.1
   ____________________________________________________________________________

   Copyright 2012-2017 Giovanni Blu Mitolo gioscarab@gmail.com

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#define _STXRX882_STANDARD 1

/* _STXRX882_STANDARD:
   Medium: STX882/SRX882 433Mhz ASK/FSK modules or 315/433 MHz modules (green)
    RX http://nicerf.com/manage/upfile/indexbanner/635331970881921250.pdf
    TX http://nicerf.com/manage/upfile/indexbanner/635169453223382155.pdf
    Timing for other hardware can be easily implemented in Timing.h

   Performance:
    Transfer speed: 1620Bb or 202B/s
    Absolute  communication speed: 180B/s (data length 20 of characters)
    Data throughput: 150B/s (data length 20 of characters)
    Range: 250m with no direct line of sight, 5km with direct line of sight  */

#ifndef OS_MODE
  #define OS_MODE _STXRX882_STANDARD
#endif

#include "Timing.h"
#include "../../utils/digitalWriteFast.h"

class OverSampling {
  public:

    /* Check if the channel is free for transmission:
    If receiving 10 bits no 1s are detected
    there is no active transmission */

    boolean can_start() {
      float value = 0.5;
      unsigned long time = micros();
      pinModeFast(_input_pin, INPUT);
      pullDownFast(_input_pin);
      while((uint32_t)(micros() - time) < OS_BIT_SPACER)
        value = digitalReadFast(_input_pin);
      if(value > 0.5) return false;
      value = 0.5;
      time = micros();
      for(uint8_t i = 0; i < 10; i++) {
        while((uint32_t)(micros() - time) < OS_BIT_WIDTH)
          value = (value * 0.999)  + (digitalReadFast(_input_pin) * 0.001);
        if(value > 0.5) return false;
      }
      delayMicroseconds(random(0, COLLISION_DELAY));
      if(digitalReadFast(_input_pin)) return false;
      return true;
    };


    /* Read a byte from the pin */

    uint8_t read_byte() {
      uint8_t byte_value = B00000000;
      for(uint8_t i = 0; i < 8; i++) {
        unsigned long time = micros();
        float value = 0.5;
        while((uint32_t)(micros() - time) < OS_BIT_WIDTH)
          value = ((value * 0.999) + (digitalReadFast(_input_pin) * 0.001));
        byte_value += (value > 0.5) << i;
      }
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
      pullDownFast(_input_pin);

      if(_output_pin != NOT_ASSIGNED && _output_pin != _input_pin)
        pullDownFast(_output_pin);

      float value = 0.5;
      unsigned long time = micros();
      /* Update pin value until the pin stops to be HIGH or passed more time than
         BIT_SPACER duration */
      while(((uint32_t)(micros() - time) < OS_BIT_SPACER) && digitalReadFast(_input_pin))
        value = (value * 0.999)  + (digitalReadFast(_input_pin) * 0.001);
      /* Save how much time passed */
      time = micros();
      /* If pin value is in average more than 0.5, is a 1, and if is more than
         ACCEPTANCE (a minimum HIGH duration) and what is coming after is a LOW bit
         probably a byte is coming so try to receive it. */
      if(value > 0.5) {
        value = 0.5;
        while((uint32_t)(micros() - time) < OS_BIT_WIDTH)
          value = (value * 0.999)  + (digitalReadFast(_input_pin) * 0.001);
        if(value < 0.5) return read_byte();
      }
      return FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      digitalWriteFast(_input_pin, LOW);

      if(_output_pin != NOT_ASSIGNED && _output_pin != _input_pin)
        digitalWriteFast(_output_pin, LOW);

      uint16_t response = FAIL;
      uint32_t time = micros();
      while(response == FAIL && (uint32_t)(micros() - OS_TIMEOUT) <= time)
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
      delayMicroseconds(OS_BIT_SPACER);
      digitalWriteFast(_output_pin, LOW);
      delayMicroseconds(OS_BIT_WIDTH);
      for(uint8_t mask = 0x01; mask; mask <<= 1) {
        digitalWriteFast(_output_pin, b & mask);
        delayMicroseconds(OS_BIT_WIDTH);
      }
    };


    /* Send byte response to package transmitter */

    void send_response(uint8_t response) {
      pinModeFast(_output_pin, OUTPUT);
      send_byte(response);
      digitalWriteFast(_output_pin, LOW); // Avoid 1 to 0 bit transition slope -\_
      pullDownFast(_output_pin);
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint16_t length) {
      pinModeFast(_output_pin, OUTPUT);
      for(uint16_t b = 0; b < length; b++)
        send_byte(string[b]);
      digitalWriteFast(_output_pin, LOW); // Avoid 1 to 0 bit transition slope -\_
      pullDownFast(_output_pin);
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
