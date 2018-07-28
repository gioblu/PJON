
/* ThroughSerial digital communication data link layer
   used as a Strategy by the PJON framework (included in version v4.1)
   Compliant with TSDL (Tardy Serial Data Link) specification v2.0

   Contributors:
    - Fred Larsen, Development, testing and debugging
    - Zbigniew Zasieczny, collision avoidance multi-drop RS485 (latency)
      and SoftwareSerial compatibility
    - Franketto (Arduino forum user) RS485 TX enable pin compatibility
    - Endre Karlson separate RS485 enable pins handling, flush timing hack
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
#define TAS_START 149
// END   frame symbol 11101010 - 0xea - ê
#define TAS_END   234
// ESCAPE      symbol 10111011 - 0xBB - »
#define TAS_ESC   187

// Used to signal communication failure
#define TAS_FAIL                     65535

// Used for pin handling
#define TAS_NOT_ASSIGNED               255

#include "Timing.h"

//#define DEBUG_ Serial

#if defined(DEBUG_)
#define DEBUGF(_1, ...) { DEBUG_.printf_P( PSTR( "[%s, line %i] " _1), __func__, __LINE__ ,##__VA_ARGS__); }
#pragma message("DEBUG enabled for ThroughAsyncSerial")
#else
#define DEBUGF(...) {} 
#endif

enum TAS_state_t : uint8_t {
  TAS_WAITING,
  TAS_RECEIVING,
  TAS_WAITING_ESCAPE, 
  TAS_WAITING_END,
  TAS_DONE
}; 

class ThroughAsyncSerial {
  public:
    uint8_t buffer[PJON_PACKET_MAX_LENGTH]{0}; 
    uint16_t position{0}; 
    TAS_state_t state{TAS_WAITING}; 
    #if defined(ARDUINO)
      Stream *serial = NULL;
    #elif defined(RPI)
      int16_t serial = 0;
    #elif defined(_WIN32)
      Serial *serial = NULL;
    #endif
    /* Returns suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < TAS_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };

    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t additional_randomness = 0) {
      PJON_DELAY_MICROSECONDS(
        PJON_RANDOM(TAS_INITIAL_DELAY) + additional_randomness
      );
      //_last_byte = receive_byte();
      return true;
    };


    /* Check if the channel is free for transmission: */

    bool can_start() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TAS_COLLISION_DELAY));
      if(PJON_SERIAL_AVAILABLE(serial)) return false;
      if((uint32_t)(PJON_MICROS() - _last_reception_time) < TAS_TIME_IN)
        return false;
      return true;
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return TAS_MAX_ATTEMPTS;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TAS_COLLISION_DELAY));
    };


    /* Receive byte response */

    uint16_t receive_response() {
      uint32_t time = PJON_MICROS();
      while((uint32_t)(PJON_MICROS() - time) < TAS_RESPONSE_TIME_OUT) {
        if(PJON_SERIAL_AVAILABLE(serial)) {
          _last_reception_time = PJON_MICROS();
          int16_t read = PJON_SERIAL_READ(serial);
          if(read >= 0) {
            _last_byte = (uint8_t)read;
            return _last_byte;
          }
        }
        #if defined(_WIN32)
          PJON_DELAY_MICROSECONDS(TAS_RESPONSE_TIME_OUT / 10);
        #endif
      }
      return TAS_FAIL;

    };

    /* Receive a string: */

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {

    if (_lastCallTime && PJON_MICROS() - _lastCallTime < _timeoutus) {
      return TAS_FAIL;
    }

    _lastCallTime = PJON_MICROS(); //  allows time for uart to receive data...

    if (_last_reception_time && ( PJON_MICROS() - _last_reception_time > 1000000)) {
        DEBUGF("Timeout _last_reception_time = %u\n", _last_reception_time);
        _last_reception_time = 0; 
        state = TAS_WAITING;           
    }

      switch (state) {
        case TAS_WAITING: {
            _last_reception_time = 0; 
            uint8_t value{TAS_NOT_ASSIGNED}; 
            while(PJON_SERIAL_AVAILABLE(serial)) {
              uint8_t value = PJON_SERIAL_READ(serial) ;  // see if it is a start frame... 
              //DEBUGF("avail = %u, next = %c [%u]\n", PJON_SERIAL_AVAILABLE(serial), value, value);
              if (value == TAS_START) {
                //_last_reception_time = PJON_MICROS(); 
                //DEBUGF("Timeout _last_reception_time set To %u\n", _last_reception_time);
                state = TAS_RECEIVING; 
                position = 0; 
                return TAS_FAIL; 
              }
            }; 

          break; 
        }

        case TAS_RECEIVING: {
            uint16_t result;
            //DEBUGF("Rec\n"); 



            int count = 0; 
            //DEBUGF("Available = %u\n", PJON_SERIAL_AVAILABLE(serial));

            while(PJON_SERIAL_AVAILABLE(serial)) {
              uint8_t value = PJON_SERIAL_READ(serial) ; 

              if (value == TAS_FAIL) { state = TAS_WAITING; DEBUGF("TAS_FAIL\n");  return TAS_FAIL; }   //  byte consumed 
               // Unescaped START byte stuffing violation
              if(value == TAS_START) { state = TAS_WAITING; DEBUGF("START_FAIL\n"); return TAS_FAIL; }   //  byte consumed        
              
              if (value == TAS_ESC) {
                DEBUGF("ESC Found: "); 
                if (!PJON_SERIAL_AVAILABLE(serial)) {
                  state = TAS_WAITING_ESCAPE; 
                  DEBUGF("waiting for ESCP\n"); 
                  return TAS_FAIL;                  
                } else {

                  value = PJON_SERIAL_READ(serial);
                  value ^= TAS_ESC; 
                   if((value != TAS_START) && (value != TAS_ESC) && (value != TAS_END)) {
                     state = TAS_WAITING; //  real fail 
                     DEBUGF("BAIL: target = %c (%2X), %u bytes written\n", value, value, count); 
                     return TAS_FAIL; 
                   }
                  count++; 
                  buffer[position++] = value;   
                  DEBUGF("ESC written [%u] = %c (%2X)\n", position, value, value); 
                  _last_reception_time = PJON_MICROS();    
                  //DEBUGF("Timeout _last_reception_time set To %u\n", _last_reception_time);
                  continue; 
                }
              }

              if (max_length == 1) {
                DEBUGF("MaxLen=1 goto END\n"); 
                state = TAS_WAITING_END; 
                return TAS_FAIL; 
              }

              if (position + 1 >= PJON_PACKET_MAX_LENGTH ) {
                  DEBUGF("Over Len: Fail\n"); 
                 state = TAS_WAITING; //  real fail 
                 return TAS_FAIL;                 
              }

              if (value == TAS_END) {
                state = TAS_DONE; 
                return TAS_FAIL; 
              }
              //DEBUGF("BufferWritten [%u] = %c (%2X)\n", position, value, value); 
              count++; 
              buffer[position++] = value; 
              _last_reception_time = PJON_MICROS(); 
              //DEBUGF("Timeout _last_reception_time set To %u\n", _last_reception_time);


            }

            DEBUGF("%u bytes written\n", count); 
            return TAS_FAIL; 
          
          break;
        }

        case TAS_WAITING_ESCAPE: 
        {
          DEBUGF("esp\n"); 
         if( PJON_SERIAL_AVAILABLE(serial) ) {
          uint8_t value = PJON_SERIAL_READ(serial); 
          value ^= TAS_ESC; 
          buffer[position++] = value; 
          state = TAS_RECEIVING; 
          return TAS_FAIL;
         }
          break; 
        }

        case TAS_WAITING_END: {
           DEBUGF("end\n"); 
          if( PJON_SERIAL_AVAILABLE(serial) ) {
            uint8_t value = PJON_SERIAL_READ(serial); 
            if (value == TAS_END) {
              state = TAS_DONE; 
              return TAS_FAIL; 
            } else {
              state = TAS_WAITING; // real fail; 
              return TAS_FAIL; 
            }
          }
          break; 
        }

        case TAS_DONE: {
          
          memcpy(&string[0], &buffer[0], position); 
          uint16_t len = position;
          position = 0;
          state = TAS_WAITING; 
          _last_reception_time = 0;

          return len; 
        }

      }; 

      return TAS_FAIL;

    };

    /*  sets the timeout for attempting to read data from serial to allow buffer to fill.  
        different from back_off that is to do with send.   */
    uint32_t get_timeout() { return _timeoutus; }
    void set_timeout(uint32_t t) { _timeoutus = t ; }


    /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b) {
      PJON_SERIAL_WRITE(serial, b);
    };


    /* Send byte response to the packet's transmitter */

    void send_response(uint8_t response) {
      start_tx();
      send_byte(response);
      PJON_SERIAL_FLUSH(serial);
      end_tx();
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint8_t length) {
      start_tx();
      uint16_t overhead = 2;
      // Add frame flag
      send_byte(TAS_START);
      for(uint8_t b = 0; b < length; b++) {
        // Byte-stuffing
        if(
          (string[b] == TAS_START) ||
          (string[b] == TAS_ESC) ||
          (string[b] == TAS_END)
        ) {
          send_byte(TAS_ESC);
          send_byte(string[b] ^ TAS_ESC);
          overhead++;
        } else send_byte(string[b]);
      }
      send_byte(TAS_END);
      /* On RPI flush fails to wait until all bytes are transmitted
         here RPI forced to wait blocking using delayMicroseconds */
      #if defined(RPI)
        if(_bd)
          PJON_DELAY_MICROSECONDS(
            ((1000000 / (_bd / 8)) + _flush_offset) * (overhead + length)
          );
      #endif
      PJON_SERIAL_FLUSH(serial);
      end_tx();
      //Serial.println(); 
    };


    /* Pass the Serial port where you want to operate with */

  #if defined(ARDUINO)
    void set_serial(Stream *serial_port) {
  #elif defined(RPI)
    void set_serial(int16_t serial_port) {
  #elif defined(_WIN32)
    void set_serial(Serial *serial_port) {
  #endif
      serial = serial_port;
    };


    /* RS485 enable pins handling: */

    void start_tx() {
      if(_enable_RS485_txe_pin != TAS_NOT_ASSIGNED) {
        PJON_IO_WRITE(_enable_RS485_txe_pin, HIGH);
        if(_enable_RS485_rxe_pin != TAS_NOT_ASSIGNED)
          PJON_IO_WRITE(_enable_RS485_rxe_pin, HIGH);
      }
    };

    void end_tx() {
      if(_enable_RS485_txe_pin != TAS_NOT_ASSIGNED) {
        PJON_IO_WRITE(_enable_RS485_txe_pin, LOW);
        if(_enable_RS485_rxe_pin != TAS_NOT_ASSIGNED)
          PJON_IO_WRITE(_enable_RS485_rxe_pin, LOW);
      }
    };

  #if defined(RPI)
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

  private:
  #if defined(RPI)
    uint16_t _flush_offset = TAS_FLUSH_OFFSET;
    uint32_t _bd;
  #endif
    uint8_t  _last_byte;
    uint32_t _last_reception_time = 0;
    uint8_t  _enable_RS485_rxe_pin = TAS_NOT_ASSIGNED;
    uint8_t  _enable_RS485_txe_pin = TAS_NOT_ASSIGNED;
    uint32_t _timeoutus = 100; 
    uint32_t _lastCallTime = 0; 




};
