
/* PJON AnalogSampling strategy Transmission Timing table
   Copyright (c) 2017, Giovanni Blu Mitolo All rights reserved.

    AS_STANDARD transmission mode performance:
    Transfer speed: 1024Bb or 128B/s
    Absolute  communication speed: 128B/s (data length 20 of characters)
    Data throughput: 100B/s (data length 20 of characters)

   All benchmarks should be executed with NetworkAnalysis and SpeedTest examples. */

#pragma once

#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #if AS_MODE == AS_STANDARD
    #if F_CPU == 16000000L
      #define AS_BIT_WIDTH    750
      #define AS_BIT_SPACER  1050
      #define AS_READ_DELAY     0
    #endif
  #endif
#endif

/* ATmega1280/2560 - Arduino Mega/Mega-nano --------------------------------- */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if AS_MODE == AS_STANDARD
    /* TODO - define dedicated timing */
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

#ifndef AS_BIT_WIDTH
  #define AS_BIT_WIDTH          750
#endif

#ifndef AS_BIT_SPACER
  #define AS_BIT_SPACER        1050
#endif

#ifndef AS_READ_DELAY
  #define AS_READ_DELAY          0
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

/* Threshold decrease interval (750 millis) */

#ifndef AS_THRESHOLD_DECREASE_INTERVAL
  #define AS_THRESHOLD_DECREASE_INTERVAL 750000
#endif
