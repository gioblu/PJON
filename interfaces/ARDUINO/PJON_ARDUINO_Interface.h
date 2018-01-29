
/* PJON Arduino Interface
   ___________________________________________________________________________

    Copyright 2018 Giovanni Blu Mitolo gioscarab@gmail.com

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

#if defined(ARDUINO)
  #ifdef ARDUINO_ESP8266_WEMOS_D1MINI
    #define PJON_ESP // WeMos mini and D1 R2
  #elif ARDUINO_ESP8266_ESP01
    #define PJON_ESP // Generic ESP's use for 01's
  #elif ARDUINO_ESP8266_NODEMCU
    #define PJON_ESP // Wio Link and NodeMCU 1.0 (also 0.9), use for ESP12
  #endif

  #include <Arduino.h>
  #include "PJON_IO.h"

  /* Fallback to digitalWrite --------------------------------------------- */

  #ifndef PJON_ANALOG_READ
    #define PJON_ANALOG_READ analogRead
  #endif

  #ifndef PJON_IO_WRITE
    #define PJON_IO_WRITE digitalWrite
  #endif

  #ifndef PJON_IO_READ
    #define PJON_IO_READ digitalRead
  #endif

  #ifndef PJON_IO_MODE
    #define PJON_IO_MODE pinMode
  #endif

  #ifndef PJON_IO_PULL_DOWN
    #define PJON_IO_PULL_DOWN(P) \
      do { \
        digitalWrite(P, LOW); \
        pinMode(P, INPUT); \
      } while(0)
  #endif

  /* Random --------------------------------------------------------------- */

  #ifndef PJON_RANDOM
    #define PJON_RANDOM random
  #endif

  #ifndef PJON_RANDOM_SEED
    #define PJON_RANDOM_SEED randomSeed
  #endif

  /* Serial --------------------------------------------------------------- */

  #ifndef PJON_SERIAL_AVAILABLE
    #define PJON_SERIAL_AVAILABLE(S) S->available()
  #endif

  #ifndef PJON_SERIAL_WRITE
    #define PJON_SERIAL_WRITE(S, C) S->write(C)
  #endif

  #ifndef PJON_SERIAL_READ
    #define PJON_SERIAL_READ(S) S->read()
  #endif

  #ifndef PJON_SERIAL_FLUSH
    #define PJON_SERIAL_FLUSH(S) S->flush()
  #endif

  /* Timing --------------------------------------------------------------- */

  #ifndef PJON_DELAY_MICROSECONDS
    #define PJON_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef PJON_MICROS
    #define PJON_MICROS micros
  #endif

  #ifndef PJON_MILLIS
    #define PJON_MILLIS millis
  #endif

  /* Set ADC prescale functions ------------------------------------------- */

  #ifndef cbi
    #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
  #endif

  #ifndef sbi
    #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
  #endif

  /* Sockets -------------------------------------------------------------- */

  #define HAS_ETHERNETUDP

  /* Byte order translation functions ------------------------------------- */

  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

    #ifndef htons // Host to network short
      #define htons(x) ( ((uint16_t)(x) << 8 & 0xFF00) | \
                         ((uint16_t)(x) >> 8 & 0x00FF) )
    #endif

    #ifndef ntohs // Network to host short
      #define ntohs(x) htons(x)
    #endif

    #ifndef htonl // Host to network long
      #define htonl(x) ( ((uint32_t)(x) << 24 & 0xFF000000UL) | \
                         ((uint32_t)(x) << 8  & 0x00FF0000UL) | \
                         ((uint32_t)(x) >> 8  & 0x0000FF00UL) | \
                         ((uint32_t)(x) >> 24 & 0x000000FFUL) )
    #endif

    #ifndef ntohl // Network to host long
      #define ntohl(x) htonl(x)
    #endif
  #endif

  #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define htons(x) (x)
    #define htonl(x) (x)
    #define ntohs(x) (x)
    #define ntohl(x) (x)
  #endif

#endif
