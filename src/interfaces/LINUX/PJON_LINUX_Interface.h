
/* PJON Linux Interface

   ___________________________________________________________________________

    Copyright 2018 Fred Larsen

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
  #include <stdio.h>
  #include <stdint.h>
  #include <inttypes.h>
  #include <stdlib.h>
  #include <stdarg.h>
  #include <string.h>
  #include <termios.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/ioctl.h>
  #include <sys/types.h>
  #include <sys/stat.h>

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


  uint32_t micros();

  uint32_t millis();

  void delayMicroseconds(uint32_t delay_value);

  void delay(uint32_t delay_value_ms);

  /* Open serial port ----------------------------------------------------- */

  int serialOpen(const char *device, const int baud);

  /* Returns the number of bytes of data available to be read in the buffer */

  int serialDataAvailable(const int fd);

/* Reads a character from the serial buffer ------------------------------- */

  int serialGetCharacter(const int fd);

  #ifndef PJON_LINUX_SEPARATE_DEFINITION
    #include "PJON_LINUX_Interface.cpp"
  #endif

  /* Generic constants ---------------------------------------------------- */

  #ifndef A0
    #define A0 0
  #endif

  #ifndef LED_BUILTIN
    #define LED_BUILTIN -1
  #endif

  /* LINUX IO system calls ------------------------------------------------ */

  #if !defined(PJON_ANALOG_READ)
    #define PJON_ANALOG_READ(P) 0
  #endif

  #if !defined(PJON_IO_WRITE)
    #define PJON_IO_WRITE(P, V)
  #endif

  #if !defined(PJON_IO_READ)
    #define PJON_IO_READ(P) 0
  #endif

  #if !defined(PJON_IO_MODE)
    #define PJON_IO_MODE(P, V)
  #endif

  #if !defined(PJON_IO_PULL_DOWN)
    #define PJON_IO_PULL_DOWN(P)
  #endif

  /* Random --------------------------------------------------------------- */

  #ifndef PJON_RANDOM
    #define PJON_RANDOM(randMax) (int)((1.0 + randMax) * rand() / ( RAND_MAX + 1.0 ) )
    /* Scale rand()'s return value against RAND_MAX using doubles instead of
       a pure modulus to have a more distributed result */
  #endif

  #ifndef PJON_RANDOM_SEED
    #define PJON_RANDOM_SEED srand
  #endif

  /* Serial --------------------------------------------------------------- */

  #ifndef PJON_SERIAL_TYPE
    #define PJON_SERIAL_TYPE int16_t
  #endif

  #ifndef PJON_SERIAL_AVAILABLE
    #define PJON_SERIAL_AVAILABLE(S) serialDataAvailable(S)
  #endif

  #ifndef PJON_SERIAL_WRITE
    #define PJON_SERIAL_WRITE(S, C) write(S, &C, 1)
  #endif

  #ifndef PJON_SERIAL_READ
    #define PJON_SERIAL_READ(S) serialGetCharacter(S)
  #endif

  #ifndef PJON_SERIAL_FLUSH
    #define PJON_SERIAL_FLUSH(S) tcflush(S, TCIOFLUSH)
  #endif

  /* Timing --------------------------------------------------------------- */

  #ifndef PJON_DELAY
    #define PJON_DELAY delay
  #endif

  #ifndef PJON_DELAY_MICROSECONDS
    #define PJON_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef PJON_MICROS
    #define PJON_MICROS micros
  #endif

  #ifndef PJON_MILLIS
    #define PJON_MILLIS millis
  #endif
#endif
