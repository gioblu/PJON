
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
    #define PJON_SERIAL_AVAILABLE Serial.available
  #endif

  #ifndef PJON_SERIAL_WRITE
    #define PJON_SERIAL_WRITE Serial.write
  #endif

  #ifndef PJON_SERIAL_READ
    #define PJON_SERIAL_READ Serial.read
  #endif

  #ifndef PJON_SERIAL_FLUSH
    #define PJON_SERIAL_FLUSH Serial.flush
  #endif

  /* Timing ----------------------------------------------------------------- */

  #ifndef PJON_DELAY_MICROSECONDS
    #define PJON_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef PJON_MICROS
    #define PJON_MICROS micros
  #endif
#endif
