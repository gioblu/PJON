
/* PJON Linux Interface
   ___________________________________________________________________________

    Copyright 2017 Fred Larsen

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

#ifdef LINUX
  #include <stdint.h>
  #include <inttypes.h>
  #include <stdlib.h>
  #include <string.h>

  #include <chrono>
  #include <thread>
  #include <sstream>

  #define OUTPUT 1
  #define INPUT 0
  #define HIGH 1
  #define LOW 0
  #define INPUT_PULLUP 0x2
  #define LSBFIRST 1
  #define MSBFIRST 2

  typedef char byte;
//  typedef unsigned char uint8_t;
//  typedef unsigned word uint16_t;

  void pinMode(byte pin, byte m) { ; };

  void digitalWrite(byte, byte) { ; };

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
    } else return elapsed_usec;
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

  /* LINUX IO functions -------------------------------- */

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

  /* Timing ----------------------------------------------------------------- */

  #ifndef PJON_DELAY_MICROSECONDS
    #define PJON_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef PJON_MICROS
    #define PJON_MICROS micros
  #endif

  #ifndef millis
    #define millis() PJON_MICROS() / 1000
  #endif
#endif
