
/* PJON AnalogSampling strategy Transmission Timing table
   Copyright (c) 2017, Giovanni Blu Mitolo All rights reserved.
   __________________________________________________________
  | 1 AS_STANDARD     | Transmission speed 1024Bb - 128B/s   |
  | ADC prescale 128  |                                      |
  |-------------------|--------------------------------------|
  | 2 AS_FAST         | Transmission speed 1361Bb - 170B/s   |
  | ADC prescale 128  |                                      |
  |-------------------|--------------------------------------|
  | 3 AS_OVERDRIVE_32 | Transmission speed 3773Bb - 471B/s   |
  | ADC prescale 32   |                                      |
  |-------------------|--------------------------------------|
  | 4 AS_OVERDRIVE_16 | Transmission speed 5547Bb - 639B/s   |
  | ADC prescale 16   |                                      |
  |-------------------|--------------------------------------|
  | 5 AS_OVERDRIVE_8  | Transmission speed 12658Bb - 1582B/s |
  | ADC prescale 8    | Caution, ADC clocked faster than     |
  |                   | manifacturer raccomends as maximum   |
  |                   | ADC sample rate (prescale 16)        |
  |___________________|______________________________________|

  All benchmarks are obtained with NetworkAnalysis and SpeedTest examples. */

#pragma once

#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #if AS_MODE == AS_STANDARD
    #if F_CPU == 16000000L
      #define AS_BIT_WIDTH    750
      #define AS_BIT_SPACER  1050
      #define AS_READ_DELAY     0
    #endif
  #elif AS_MODE == AS_FAST
    #if F_CPU == 16000000L
      #define AS_BIT_WIDTH    572
      #define AS_BIT_SPACER   728
      #define AS_READ_DELAY     0
    #endif
  #elif AS_MODE == AS_OVERDRIVE_32
    #ifndef AS_PRESCALE
      #define AS_PRESCALE      32
    #endif
    #if F_CPU == 16000000L
      #define AS_BIT_WIDTH    188
      #define AS_BIT_SPACER   428
      #define AS_READ_DELAY     0
    #endif
  #elif AS_MODE == AS_OVERDRIVE_16
    #ifndef AS_PRESCALE
      #define AS_PRESCALE      16
    #endif
    #if F_CPU == 16000000L
      #define AS_BIT_WIDTH    128
      #define AS_BIT_SPACER   290
      #define AS_READ_DELAY     0
    #endif
  #elif AS_MODE == AS_OVERDRIVE_8
    #ifndef AS_PRESCALE
      #define AS_PRESCALE       8
    #endif
    #if F_CPU == 16000000L
      #define AS_BIT_WIDTH     56
      #define AS_BIT_SPACER   128
      #define AS_READ_DELAY    16
    #endif
  #endif
#endif

/* ATmega1280/2560 - Arduino Mega/Mega-nano --------------------------------- */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if AS_MODE == AS_STANDARD
    #if F_CPU == 16000000L
      /* Standard timing is applied below */
    #endif
  #elif AS_MODE == AS_FAST
    #if F_CPU == 16000000L
      /* Standard timing is applied below */
    #endif
  #elif AS_MODE == AS_OVERDRIVE_32
    #ifndef AS_PRESCALE
      #define AS_PRESCALE 32
    #endif
    #if F_CPU == 16000000L
      /* Standard timing is applied below */
    #endif
  #elif AS_MODE == AS_OVERDRIVE_16
    #ifndef AS_PRESCALE
      #define AS_PRESCALE 16
    #endif
    #if F_CPU == 16000000L
      /* Standard timing is applied below */
    #endif
  #elif AS_MODE == AS_OVERDRIVE_8
    #ifndef AS_PRESCALE
      #define AS_PRESCALE 8
    #endif
    #if F_CPU == 16000000L
      /* TODO - define dedicated timing */
    #endif
  #endif
#endif

/* ATmega16/32U4 - Arduino Leonardo/Micro ----------------------------------- */
#if defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
  #if AS_MODE == AS_STANDARD
    /* TODO - define dedicated timing */
  #endif
#endif

/* NodeMCU, generic ESP8266 ------------------------------------------------- */
#if defined(ESP8266)
  #if AS_MODE == AS_STANDARD
    #if F_CPU == 80000000L
      /* TODO - define dedicated timing */
    #endif
  #endif
#endif

/* Fallback to standard timing if not previously defined: */

#if AS_MODE == AS_STANDARD
  #ifndef AS_BIT_WIDTH
    #define AS_BIT_WIDTH    750
  #endif
  #ifndef AS_BIT_SPACER
    #define AS_BIT_SPACER  1050
  #endif
  #ifndef AS_READ_DELAY
    #define AS_READ_DELAY     0
  #endif
#elif AS_MODE == AS_FAST
  #ifndef AS_BIT_WIDTH
    #define AS_BIT_WIDTH    572
  #endif
  #ifndef AS_BIT_SPACER
    #define AS_BIT_SPACER   728
  #endif
  #ifndef AS_READ_DELAY
    #define AS_READ_DELAY     0
  #endif
#elif AS_MODE == AS_OVERDRIVE_32
  #ifndef AS_BIT_WIDTH
    #define AS_BIT_WIDTH    188
  #endif
  #ifndef AS_BIT_SPACER
    #define AS_BIT_SPACER   428
  #endif
  #ifndef AS_READ_DELAY
    #define AS_READ_DELAY     0
  #endif
#elif AS_MODE == AS_OVERDRIVE_16
  #ifndef AS_BIT_WIDTH
    #define AS_BIT_WIDTH    128
  #endif
  #ifndef AS_BIT_SPACER
    #define AS_BIT_SPACER   290
  #endif
  #ifndef AS_READ_DELAY
    #define AS_READ_DELAY     0
  #endif
#elif AS_MODE == AS_OVERDRIVE_8
  #ifndef AS_BIT_WIDTH
    #define AS_BIT_WIDTH     56
  #endif
  #ifndef AS_BIT_SPACER
    #define AS_BIT_SPACER   128
  #endif
  #ifndef AS_READ_DELAY
    #define AS_READ_DELAY    16
  #endif
#endif

/* Synchronous acknowledgement response timeout. (15 milliseconds default).
   If (latency + CRC computation) > AS_RESPONSE_TIMEOUT synchronous
   acknowledgement reliability could be affected or disrupted higher
   AS_RESPONSE_TIMEOUT if necessary. */

#ifndef AS_RESPONSE_TIMEOUT
  #define AS_RESPONSE_TIMEOUT 15000
#endif

/* Maximum initial delay in milliseconds: */

#ifndef AS_INITIAL_DELAY
  #define AS_INITIAL_DELAY     1000
#endif

/* Maximum delay in case of collision in microseconds: */

#ifndef AS_COLLISION_DELAY
  #define AS_COLLISION_DELAY     64
#endif

/* Maximum transmission attempts */

#ifndef AS_MAX_ATTEMPTS
  #define AS_MAX_ATTEMPTS        10
#endif

/* Back-off exponential degree */

#ifndef AS_BACK_OFF_DEGREE
  #define AS_BACK_OFF_DEGREE      5
#endif

/* Threshold decrease interval (10 millis) */

#ifndef AS_THRESHOLD_DECREASE_INTERVAL
  #define AS_THRESHOLD_DECREASE_INTERVAL 100000
#endif
