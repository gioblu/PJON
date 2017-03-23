
/* ThroughSerial digital communication data link layer
   used as a Strategy by the PJON framework (included in version v4.1)

   Contributors:
    - Fred Larsen, Development, testing and debugging
    - Zbigniew Zasieczny, collision avoidance multi-drop RS485 (latency)
      and SoftwareSerial compatibility
    - Franketto (Arduino forum user) RS485 TX enable pin compatibility
   ____________________________________________________________________________

   ThroughSerial, Copyright (c) 2016 by Giovanni Blu Mitolo All rights reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include "Timing.h"

class ThroughSerial {
  public:
    #if defined(RPI)
      int16_t serial = 0;
    #elif defined(ARDUINO)
      Stream *serial = NULL;
    #endif
    /* Returns the suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < TS_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };

    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t additional_randomness = 0) {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TS_INITIAL_DELAY) + additional_randomness);
      return true;
    };


    /* Check if the channel is free for transmission: */

    bool can_start() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TS_COLLISION_DELAY));
      if(PJON_SERIAL_AVAILABLE(serial)) return false;
      if((uint32_t)(PJON_MICROS() - _last_reception_time) < TS_FREE_TIME_BEFORE_START)
        return false;
      return true;
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return TS_MAX_ATTEMPTS;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TS_COLLISION_DELAY));
    };


    /* Try to receive a byte with a maximum waiting time */

    uint16_t receive_byte() {
      uint32_t time = PJON_MICROS();
      while((uint32_t)(PJON_MICROS() - time) < TS_MAX_BYTE_TIME)
        if(PJON_SERIAL_AVAILABLE(serial)) {
          _last_reception_time = PJON_MICROS();
          return (uint8_t)PJON_SERIAL_READ(serial);
        }
      return PJON_FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      return receive_byte();
    };


    /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b) {
      PJON_SERIAL_WRITE(serial, b);
    };


    /* Send byte response to the packet's transmitter */

    void send_response(uint8_t response) {
      if(_enable_RS485_pin != PJON_NOT_ASSIGNED)
        PJON_IO_WRITE(_enable_RS485_pin, HIGH);

      send_byte(response);
      PJON_SERIAL_FLUSH(serial);

      if(_enable_RS485_pin != PJON_NOT_ASSIGNED)
        PJON_IO_WRITE(_enable_RS485_pin, LOW);
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint8_t length) {
      if(_enable_RS485_pin != PJON_NOT_ASSIGNED)
        PJON_IO_WRITE(_enable_RS485_pin, HIGH);

      for(uint8_t b = 0; b < length; b++)
        send_byte(string[b]);
      PJON_SERIAL_FLUSH(serial);

      if(_enable_RS485_pin != PJON_NOT_ASSIGNED)
        PJON_IO_WRITE(_enable_RS485_pin, LOW);
    };

    /* Pass the Serial port where you want to operate with */
  #if defined(RPI)
    void set_serial(int16_t serial_port) {
  #elif defined(ARDUINO)
    void set_serial(Stream *serial_port) {
  #endif
      serial = serial_port;
    };


    /* Pass the enable transmission pin for RS485 if in use */

    void set_enable_RS485_pin(uint8_t pin) {
      _enable_RS485_pin = pin;
      PJON_IO_MODE(_enable_RS485_pin, OUTPUT);
    };

  private:
    uint32_t _last_reception_time;
    uint8_t  _enable_RS485_pin = PJON_NOT_ASSIGNED;
};
