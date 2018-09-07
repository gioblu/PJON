
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

  auto start_ts = std::chrono::high_resolution_clock::now();
  auto start_ts_ms = std::chrono::high_resolution_clock::now();

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

  uint32_t millis() {
    return (uint32_t)
      std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start_ts_ms
      ).count();
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

  void delay(uint32_t delay_value_ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_value_ms));
  };

  /* Open serial port ----------------------------------------------------- */

  int serialOpen(const char *device, const int baud) {
    speed_t bd;
    int fd;
    int state;
    struct termios config;

    switch(baud) {
      case     200:	bd =     B200; break;
      case     300:	bd =     B300; break;
      case     600:	bd =     B600; break;
      case    1200:	bd =    B1200; break;
      case    1800:	bd =    B1800; break;
      case    2400:	bd =    B2400; break;
      case    4800:	bd =    B4800; break;
      case    9600:	bd =    B9600; break;
      case   19200:	bd =   B19200; break;
      case   38400:	bd =   B38400; break;
      case   57600:	bd =   B57600; break;
      case  115200:	bd =  B115200; break;
      case  230400:	bd =  B230400; break;
      #if !defined(__APPLE__) && !defined(__FreeBSD__)
        case  460800:	bd =  B460800; break;
        case  500000:	bd =  B500000; break;
        case  576000:	bd =  B576000; break;
        case  921600:	bd =  B921600; break;
        case 1000000:	bd = B1000000; break;
        case 1152000:	bd = B1152000; break;
        case 1500000:	bd = B1500000; break;
        case 2000000:	bd = B2000000; break;
        case 2500000:	bd = B2500000; break;
        case 3000000:	bd = B3000000; break;
        case 3500000:	bd = B3500000; break;
        case 4000000:	bd = B4000000; break;
      #endif
      default: return -2;
    }

    if((fd = open(device, O_NDELAY | O_NOCTTY | O_NONBLOCK | O_RDWR)) == -1)
      return -1;

    fcntl(fd, F_SETFL, O_RDWR);
    tcgetattr(fd, &config);
    cfmakeraw(&config);
    cfsetispeed(&config, bd);
    cfsetospeed(&config, bd);

    config.c_cflag |= (CLOCAL | CREAD);
    config.c_cflag &= ~(CSTOPB | CSIZE | PARENB);
    config.c_cflag |= CS8;
    config.c_lflag &= ~(ECHO | ECHOE | ICANON | ISIG);
    config.c_oflag &= ~OPOST;
    config.c_cc [VMIN] = 0;
    config.c_cc [VTIME] = 50; // 5 seconds reception timeout

    tcsetattr(fd, TCSANOW, &config);
    ioctl(fd, TIOCMGET, &state);
    state |= (TIOCM_DTR | TIOCM_RTS);
    ioctl(fd, TIOCMSET, &state);

    usleep(10000);	// Sleep for 10 milliseconds
    return fd;
  };

  /* Returns the number of bytes of data avalable to be read in the buffer */

  int serialDataAvailable(const int fd) {
    int result;
    ioctl(fd, FIONREAD, &result);
    return result;
  };

/* Reads a character from the serial buffer ------------------------------- */

  int serialGetCharacter(const int fd) {
    uint8_t result;
    if(read(fd, &result, 1) != 1) return -1;
    return ((int)result) & 0xFF;
  };

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

  #ifndef TS_FLUSH_OFFSET
    #define TS_FLUSH_OFFSET 152
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
