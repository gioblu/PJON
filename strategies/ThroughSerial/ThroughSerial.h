
/* ThroughSerial digital communication data link layer
   used as a Strategy by the PJON framework (included in version v4.1)

   Contributors:
    - Fred Larsen, Development, testing and debugging
    - Zbigniew Zasieczny, collision avoidance multi-drop RS485 (latency)
      and SoftwareSerial compatibility
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

#include <Arduino.h>
#include <SoftwareSerial.h>

#define THROUGH_SERIAL_MAX_BYTE_TIME        10000  // Wait up to 10 milliseconds for an incoming byte
#define THROUGH_SERIAL_FREE_TIME_BEFORE_START 500  // 0.5 milliseconds of free channell before sending

class ThroughSerial {
  public:
    Stream *serial = NULL;

    boolean can_start() {
      if(serial->available()) return false;

      if(
        (uint32_t)(micros() - _last_reception_time) <
        random(THROUGH_SERIAL_FREE_TIME_BEFORE_START, 2 * THROUGH_SERIAL_FREE_TIME_BEFORE_START)
      ) return false;

      return (serial != NULL);
    };


    /* Try to receive a byte with a maximum waiting time */

    uint16_t receive_byte() {
      uint32_t time = micros();
      while((uint32_t)(micros() - time) < THROUGH_SERIAL_MAX_BYTE_TIME)
        if(serial->available()) {
          _last_reception_time = micros();
          return (uint8_t)serial->read();
        }
      return FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      return receive_byte();
    };


    /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b) {
      serial->write(b);
    };


    /* Send byte response to the packet's transmitter */

    void send_response(uint8_t response) {
      send_byte(response);
      serial->flush();
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint8_t length) {
      for(uint8_t b = 0; b < length; b++)
        send_byte(string[b]);
      serial->flush();
    };


    /* Pass the Serial port where you want to operate with */

    void set_serial(HardwareSerial *serial_port) {
      serial = serial_port;
    };

    void set_serial(SoftwareSerial *serial_port) {
      serial = serial_port;
    };

  private:
    uint32_t _last_reception_time;
};
