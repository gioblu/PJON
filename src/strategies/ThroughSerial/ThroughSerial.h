
/* ThroughSerial data link layer
   used as a Strategy by the PJON framework (included in version v11.2)
   Compliant with TSDL (Tardy Serial Data Link) specification v2.0

   Contributors:
    - sticilface async reception development
    - Fred Larsen, Development, testing and debugging
    - Zbigniew Zasieczny, collision avoidance multi-drop RS485 (latency)
      and SoftwareSerial compatibility
    - Franketto (Arduino forum user) RS485 TX enable pin compatibility
    - Endre Karlson separate RS485 enable pins handling, flush timing hack
   ___________________________________________________________________________

   ThroughSerial, based on ThroughSerial, developed by sticilface
   copyright 2018 by Giovanni Blu Mitolo All rights reserved

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

// START  symbol 10010101 - 0x95 - 
#define TS_START        149
// END    symbol 11101010 - 0xea - ê
#define TS_END          234
// ESCAPE symbol 10111011 - 0xBB - »
#define TS_ESC          187

// Used to signal communication failure
#define TS_FAIL       65535
// Used for unused pin handling
#define TS_NOT_ASSIGNED 255

#include "Timing.h"

enum TS_state_t : uint8_t {
  TS_WAITING,
  TS_RECEIVING,
  TS_WAITING_ESCAPE,
  TS_WAITING_END,
  TS_DONE
};

// Recommended receive time for this strategy, in microseconds
#ifndef TS_RECEIVE_TIME
  #define TS_RECEIVE_TIME 0
#endif

class ThroughSerial {
  public:
    uint8_t buffer[PJON_PACKET_MAX_LENGTH] = {0};
    uint16_t position = 0;
    TS_state_t state = TS_WAITING;
    PJON_SERIAL_TYPE serial;

    /* Returns suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < TS_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result + PJON_RANDOM(TS_COLLISION_DELAY);
    };


    /* Begin method, to be called on initialization:
       (returns always true) */

    bool begin(uint8_t did = 0) {
      PJON_DELAY(PJON_RANDOM(TS_INITIAL_DELAY) + did);
      _last_reception_time = PJON_MICROS();
      return true;
    };


    /* Check if the channel is free for transmission: */

    bool can_start() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TS_COLLISION_DELAY));
      if(
        (state != TS_WAITING) ||
        PJON_SERIAL_AVAILABLE(serial) ||
        ((uint32_t)(PJON_MICROS() - _last_reception_time) < TS_TIME_IN)
      ) return false;
      return true;
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return TS_MAX_ATTEMPTS;
    };


    /* Returns the recommended receive time for this strategy: */

    static uint16_t get_receive_time() {
      return TS_RECEIVE_TIME;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TS_COLLISION_DELAY));
    };


    /* Receive Byte */

    int16_t receive_byte() {
      int16_t value = PJON_SERIAL_READ(serial);
      if(value == -1) return -1;
      _last_reception_time = PJON_MICROS();
      return value;
    };


    /* It returns the state of the previous transmission: */

    uint16_t receive_response() {
      if(_fail) return TS_FAIL;
      uint32_t time = PJON_MICROS();
      uint8_t i = 0;
      while((uint32_t)(PJON_MICROS() - time) < TS_RESPONSE_TIME_OUT) {
        if(PJON_SERIAL_AVAILABLE(serial)) {
          int16_t read = PJON_SERIAL_READ(serial);
          _last_reception_time = PJON_MICROS();
          if(read >= 0) {
            if(_response[i++] != read) return TS_FAIL;
            if(i == TS_RESPONSE_LENGTH) return PJON_ACK;
          }
        }
        #if defined(_WIN32)
          PJON_DELAY_MICROSECONDS(TS_RESPONSE_TIME_OUT / 10);
        #endif
      }
      return TS_FAIL;
    };


    /* Receive a string: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      if( // Reception attempts are spaced by an interval
        _last_call_time &&
        (uint32_t)(PJON_MICROS() - _last_call_time) < _read_interval
      ) return TS_FAIL;

      _last_call_time = PJON_MICROS();

      if( // If reception timeout is reached discard data
        (
          (state == TS_RECEIVING) ||
          (state == TS_WAITING_END) ||
          (state == TS_WAITING_ESCAPE)
        ) &&
        ((uint32_t)(PJON_MICROS() - _last_reception_time) > TS_BYTE_TIME_OUT)
      ) {
        state = TS_WAITING;
        return TS_FAIL;
      }

      switch(state) {
        case TS_WAITING: {
          while(PJON_SERIAL_AVAILABLE(serial)) {
            int16_t value = receive_byte();
            if(value == -1) return TS_FAIL;
            if(value == TS_START) {
              state = TS_RECEIVING;
              position = 0;
              return TS_FAIL;
            }
          };
          break;
        }
        case TS_RECEIVING: {
          while(PJON_SERIAL_AVAILABLE(serial)) {
            int16_t value = receive_byte();
            if(value == -1) return TS_FAIL;
            if(value == TS_START) {
              state = TS_WAITING;
              return TS_FAIL;
            }
            if(value == TS_ESC) {
              if(!PJON_SERIAL_AVAILABLE(serial)) {
                state = TS_WAITING_ESCAPE;
                return TS_FAIL;
              } else {
                value = receive_byte();
                if(value == -1) return TS_FAIL;
                value = value ^ TS_ESC;
                if(
                  (value != TS_START) &&
                  (value != TS_ESC) &&
                  (value != TS_END)
                ) {
                  state = TS_WAITING;
                  return TS_FAIL;
                }
                buffer[position++] = (uint8_t)value;
                continue;
              }
            }

            if(max_length == 1) {
              state = TS_WAITING_END;
              return TS_FAIL;
            }

            if(position + 1 >= PJON_PACKET_MAX_LENGTH) {
              state = TS_WAITING;
              return TS_FAIL;
            }

            if(value == TS_END) {
              state = TS_DONE;
              return TS_FAIL;
            }

            buffer[position++] = (uint8_t)value;
          }
          return TS_FAIL;
        }

        case TS_WAITING_ESCAPE: {
          if(PJON_SERIAL_AVAILABLE(serial)) {
            int16_t value = receive_byte();
            if(value == -1) return TS_FAIL;
            value = value ^ TS_ESC;
            if(
              (value != TS_START) &&
              (value != TS_ESC) &&
              (value != TS_END)
            ) {
              state = TS_WAITING;
              return TS_FAIL;
            }
            buffer[position++] = (uint8_t)value;
            state = TS_RECEIVING;
            return TS_FAIL;
          }
          break;
        }

        case TS_WAITING_END: {
          if(PJON_SERIAL_AVAILABLE(serial)) {
            int16_t value = receive_byte();
            if(value == -1) return TS_FAIL;
            if(value == TS_END) {
              state = TS_DONE;
              return TS_FAIL;
            } else {
              state = TS_WAITING;
              return TS_FAIL;
            }
          }
          break;
        }

        case TS_DONE: {
          memcpy(&data[0], &buffer[0], position);
          prepare_response(buffer, position);
          state = TS_WAITING;
          return position;
        }

      };
      return TS_FAIL;
    };


    /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b) {
      uint32_t time = PJON_MICROS();
      int16_t result = 0;
      while(
        ((result = PJON_SERIAL_WRITE(serial, b)) != 1) &&
        ((uint32_t)(PJON_MICROS() - time) < TS_BYTE_TIME_OUT)
      );
      if(result != 1) _fail = true;
    };


    /* The last 5 bytes of the frame are used as a unique identifier within
       the response. PJON has CRC8 or CRC32 at the end of the packet, encoding
       a CRC (that is a good hashing algorithm) and using 40 bits looks enough
       to provide a relatively safe response that should be nearly flawless
       (yield few false positives per millennia). */

    void prepare_response(const uint8_t *buffer, uint16_t position) {
      uint8_t raw = 0;
      for(int8_t i = 0; i < TS_RESPONSE_LENGTH; i++) {
        raw = buffer[(position - ((TS_RESPONSE_LENGTH - 1) - i)) - 1];
        _response[i] = (
          (raw == TS_START) || (raw == TS_ESC) || (raw == TS_END)
        ) ? (raw - 1) : raw; // Avoid encoding symbols
      }
    };


    /* Send byte response to the packet's transmitter */

    void send_response(uint8_t response) {
      if(response == PJON_ACK) {
        start_tx();
        wait_RS485_pin_change();
        for(uint8_t i = 0; i < TS_RESPONSE_LENGTH; i++)
          send_byte(_response[i]);
        PJON_SERIAL_FLUSH(serial);
        wait_RS485_pin_change();
        end_tx();
      }
    };


    /* Send a string: */

    void send_frame(uint8_t *data, uint16_t length) {
      _fail = false;
      start_tx();
      uint16_t overhead = 2;
      // Add frame flag
      send_byte(TS_START);
      for(uint16_t b = 0; b < length; b++) {
        if(_fail) return;
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
      // Prepare expected response for the receive_response call
      prepare_response(data, length);
    };


    /* Pass the Serial port where you want to operate with */

    void set_serial(PJON_SERIAL_TYPE serial_port) {
      serial = serial_port;
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

    /* Sets the interval between each read attempt from serial
       (TS_READ_INTERVAL or 100 microseconds by default) to allow the buffer
       to fill and to reduce the computation time consumed while polling for
       incoming data.  */

    uint32_t get_read_interval() {
      return _read_interval;
    };

    void set_read_interval(uint32_t t) {
      _read_interval = t;
    };

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

    void wait_RS485_pin_change() {
      if(_enable_RS485_txe_pin != TS_NOT_ASSIGNED)
        PJON_DELAY(_RS485_delay);
    };

  private:
  #if defined(RPI) || defined(LINUX)
    uint16_t _flush_offset = TS_FLUSH_OFFSET;
    uint32_t _bd;
  #endif
    bool     _fail = false;
    uint8_t  _response[TS_RESPONSE_LENGTH];
    uint32_t _last_reception_time = 0;
    uint32_t _last_call_time = 0;
    uint8_t  _enable_RS485_rxe_pin = TS_NOT_ASSIGNED;
    uint8_t  _enable_RS485_txe_pin = TS_NOT_ASSIGNED;
    uint32_t _RS485_delay = TS_RS485_DELAY;
    uint32_t _read_interval = TS_READ_INTERVAL;
};
