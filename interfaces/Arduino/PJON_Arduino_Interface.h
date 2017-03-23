
/* PJON Arduino Interface
   _____________________________________________________________________________

    Copyright 2017 Giovanni Blu Mitolo gioscarab@gmail.com

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
  #include <Arduino.h>
  #include "PJON_IO.h"

  /* Fallback to digitalWrite --------------------------------------------- */

  #if !defined(PJON_ANALOG_READ)
    #define PJON_ANALOG_READ analogRead
  #endif

  #if !defined(PJON_IO_WRITE)
    #define PJON_IO_WRITE digitalWrite
  #endif

  #if !defined(PJON_IO_READ)
    #define PJON_IO_READ digitalRead
  #endif

  #if !defined(PJON_IO_MODE)
    #define PJON_IO_MODE pinMode
  #endif

  #if !defined(PJON_IO_PULL_DOWN)
    #define PJON_IO_PULL_DOWN(P) \
      do { \
        digitalWrite(P, LOW); \
        pinMode(P, INPUT); \
      } while(0)
  #endif

  /* Random ----------------------------------------------------------------- */

  #ifndef PJON_RANDOM
    #define PJON_RANDOM random
  #endif

  #ifndef PJON_RANDOM_SEED
    #define PJON_RANDOM_SEED randomSeed
  #endif

  /* Serial ----------------------------------------------------------------- */

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

  /* Timing ----------------------------------------------------------------- */

  #ifndef PJON_DELAY_MICROSECONDS
    #define PJON_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef PJON_MICROS
    #define PJON_MICROS micros
  #endif
#endif
