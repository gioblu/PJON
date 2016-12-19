
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

#include <Arduino.h>

#define THROUGH_SERIAL_MAX_BYTE_TIME         100000
/* 1 second maximum timeframe you can have between every receive call in
   any of the connected devices without loosing synchronous acknowledment
   because delaying. */

#define THROUGH_SERIAL_FREE_TIME_BEFORE_START   500
/* 0.5 milliseconds minimum timeframe of free port before transmitting

   This timing configuration is ok for a master-slave setup, but could lead to
   collisions if used in a multi-master setup.

   If using ThroughSerial multi-master, NEVER set
   THROUGH_SERIAL_FREE_TIME_BEFORE_START < THROUGH_SERIAL_MAX_BYTE_TIME
   or a device could start transmitting while a couple is still exchanging an acknowledge

   i.e.
   #define THROUGH_SERIAL_MAX_BYTE_TIME           100000
   #define THROUGH_SERIAL_FREE_TIME_BEFORE_START  110000

   Above is shown multi-master compatible setup able to receive a synchronous
   acknowledgment with a maximum delay 100 milliseconds. Channel analysis before
   transmission is set to 110 milliseconds to avoid collisions.

   Which is the correct value for your setup depends on the maximum average time
   interval between every receive call in your system. THROUGH_SERIAL_MAX_BYTE_TIME
   should be around the same duration. So in a sketch where there is only a
   delay(10) between every receive call 10000 should be the correct value for
   THROUGH_SERIAL_MAX_BYTE_TIME.

   If your tasks timing are long and a satisfactory setup can't be reached
   consider to drop the use of the synchronous acknowledge and start using the
   asynchronous acknowledgment instead. */

class ThroughSerial {
  public:
    Stream *serial = NULL;

    boolean can_start() {
      delayMicroseconds(random(0, COLLISION_DELAY));
      if(serial->available()) return false;

      if((uint32_t)(micros() - _last_reception_time) < THROUGH_SERIAL_FREE_TIME_BEFORE_START)
        return false;

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
      if(_enable_RS485_pin != NOT_ASSIGNED)
        digitalWriteFast(_enable_RS485_pin, HIGH);

      send_byte(response);
      serial->flush();

      if(_enable_RS485_pin != NOT_ASSIGNED)
        digitalWriteFast(_enable_RS485_pin, LOW);
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint8_t length) {
      if(_enable_RS485_pin != NOT_ASSIGNED)
        digitalWriteFast(_enable_RS485_pin, HIGH);

      for(uint8_t b = 0; b < length; b++)
        send_byte(string[b]);
      serial->flush();

      if(_enable_RS485_pin != NOT_ASSIGNED)
        digitalWriteFast(_enable_RS485_pin, LOW);
    };


    /* Pass the Serial port where you want to operate with */

    void set_serial(Stream *serial_port) {
      serial = serial_port;
    };


    /* Pass the enable transmission pin for RS485 if in use */

    void set_enable_RS485_pin(uint8_t pin) {
      _enable_RS485_pin = pin;
      pinModeFast(_enable_RS485_pin, OUTPUT);
    };

  private:
    uint32_t _last_reception_time;
    uint8_t  _enable_RS485_pin = NOT_ASSIGNED;
};
