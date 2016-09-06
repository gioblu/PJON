
/* ThroughHardwareSerial digital communication transport layer
   used as a Strategy by the PJON framework (included in version v4.1)
   ____________________________________________________________________________

   ThroughHardwareSerial, Copyright (c) 2016 by Giovanni Blu Mitolo
   and Fred Larsen All rights reserved.

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

#define THROUGH_HARDWARE_SERIAL_MAX_TIME 1000000 // Wait up to 1 second for an incoming byte

class ThroughHardwareSerial {
  public:
    HardwareSerial *serial;

    /* Returns the Serial object value i.e. if(Serial) */

    boolean can_start() {
      return (serial != NULL) && *serial; // Check if pointer
    };


    /* Try to receive a byte with a maximum waiting time */

    uint16_t receive_byte() {
      uint32_t time = micros();
      while(micros() - time < THROUGH_HARDWARE_SERIAL_MAX_TIME)
        if(serial->available() > 0)
          return (uint8_t)serial->read();
      return FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      return receive_byte();
    };


    /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b) {
      serial->write(b);
      serial->flush();
    };


    /* Send byte response to the packet's transmitter */

    void send_response(uint8_t response) {
      send_byte(response);
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint8_t length) {
      for(uint8_t b = 0; b < length; b++)
        send_byte(string[b]);
    };


    /* Pass the Serial port where you want to operate with */

    void set_serial(HardwareSerial *serial_port) {
      serial = serial_port;
    };
};
