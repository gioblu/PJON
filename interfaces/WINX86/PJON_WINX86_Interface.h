
/* PJON Windows x86 Interface
   _____________________________________________________________________________

    Copyright 2018 Zbigniew Zasieczny z.zasieczny@gmail.com

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

#if defined(_WIN32)

  #include <chrono>
  #include <thread>
  #include <sstream>
  #include "Serial/Serial.h"

  #define OUTPUT 1
  #define INPUT 0
  #define HIGH 1
  #define LOW 0
  #define INPUT_PULLUP 0x2
  #define LSBFIRST 1
  #define MSBFIRST 2

  void pinMode(char pin, char m) { ; };

  void digitalWrite(char, char) { ; };

  int analogRead(int) {
    return 0;
  };

  auto start_ts = std::chrono::high_resolution_clock::now();

  uint32_t micros() {
    auto elapsed_usec =
      std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start_ts
      ).count();

    if(elapsed_usec >= UINT32_MAX) {
      start_ts = std::chrono::high_resolution_clock::now();
      return 0;
    } else return (uint32_t) elapsed_usec;
  };


  void delayMicroseconds(uint32_t delay_value) {
    auto begin_ts = std::chrono::high_resolution_clock::now();
    while(true) {
      auto elapsed_usec =
        std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::high_resolution_clock::now() - begin_ts
        ).count();
      if(elapsed_usec >= delay_value) break;
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
  };

  #ifndef A0
    #define A0 0
  #endif

  /* Fallback to WINDOWS-specific functions -------------------------------- */

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
      PJON_IO_MODE(P, INPUT); \
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
    #define PJON_SERIAL_AVAILABLE(S) S->serialDataAvail()
  #endif

  #ifndef PJON_SERIAL_WRITE
    #define PJON_SERIAL_WRITE(S, C) S->putChar((char*)&C)
  #endif

  #ifndef PJON_SERIAL_READ
    #define PJON_SERIAL_READ(S) S->getChar()
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

  #ifndef PJON_MILLIS
    #define PJON_MILLIS() (PJON_MICROS() / 1000)
  #endif
#endif
