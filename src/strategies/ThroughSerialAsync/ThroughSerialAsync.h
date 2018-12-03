/* ThroughSerialAsync data link layer
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

   ThroughSerialAsync, based on ThroughSerial, developed by sticilface
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

// START  symbol 10010101 - 0x95 - �
#define TSA_START        149
// END    symbol 11101010 - 0xea - ê
#define TSA_END          234
// ESCAPE symbol 10111011 - 0xBB - »
#define TSA_ESC          187

// Used to signal communication failure
#define TSA_FAIL       65535
// Used for unused pin handling
#define TSA_NOT_ASSIGNED 255

#include "Timing.h"

enum TSA_state_t : uint8_t {
  TSA_RESET,
  TSA_WAITING_START,
  TSA_RECEIVING,
  TSA_WAITING_ESCAPE,
  TSA_WAITING_END,
  TSA_DONE
};

class ThroughSerialAsync {
  public:
    uint8_t buffer[PJON_PACKET_MAX_LENGTH] = {0};
    uint16_t position = 0;
    TSA_state_t state = TSA_WAITING_START;
    PJON_SERIAL_TYPE serial;

    /* Returns suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for (uint8_t d = 0; d < TSA_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };


    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t additional_randomness = 0) {
      PJON_DELAY(PJON_RANDOM(TSA_INITIAL_DELAY) + additional_randomness);
      _last_reception_time = 0;
      _last_byte = TSA_FAIL;
      return true;
    };


    /* Check if the channel is free for transmission: */

    bool can_start() {
      if(
        (state != TSA_WAITING_START) ||
        PJON_SERIAL_AVAILABLE(serial) ||
        ((uint32_t)(PJON_MICROS() - _last_reception_time) < TSA_TIME_IN)
      ) return false;
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TSA_COLLISION_DELAY));
      return true;
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return TSA_MAX_ATTEMPTS;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TSA_COLLISION_DELAY));
    };


    /* Receive byte response */

    uint16_t receive_response() {
      uint32_t time = PJON_MICROS();
      while((uint32_t)(PJON_MICROS() - time) < TSA_RESPONSE_TIME_OUT) {
        if(PJON_SERIAL_AVAILABLE(serial)) {
          int16_t read = PJON_SERIAL_READ(serial);
          _last_reception_time = PJON_MICROS();
          if(read >= 0) {
            _last_byte = (uint8_t)read;
            return _last_byte;
          }
        }
        #if defined(_WIN32)
          PJON_DELAY_MICROSECONDS(TSA_RESPONSE_TIME_OUT / 10);
        #endif
      }
      return TSA_FAIL;
    };


    /* Receive a string: */
	//force_attempt if serialbus is almost full (more than a thereshold) --> SerialEvent function for checking that!!
    uint16_t receive_string(uint8_t *string, uint16_t max_length,bool force_attempt = 0) {
      if( // Reception attempts are spaced by an interval
		!(force_attempt) &&
		(_last_call_time &&
		(uint32_t)(PJON_MICROS() - _last_call_time) < _read_interval)
      ) return TSA_FAIL;

      _last_call_time = PJON_MICROS();

      if( // If reception timeout is reached discard data
        (
          (state == TSA_RECEIVING) ||
          (state == TSA_WAITING_END) ||
          (state == TSA_WAITING_ESCAPE)
        ) &&
        ((uint32_t)(PJON_MICROS() - _last_reception_time) > TSA_BYTE_TIME_OUT)
      ) {
        state = TSA_WAITING_START;
        return TSA_FAIL;
      }

      switch(state) {
        case TSA_RESET: {
           state = TSA_WAITING_START;
           position = 0;
		   //break;
        }
		case TSA_WAITING_START: {
          while(PJON_SERIAL_AVAILABLE(serial)) {
            uint8_t value = PJON_SERIAL_READ(serial);
            _last_reception_time = PJON_MICROS();
            if(value == TSA_START) {
              state = TSA_RECEIVING;
              //position = 0;
              //return TSA_FAIL;
			  break;  //breaks the while --> next case: TSA_RECEIVING
            }
          }
          if (PJON_SERIAL_AVAILABLE(serial)==0) {
			break;    //only breaks if no data available at serial
		  }          
		  //break;
        }
        case TSA_RECEIVING: {
          while(PJON_SERIAL_AVAILABLE(serial)) {
            uint8_t value = PJON_SERIAL_READ(serial);
            _last_reception_time = PJON_MICROS();
            if(value == TSA_START) {
              state = TSA_WAITING_START;
              return TSA_FAIL;
            }
            if(value == TSA_ESC) {
              if(!PJON_SERIAL_AVAILABLE(serial)) {
                state = TSA_WAITING_ESCAPE;
                return TSA_FAIL;
              } else {
                value = PJON_SERIAL_READ(serial) ^ TSA_ESC;
                _last_reception_time = PJON_MICROS();
                if(
                  (value != TSA_START) &&
                  (value != TSA_ESC) &&
                  (value != TSA_END)
                ) {
                  state = TSA_WAITING_START;
                  return TSA_FAIL;
                }
                buffer[position++] = value;
                continue;
              }
            }

            if(max_length == 1) {  //???????
              state = TSA_WAITING_END;
              return TSA_FAIL;
            }

            if(position + 1 >= PJON_PACKET_MAX_LENGTH) {
              //state = TSA_WAITING_START;
			  state = TSA_RESET;  //overflow of buffer[PJON_PACKET_MAX_LENGTH]
              return TSA_FAIL;
            }

            if(value == TSA_END) {
              state = TSA_DONE;
              //return TSA_FAIL;
			  break; //breaks the while --> next case: TSA_DONE
            }

            buffer[position++] = value;
          }
          if (state == TSA_DONE) {
			break;
		  } else {
		    return TSA_FAIL;
		  }
        }

        case TSA_WAITING_ESCAPE: {
          if(PJON_SERIAL_AVAILABLE(serial)) {
            uint8_t value = PJON_SERIAL_READ(serial) ^ TSA_ESC;
            _last_reception_time = PJON_MICROS();
            if(
              (value != TSA_START) &&
              (value != TSA_ESC) &&
              (value != TSA_END)
            ) {
              state = TSA_WAITING_START;
              return TSA_FAIL;
            }
            buffer[position++] = value;
            state = TSA_RECEIVING;
            return TSA_FAIL;
          }
          break;
        }

        case TSA_WAITING_END: {
          if(PJON_SERIAL_AVAILABLE(serial)) {
            uint8_t value = PJON_SERIAL_READ(serial);
            _last_reception_time = PJON_MICROS();
            if(value == TSA_END) {
              state = TSA_DONE;
              return TSA_FAIL;
            } else {
              //state = TSA_WAITING_START;
			  state = TSA_RECEIVING; //serial data could be truncated (still more data to arrive)
              return TSA_FAIL;
            }
          }
          break;
        }

        case TSA_DONE: {
          memcpy(&string[0], &buffer[0], position);
          uint16_t len = position;
          state = TSA_WAITING_START; //it could be more messages or partial messages in buffer
          return len;
        }

      };
      return TSA_FAIL;
    };


    /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b) {
      PJON_SERIAL_WRITE(serial, b);
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


    /* Send a string: */

    void send_string(uint8_t *string, uint16_t length) {
      start_tx();
      uint16_t overhead = 2;
      // Add frame flag
      send_byte(TSA_START);
      for(uint16_t b = 0; b < length; b++) {
        // Byte-stuffing
        if(
          (string[b] == TSA_START) ||
          (string[b] == TSA_ESC) ||
          (string[b] == TSA_END)
        ) {
          send_byte(TSA_ESC);
          send_byte(string[b] ^ TSA_ESC);
          overhead++;
        } else send_byte(string[b]);
      }
      send_byte(TSA_END);
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


    /* RS485 enable pins handling: */

    void start_tx() {
      if(_enable_RS485_txe_pin != TSA_NOT_ASSIGNED) {
        PJON_IO_WRITE(_enable_RS485_txe_pin, HIGH);
        if(_enable_RS485_rxe_pin != TSA_NOT_ASSIGNED)
          PJON_IO_WRITE(_enable_RS485_rxe_pin, HIGH);
        wait_RS485_pin_change();
      }
    };

    void end_tx() {
      if(_enable_RS485_txe_pin != TSA_NOT_ASSIGNED) {
        wait_RS485_pin_change();
        PJON_IO_WRITE(_enable_RS485_txe_pin, LOW);
        if(_enable_RS485_rxe_pin != TSA_NOT_ASSIGNED)
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
       (TSA_READ_INTERVAL or 100 microseconds by default) to allow the buffer
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
      if(_enable_RS485_txe_pin != TSA_NOT_ASSIGNED)
        PJON_DELAY(_RS485_delay);
    };

  private:
  #if defined(RPI) || defined(LINUX)
    uint16_t _flush_offset = TSA_FLUSH_OFFSET;
    uint32_t _bd;
  #endif
    uint8_t  _last_byte;
    uint32_t _last_reception_time = 0;
    uint32_t _last_call_time = 0;
    uint8_t  _enable_RS485_rxe_pin = TSA_NOT_ASSIGNED;
    uint8_t  _enable_RS485_txe_pin = TSA_NOT_ASSIGNED;
    uint32_t _RS485_delay = TSA_RS485_DELAY;
    uint32_t _read_interval = TSA_READ_INTERVAL;
};
