
/* ThroughSerial digital communication data link layer
   used as a Strategy by the PJON framework (included in version v4.1)
   Compliant with TSDL (Tardy Serial Data Link) specification v2.0

   Contributors:
    - Fred Larsen, Development, testing and debugging
    - Zbigniew Zasieczny, collision avoidance multi-drop RS485 (latency)
      and SoftwareSerial compatibility
    - Franketto (Arduino forum user) RS485 TX enable pin compatibility
    - Endre Karlson separate RS485 enable pins handling, flush timing hack
    - hyndruide github user added set_RS485_delay
    - fabpolli github user RS485 missing ack delay report and testing
   ___________________________________________________________________________

   ThroughSerial,
   copyright 2016-2018 by Giovanni Blu Mitolo All rights reserved

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

// START frame symbol 10010101 - 0x95 - 
#define TS_START 149
// END   frame symbol 11101010 - 0xea - ê
#define TS_END   234
// ESCAPE      symbol 10111011 - 0xBB - »
#define TS_ESC   187

// Used to signal communication failure
#define TS_FAIL                     65535

// Used for pin handling
#define TS_NOT_ASSIGNED               255

#include "Timing.h"

class ThroughSerial {
  public:
    PJON_SERIAL_TYPE serial;

    /* Returns suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < TS_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };

    /* Begin method, to be called on initialization:
       (returns always true) */

    bool begin(uint8_t did = 0) {
      PJON_DELAY(PJON_RANDOM(TS_INITIAL_DELAY) + did);
      _last_reception_time = 0;
      _last_byte = receive_byte();
      return true;
    };


    /* Check if the channel is free for transmission: */

    bool can_start() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TS_COLLISION_DELAY));
      if(PJON_SERIAL_AVAILABLE(serial)) return false;
      if((uint32_t)(PJON_MICROS() - _last_reception_time) < TS_TIME_IN)
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

    uint16_t receive_byte(uint32_t time_out = TS_BYTE_TIME_OUT) {
      uint32_t time = PJON_MICROS();
      while((uint32_t)(PJON_MICROS() - time) < time_out) {
        if(PJON_SERIAL_AVAILABLE(serial)) {
          _last_reception_time = PJON_MICROS();
          int16_t read = PJON_SERIAL_READ(serial);
          #if defined(_WIN32)
            read = (uint8_t)read;
          #endif
          if(read >= 0) {
            _last_byte = (uint8_t)read;
            return _last_byte;
          }
        }
        #if defined(_WIN32)
          PJON_DELAY_MICROSECONDS(time_out / 10);
        #endif
      }
      return TS_FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response() {
      return receive_byte(TS_RESPONSE_TIME_OUT);
    };


    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      uint16_t result;
      // No initial flag, byte-stuffing violation
      if(max_length == PJON_PACKET_MAX_LENGTH)
        if(
          (receive_byte() != TS_START) ||
          (_last_byte == TS_ESC)
        ) return TS_FAIL;

      result = receive_byte();
      if(result == TS_FAIL) return TS_FAIL;

      // Unescaped START byte stuffing violation
      if(result == TS_START) return TS_FAIL;

      if(result == TS_ESC) {
        result = receive_byte();
        result ^= TS_ESC;
        // Avoid byte-stuffing violation
        if((result != TS_START) && (result != TS_ESC) && (result != TS_END))
          return TS_FAIL;
      }

      // No end flag, byte-stuffing violation
      if((max_length == 1) && (receive_byte() != TS_END))
        return TS_FAIL;
      *data = (uint8_t)result;
      return 1;
    };


    /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b) {
      uint32_t time = PJON_MICROS();
      while(
        (PJON_SERIAL_WRITE(serial, b) != 1) &&
        ((uint32_t)(PJON_MICROS() - time) < TS_BYTE_TIME_OUT)
      );
    };

    /* Send byte response to the packet's transmitter */

    void send_response(uint8_t response) {
      start_tx();
      wait_RS485_pin_change();
      send_byte(response);
      PJON_SERIAL_FLUSH(serial);
      wait_RS485_pin_change();
      end_tx();
    };


    /* Send a frame: */

    void send_frame(uint8_t *data, uint16_t length) {
      start_tx();
      uint16_t overhead = 2;
      // Add frame flag
      send_byte(TS_START);
      for(uint16_t b = 0; b < length; b++) {
        // Byte-stuffing
        if(
          (data[b] == TS_START) ||
          (data[b] == TS_ESC) ||
          (data[b] == TS_END)
        ) {
          send_byte(TS_ESC);
          send_byte(data[b] ^ TS_ESC);
          overhead++;
        } else send_byte(data[b]);
      }
      send_byte(TS_END);
      /* On RPI flush fails to wait until all bytes are transmitted
         here RPI forced to wait blocking using delayMicroseconds */
      #if defined(RPI) || defined(LINUX)
        if(_bd)
          PJON_DELAY_MICROSECONDS(
            ((1000000 / (_bd / 8)) + _flush_offset) * (overhead + length)
          );
      #endif
      PJON_SERIAL_FLUSH(serial);
      end_tx();
    };


    /* Pass the Serial port where you want to operate with */

    void set_serial(PJON_SERIAL_TYPE serial_port) {
      serial = serial_port;
    };

    void wait_RS485_pin_change() {
      if(_enable_RS485_txe_pin != TS_NOT_ASSIGNED)
        PJON_DELAY(_RS485_delay);
    };

    /* RS485 enable pins handling: */

    void start_tx() {
      if(_enable_RS485_txe_pin != TS_NOT_ASSIGNED) {
        PJON_IO_WRITE(_enable_RS485_txe_pin, HIGH);
        if(_enable_RS485_rxe_pin != TS_NOT_ASSIGNED)
          PJON_IO_WRITE(_enable_RS485_rxe_pin, HIGH);
        wait_RS485_pin_change();
      }
    };

    void end_tx() {
      if(_enable_RS485_txe_pin != TS_NOT_ASSIGNED) {
        wait_RS485_pin_change();
        PJON_IO_WRITE(_enable_RS485_txe_pin, LOW);
        if(_enable_RS485_rxe_pin != TS_NOT_ASSIGNED)
          PJON_IO_WRITE(_enable_RS485_rxe_pin, LOW);
      }
    };

  #if defined(RPI) || defined(LINUX)
    /* Pass baudrate to ThroughSerial
       (needed only for RPI flush hack): */

    void set_baud_rate(uint32_t baud) {
      _bd = baud;
    };

    /* Set flush timing offset in microseconds between expected and real
       serial byte transmission: */

    void set_flush_offset(uint16_t offset) {
      _flush_offset = offset;
    };
  #endif

    /* RS485 enable pins setters: */

    void set_enable_RS485_pin(uint8_t pin) {
      set_RS485_txe_pin(pin);
    };

    void set_RS485_rxe_pin(uint8_t pin) {
      _enable_RS485_rxe_pin = pin;
      PJON_IO_MODE(_enable_RS485_rxe_pin, OUTPUT);
    }

    void set_RS485_txe_pin(uint8_t pin) {
      _enable_RS485_txe_pin = pin;
      PJON_IO_MODE(_enable_RS485_txe_pin, OUTPUT);
    }

    void set_RS485_delay(uint32_t d) {
      _RS485_delay = d;
    };

  private:
  #if defined(RPI) || defined(LINUX)
    uint16_t _flush_offset = TS_FLUSH_OFFSET;
    uint32_t _bd;
  #endif
    uint8_t  _last_byte;
    uint32_t _last_reception_time;
    uint8_t  _enable_RS485_rxe_pin = TS_NOT_ASSIGNED;
    uint8_t  _enable_RS485_txe_pin = TS_NOT_ASSIGNED;
    uint32_t _RS485_delay = TS_RS485_DELAY;
};
