
/* PJON SoftwareBitBang strategy Transmission Timing table
   Copyright 2010-2020, Giovanni Blu Mitolo All rights reserved.

   Often timing in two different machines do not match, code execution
   time can variate and time measurements can be not perfectly equal.
   Consider that durations defined below may differ from what is specified in
   PJDL v5.0. This is done to accomodate machine's inner workings and
   effectively produce the specified timing.

   Arduino Duemilanove/UNO/Nano is used as timing master, or the machine used
   to test all new supported MCUs.

   Benchmarks can be executed using NetworkAnalysis and SpeedTest examples.

   MODE   1: 1.97kB/s - 15808Bd
   MODE   2: 2.21kB/s - 17696Bd
   MODE   3: 3.10kB/s - 24844Bd
   MODE   4: 3.34kB/s - 26755Bd

   Use the same pin number on all connected devices to achieve maximum
   timing efficiency, not all different pin combinations work nominally
   because of execution timing discrepancies between physical pins. */

#pragma once

/* ATmega88/168/328 - Arduino Duemilanove, Uno, Nano, Mini, Pro, Pro mini */
#if defined(__AVR_ATmega88__)  || defined(__AVR_ATmega168__) || \
    defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #if SWBB_MODE == 1
    #if F_CPU == 16000000L
      /* Working on pin: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 */
      #define SWBB_BIT_WIDTH   40
      #define SWBB_BIT_SPACER 106
      #define SWBB_ACCEPTANCE  53
      #define SWBB_READ_DELAY   3
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
      #define SWBB_BIT_SPACER  66
      #define SWBB_ACCEPTANCE  30
      #define SWBB_READ_DELAY   8
    #endif
  #endif
  #if SWBB_MODE == 4
    #if F_CPU == 16000000L
      /* Working on pin: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 */
      #define SWBB_BIT_WIDTH   22
      #define SWBB_BIT_SPACER  61
      #define SWBB_ACCEPTANCE  30
      #define SWBB_READ_DELAY   7
    #endif
  #endif
#endif

/* ATmega328PB ------------------------------------------------------------ */
#if defined(__AVR_ATmega328PB__)
  #if SWBB_MODE == 1
    #if F_CPU == 16000000L
      /* Working on pin: 10 */
      #define SWBB_BIT_WIDTH   37
      #define SWBB_BIT_SPACER 110
      #define SWBB_ACCEPTANCE  56
      #define SWBB_READ_DELAY   4
    #endif
  #endif
  #if SWBB_MODE == 2
    #if F_CPU == 16000000L
      /* Working on pin: 10 */
      #define SWBB_BIT_WIDTH   33
      #define SWBB_BIT_SPACER  88
      #define SWBB_ACCEPTANCE  56
      #define SWBB_READ_DELAY   8
    #endif
  #endif
#endif


/* ATmega16/32U4 - Arduino Leonardo/Micro --------------------------------- */
#if defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
  #if SWBB_MODE == 1
    /* Working on pin: 2, 4, 8, 12 */
    #define SWBB_BIT_WIDTH   40
    #define SWBB_BIT_SPACER 106
    #define SWBB_ACCEPTANCE  53
    #define SWBB_READ_DELAY   6
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
    #define SWBB_BIT_SPACER 104
    #define SWBB_ACCEPTANCE  53
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
     Updated by Giovanni Blu Mitolo - 21/07/2020
     Working on pin: D0, D1, D3, A0, A1 */
    #define SWBB_BIT_WIDTH   43.5
    #define SWBB_BIT_SPACER 109.5
    #define SWBB_ACCEPTANCE  45
    #define SWBB_READ_DELAY  -4
  #endif
#endif

/* NodeMCU, generic ESP8266 ----------------------------------------------- */
#if defined(ESP8266)
  #if SWBB_MODE == 1
  /* Added by github user 240974a                 - 09/03/2016
     Added full support to MODE 1 (80 and 160MHz) - 12/06/2018 */
    #if (F_CPU == 80000000L)
      /* Working on pin: D1 or GPIO 5 */
      #define SWBB_BIT_WIDTH   43.5
      #define SWBB_BIT_SPACER 109.5
      #define SWBB_ACCEPTANCE  52
      #define SWBB_READ_DELAY  -6
    #endif
    #if (F_CPU == 160000000L)
      /* Working on pin: D1 or GPIO 5 */
      #define SWBB_BIT_WIDTH   44
      #define SWBB_BIT_SPACER 110
      #define SWBB_ACCEPTANCE  52
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
      #define SWBB_BIT_SPACER 110
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
/* Added by jcallano - 09-07-2020
   Working on pin: PB15, PB14, PB13, PB12, PB11, PB10, PB9, PB8, PB7, PB6, PB4,
   PB3, PA15, PA10. 5v tolerant pins on bluepill */
#if defined(__STM32F1__)
  #if SWBB_MODE == 1
    #if F_CPU == 72000000L
      #define SWBB_BIT_WIDTH   43.5
      #define SWBB_BIT_SPACER 109.5
      #define SWBB_ACCEPTANCE  75
      #define SWBB_READ_DELAY  -6
    #endif
  #endif
  #if SWBB_MODE == 2
    #if F_CPU == 72000000L
      #define SWBB_BIT_WIDTH   39.5
      #define SWBB_BIT_SPACER  91.5
      #define SWBB_ACCEPTANCE  53
      #define SWBB_READ_DELAY  -8
    #endif
  #endif
  #if SWBB_MODE == 3
    #if F_CPU == 72000000L
      #define SWBB_BIT_WIDTH   27.5
      #define SWBB_BIT_SPACER  69.5
      #define SWBB_ACCEPTANCE  33
      #define SWBB_READ_DELAY  -5
    #endif
  #endif
  #if SWBB_MODE == 4
    #if F_CPU == 72000000L
      #define SWBB_BIT_WIDTH   25
      #define SWBB_BIT_SPACER  59
      #define SWBB_ACCEPTANCE  30 //
      #define SWBB_READ_DELAY  4
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
  #ifndef SWBB_LATENCY
    #define SWBB_LATENCY     13
  #endif
  #ifndef SWBB_DEVIATION
    #define SWBB_DEVIATION   10
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
  #ifndef SWBB_LATENCY
    #define SWBB_LATENCY     10
  #endif
  #ifndef SWBB_DEVIATION
    #define SWBB_DEVIATION    9
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
  #ifndef SWBB_LATENCY
    #define SWBB_LATENCY      8
  #endif
  #ifndef SWBB_DEVIATION
    #define SWBB_DEVIATION    7
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
  #ifndef SWBB_LATENCY
    #define SWBB_LATENCY      5
  #endif
  #ifndef SWBB_DEVIATION
    #define SWBB_DEVIATION    7
  #endif
#endif

/* Synchronous acknowledgement response offset.
   If (latency + CRC computation) > (SWBB_RESPONSE_OFFSET * length)
   synchronous acknowledgement reliability could be affected or disrupted
   set a higher SWBB_RESPONSE_OFFSET if necessary. */

#ifndef SWBB_RESPONSE_OFFSET
  #define SWBB_RESPONSE_OFFSET 20
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
