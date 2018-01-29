
/* PJON OverSampling strategy Transmission Timing table
   Copyright 2018, Giovanni Blu Mitolo All rights reserved.

   All benchmarks are executed with NetworkAnalysis and SpeedTest examples.

   The following constants setup is quite conservative and determined only
   with a huge amount of time and blind testing (without oscilloscope)
   tweaking values and analysing results. Theese can be changed to obtain
   faster speed. Probably you need experience, time and an oscilloscope. */

#pragma once

/* ATmega88/168/328 - Arduino Duemilanove, Uno, Nano, Mini, Pro, Pro mini */
#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || \
    defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #if OS_MODE == 1
    #if F_CPU == 16000000L
      #define OS_BIT_WIDTH   512
      #define OS_BIT_SPACER  328
    #endif
  #endif
#endif

/* ATmega1280/2560 - Arduino Mega/Mega-nano ------------------------------- */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if OS_MODE == 1
    #define OS_BIT_WIDTH     508
    #define OS_BIT_SPACER    332
  #endif
#endif

/* ATmega16/32U4 - Arduino Leonardo/Micro --------------------------------- */
#if defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
  #if OS_MODE == 1
    #define OS_BIT_WIDTH     508
    #define OS_BIT_SPACER    332
  #endif
#endif

/* NodeMCU, generic ESP8266 ----------------------------------------------- */
#if defined(ESP8266)
  #if OS_MODE == 1
    #if F_CPU == 80000000L
      #define OS_BIT_WIDTH   520
      #define OS_BIT_SPACER  332
    #endif
  #endif
#endif

#ifndef OS_BIT_WIDTH
  #define OS_BIT_WIDTH       512  // 520 microseconds detected by oscilloscope
#endif

#ifndef OS_BIT_SPACER
  #define OS_BIT_SPACER      328  // 340 microseconds detected by oscilloscope
#endif

/* Preamble data sent for receiver to tune its gain to signal dB: */

#ifndef OS_PREAMBLE_PULSE_WIDTH
  #define OS_PREAMBLE_PULSE_WIDTH 100000
#endif

/* The default response timeout setup dedicates the transmission time of 1
   byte plus 1 millisecond for latency and CRC computation. If receiver needs
   more than OS_TIMEOUT to compute CRC and answer back ACK, transmitter will
   not receive the incoming synchronous ACK, Higher or lower if necessary! */

#ifndef OS_LATENCY
  #define OS_LATENCY         4000
#endif

#define OS_TIMEOUT ((OS_BIT_WIDTH * 9) + OS_BIT_SPACER + OS_LATENCY)

/* Maximum initial delay in milliseconds: */

#ifndef OS_INITIAL_DELAY
  #define OS_INITIAL_DELAY  1000
#endif

/* Maximum delay in case of collision in microseconds: */

#ifndef OS_COLLISION_DELAY
  #define OS_COLLISION_DELAY  64
#endif

/* Maximum transmission attempts */

#ifndef OS_MAX_ATTEMPTS
  #define OS_MAX_ATTEMPTS    10
#endif

/* Back-off exponential degree */

#ifndef OS_BACK_OFF_DEGREE
  #define OS_BACK_OFF_DEGREE 5
#endif
