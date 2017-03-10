
/* PJON RaspeberryPi Interface
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

#if defined(RPI)
  #include <wiringPi.h>
  #include <wiringSerial.h>
  #include <inttypes.h>
  #include <stdio.h>
  #include <stdint.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
  #include <errno.h>

  #ifndef A0
    #define A0 0
  #endif

  /* Fallback to WiringPi core functions ------------------------------------ */

  #if !defined(PJON_ANALOG_READ)
    #define PJON_ANALOG_READ(P) analogRead(P)
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
    #define PJON_IO_PULL_DOWN(P) { \
      PJON_IO_MODE(p, INPUT); \
      pullUpDnControl(P, PUD_DOWN); \
    }
  #endif

  /* Random ----------------------------------------------------------------- */

  #ifndef PJON_RANDOM
    #define PJON_RANDOM(randMax) (int)((1.0 + randMax) * rand() / ( RAND_MAX + 1.0 ) )
    /* Scale rand()'s return value against RAND_MAX using doubles instead of
       a pure modulus to have a more distributed result */
  #endif

  #ifndef PJON_RANDOM_SEED
    #define PJON_RANDOM_SEED srand
  #endif

  /* Serial ----------------------------------------------------------------- */

  #ifndef PJON_SERIAL_AVAILABLE
    #define PJON_SERIAL_AVAILABLE(S) serialDataAvail(S)
  #endif

  #ifndef PJON_SERIAL_WRITE
    #define PJON_SERIAL_WRITE(S, C) serialPutchar(S, C)
  #endif

  #ifndef PJON_SERIAL_READ
    #define PJON_SERIAL_READ(S) serialGetchar(S)
  #endif

  #ifndef PJON_SERIAL_FLUSH
    #define PJON_SERIAL_FLUSH(S) serialFlush(S)
  #endif

  /* Timing ----------------------------------------------------------------- */

  #ifndef PJON_DELAY_MICROSECONDS
    #define PJON_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef PJON_MICROS
    #define PJON_MICROS micros
  #endif
#endif
