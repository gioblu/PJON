
/* PJON SoftwareBitBang strategy Transmission Timing table
   Copyright 2010-2019, Giovanni Blu Mitolo All rights reserved.

   Often timing in two different architectures do not match. Code execution
   time can variate and time measurements can be not perfectly equal.
   Arduino Duemilanove/UNO/Nano durations are used as master.
   Consider that master durations defined below are shorter than specified
   in the PJDL-specification-v2.1 to accomodate the input-output pin change
   duration (4 microseconds) and effectively produce the specified durations.

   Benchmarks can be executed using NetworkAnalysis and SpeedTest examples.

   MODE   1: 1.95kB/s - 15625Bd
   MODE   2: 2.21kB/s - 17696Bd
   MODE   3: 2.97kB/s - 23529Bd
   MODE   4: 3.40kB/s - 27210Bd

   Use the same pin number on all connected devices to achieve maximum
   timing efficiency, not all different pin combinations work nominally
   because of execution timing discrepancies between physical pins. */

#pragma once

/* ATmega88/168/328 - Arduino Duemilanove, Uno, Nano, Mini, Pro, Pro mini */
#if defined(__AVR_ATmega88__)  || defined(__AVR_ATmega168__)  || \
    defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__)
  #if SWBB_MODE == 1
    #if F_CPU == 16000000L
      /* Working on pin: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 */
      #define SWBB_BIT_WIDTH   40
      #define SWBB_BIT_SPACER 112
      #define SWBB_ACCEPTANCE  56
      #define SWBB_READ_DELAY   4
    #endif
  #endif
  #if SWBB_MODE == 2
    #if F_CPU == 16000000L
      /* Working on pin: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 */
      #define SWBB_BIT_WIDTH   36
      #define SWBB_BIT_SPACER  88
      #define SWBB_ACCEPTANCE  56
      #define SWBB_READ_DELAY   4
    #endif
  #endif
  #if SWBB_MODE == 3
    #if F_CPU == 16000000L
      /* Working on pin: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 */
      #define SWBB_BIT_WIDTH   24
      #define SWBB_BIT_SPACER  80
      #define SWBB_ACCEPTANCE  40
      #define SWBB_READ_DELAY   8
    #endif
  #endif
  #if SWBB_MODE == 4
    #if F_CPU == 16000000L
      /* Working on pin: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 */
      #define SWBB_BIT_WIDTH   22
      #define SWBB_BIT_SPACER  56
      #define SWBB_ACCEPTANCE  30
      #define SWBB_READ_DELAY   9
    #endif
  #endif
#endif

/* ATmega16/32U4 - Arduino Leonardo/Micro --------------------------------- */
#if defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
  #if SWBB_MODE == 1
    /* Working on pin: 2, 4, 8, 12
       Fallback to default timing */
    #define SWBB_READ_DELAY   8
  #endif
  #if SWBB_MODE == 2
    /* Working on pin: 2, 4, 8, 12
       Fallback to default timing */
    #define SWBB_READ_DELAY  12
  #endif
#endif

/* ATmega1280/2560 - Arduino Mega/Mega-nano ------------------------------- */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if SWBB_MODE == 1
    /* Working on pin: 3, 4, 7, 8, 9, 10, 12 */
    #define SWBB_BIT_WIDTH   38
    #define SWBB_BIT_SPACER 110
    #define SWBB_ACCEPTANCE  62
    #define SWBB_READ_DELAY  11
  #endif
  #if SWBB_MODE == 2
    /* Working on pin: 3, 4, 7, 8, 9, 10, 12 */
    #define SWBB_BIT_WIDTH   34
    #define SWBB_BIT_SPACER  86
    #define SWBB_ACCEPTANCE  58
    #define SWBB_READ_DELAY  10
  #endif
#endif

/* ATtiny45/85 ------------------------------------------------------------ */
#if defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  #if SWBB_MODE == 1
    #if F_CPU == 16000000L
      /* Working on pin: 1, 2
         Fallback to default */
    #endif
  #endif
  #if SWBB_MODE == 2
    #if F_CPU == 16000000L
      /* Working on pin: 1, 2
         Fallback to default */
    #endif
  #endif
#endif

/* ATtiny44/84/44A/84A ---------------------------------------------------- */
#if defined(__AVR_ATtiny44__)  || defined(__AVR_ATtiny84__) || \
    defined(__AVR_ATtiny84A__) || defined(__AVR_ATtiny84A__)
  #if SWBB_MODE == 1
    #if F_CPU == 16000000L
      /* Working on pin: 0, 1, 2, 3, 4
         Fallback to default */
    #endif
  #endif
  #if SWBB_MODE == 2
    #if F_CPU == 16000000L
      /* Working on pin: 0, 1, 2, 3, 4
         Fallback to default */
    #endif
  #endif
#endif

/* Arduino Zero ----------------------------------------------------------- */
#if defined(ARDUINO_SAMD_ZERO)
  #if SWBB_MODE == 1
  /* Added by Esben Soeltoft - 03/09/2016
     Updated by Giovanni Blu Mitolo - 31/05/2019
     Working on pin: D0, D1, D3, A0, A1 */
    #define SWBB_BIT_WIDTH   43
    #define SWBB_BIT_SPACER 115
    #define SWBB_ACCEPTANCE  40
    #define SWBB_READ_DELAY  -4
  #endif
  #if SWBB_MODE == 3
  /* Added by Esben Soeltoft - 09/03/2016
     Speed: 48000Bd or 6.00kB/s */
    #define SWBB_BIT_WIDTH   12
    #define SWBB_BIT_SPACER  36
    #define SWBB_ACCEPTANCE  12
    #define SWBB_READ_DELAY   1
  #endif
#endif

/* NodeMCU, generic ESP8266 ----------------------------------------------- */
#if defined(ESP8266)
  #if SWBB_MODE == 1
  /* Added by github user 240974a                 - 09/03/2016
     Added full support to MODE 1 (80 and 160MHz) - 12/06/2018 */
    #if (F_CPU == 80000000L) || (F_CPU == 160000000L)
      /* Working on pin: D1 or GPIO 5 */
      #define SWBB_BIT_WIDTH   44
      #define SWBB_BIT_SPACER 112
      #define SWBB_ACCEPTANCE  56
      #define SWBB_READ_DELAY  -6
    #endif
  #endif
#endif

/* Heltech WiFi LoRa ESP32, generic ESP32 --------------------------------- */
#if defined(ESP32)
  #if SWBB_MODE == 1
      /* Added full support to MODE 1 - 28/06/2018
         Working on pin: 12 and 25 */
      #define SWBB_BIT_WIDTH   44
      #define SWBB_BIT_SPACER 112
      #define SWBB_ACCEPTANCE  56
      #define SWBB_READ_DELAY  -2
    #endif
#endif

/* MK20DX256 - Teensy ----------------------------------------------------- */
#if defined(__MK20DX256__)
  #if SWBB_MODE == 1
  /* Added by github user SticilFace - 25/04/2016  */
    #if F_CPU == 96000000L
      #define SWBB_BIT_WIDTH   46
      #define SWBB_BIT_SPACER 112
      #define SWBB_ACCEPTANCE  40
      #define SWBB_READ_DELAY -10
    #endif
  #endif
#endif

/* STM32F1 ---------------------------------------------------------------- */
#if defined(__STM32F1__)
  #if SWBB_MODE == 1
    #if F_CPU == 72000000L
    /* Added by github user osman-aktepe - 22/06/2018 */
      #define SWBB_BIT_WIDTH   44
      #define SWBB_BIT_SPACER 112
      #define SWBB_ACCEPTANCE  40
      #define SWBB_READ_DELAY -10
    #endif
  #endif
#endif

/* Avoid error if any previous defined ------------------------------------ */
#if SWBB_MODE == 1
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH   40
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER 112
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE  56
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY   4
  #endif
#endif
#if SWBB_MODE == 2
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH   36
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER  88
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE  56
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY   4
  #endif
#endif
#if SWBB_MODE == 3
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH   24
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER  80
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE  40
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY   8
  #endif
#endif
#if SWBB_MODE == 4
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH   22
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER  56
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE  30
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY   9
  #endif
#endif

/* Synchronous acknowledgement response timeout. (1.5 milliseconds default).
   If (latency + CRC computation) > SWBB_RESPONSE_TIMEOUT synchronous
   acknowledgement reliability could be affected or disrupted higher
   SWBB_RESPONSE_TIMEOUT if necessary. */

#ifndef SWBB_RESPONSE_TIMEOUT
  #define SWBB_RESPONSE_TIMEOUT 1500
#endif

/* Maximum initial delay in milliseconds: */

#ifndef SWBB_INITIAL_DELAY
  #define SWBB_INITIAL_DELAY 1000
#endif

/* Maximum delay in case of collision in microseconds: */

#ifndef SWBB_COLLISION_DELAY
  #define SWBB_COLLISION_DELAY 16
#endif

/* Maximum transmission attempts */

#ifndef SWBB_MAX_ATTEMPTS
  #define SWBB_MAX_ATTEMPTS    20
#endif

/* Back-off exponential degree */

#ifndef SWBB_BACK_OFF_DEGREE
  #define SWBB_BACK_OFF_DEGREE  4
#endif
