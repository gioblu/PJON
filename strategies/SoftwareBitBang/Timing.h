
/* PJON SoftwareBitBang strategy Transmission Timing table
   Copyright (c) 2012-2016, Giovanni Blu Mitolo All rights reserved.

   Often timing in two different architectures doesn't match. This happens because
   PJON's code execution time can variate, and also time measurement can be
   be not perfectly equal. Arduino Duemilanove / Uno / Nano timing is considered
   as the master. All benchmarks should be executed with NetworkAnalysis
   and SpeedTest examples.

   _SWBB_STANDARD mode: Transfer speed: 16.944kBb or 2.12kB/s
   Absolute communication speed: 1.81kB/s
   Data throughput: 1.51kB/s

   _SWBB_FAST mode: Transfer speed: 25.157kBd or 3.15kB/s
   Absolute  communication speed: 2.55kB/s
   Data throughput: 2.13kB/s

   _SWBB_OVERDRIVE mode: Architecture / Toolchain dependant */

/* Arduino Duemilanove, Uno, Nano, Mini, Pro, Pro mini ecc. ----------------- */
#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #if SWBB_MODE == _SWBB_STANDARD
    #if F_CPU == 16000000L
      #define SWBB_BIT_WIDTH 40
      #define SWBB_BIT_SPACER 112
      #define SWBB_ACCEPTANCE 40
      #define SWBB_READ_DELAY 4
    #endif
  #endif
  #if SWBB_MODE == _SWBB_FAST
    #if F_CPU == 16000000L
      #define SWBB_BIT_WIDTH 28
      #define SWBB_BIT_SPACER 66
      #define SWBB_ACCEPTANCE 28
      #define SWBB_READ_DELAY 4
    #endif
  #endif
  #if SWBB_MODE == _SWBB_OVERDRIVE
    #if F_CPU == 16000000L
    /* Transfer speed: 33.898kBd or 4.24kB/s
       Absolute  communication speed: 3.23kB/s
       Data throughput: 2.68kB/s */
      #define SWBB_BIT_WIDTH 20
      #define SWBB_BIT_SPACER 56
      #define SWBB_ACCEPTANCE 20
      #define SWBB_READ_DELAY 8
    #endif
  #endif
#endif

/* Arduino Leonardo / Micro ------------------------------------------------- */
#if defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
  /* Working nominally on pin 2 - 4 - 8 - 12 */
  #if SWBB_MODE == _SWBB_STANDARD
    #define SWBB_BIT_WIDTH 40
    #define SWBB_BIT_SPACER 112
    #define SWBB_ACCEPTANCE 40
    #define SWBB_READ_DELAY 8
  #endif
  #if SWBB_MODE == _SWBB_FAST
    /* Working nominally on pin 2 - 4 - 8 - 12 */
    #define SWBB_BIT_WIDTH 28
    #define SWBB_BIT_SPACER 66
    #define SWBB_ACCEPTANCE 28
    #define SWBB_READ_DELAY 14
  #endif
#endif

/* Arduino Mega / Mega nano ------------------------------------------------- */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if SWBB_MODE == _SWBB_STANDARD
    #define SWBB_BIT_WIDTH 38
    #define SWBB_BIT_SPACER 110
    #define SWBB_ACCEPTANCE 38
    #define SWBB_READ_DELAY 11
  #endif
  #if SWBB_MODE == _SWBB_FAST || SWBB_MODE == _SWBB_OVERDRIVE
    #define SWBB_BIT_WIDTH 26
    #define SWBB_BIT_SPACER 64
    #define SWBB_ACCEPTANCE 26
    #define SWBB_READ_DELAY 12
  #endif
#endif

/* ATtiny85 ----------------------------------------------------------------- */
#if defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  #if SWBB_MODE == _SWBB_STANDARD
    #if F_CPU == 8000000L
      /* Internal oscillator */
      #define SWBB_BIT_WIDTH  34
      #define SWBB_BIT_SPACER 114
      #define SWBB_ACCEPTANCE 34
      #define SWBB_READ_DELAY 10
    #endif
  #endif
#endif

/* Arduino Zero ------------------------------------------------------------- */
#if defined(ARDUINO_SAMD_ZERO)
  #if SWBB_MODE == _SWBB_STANDARD
  /* Added by Esben Soeltoft - 03/09/2016 */
    #define SWBB_BIT_WIDTH 40
    #define SWBB_BIT_SPACER 112
    #define SWBB_ACCEPTANCE 40
    #define SWBB_READ_DELAY 4
  #endif
  #if SWBB_MODE == _SWBB_OVERDRIVE
  /* Added by Esben Soeltoft - 09/03/2016
     Transfer speer: 48.000kBd or 6.00kB/s
     Absolute communication speed: 6.00kB/s
     Data throughput: 5.00kB/s */
    #define SWBB_BIT_WIDTH 12
    #define SWBB_BIT_SPACER 36
    #define SWBB_ACCEPTANCE 12
    #define SWBB_READ_DELAY 1
  #endif
#endif

/* NodeMCU, generic ESP8266 ------------------------------------------------- */
#if defined(ESP8266)
  #if SWBB_MODE == _SWBB_STANDARD
  /* Added by github user 240974a - 09/03/2016  */
    #if F_CPU == 80000000L
      #define SWBB_BIT_WIDTH  44
      #define SWBB_BIT_SPACER 110
      #define SWBB_ACCEPTANCE 35
      #define SWBB_READ_DELAY 4
    #endif
  #endif
#endif

/* Teensy ------------------------------------------------------------------- */
#if defined(__MK20DX256__)
  #if SWBB_MODE == _SWBB_STANDARD
  /* Added by github user SticilFace - 25/04/2016  */
    #if F_CPU == 96000000L
      #define SWBB_BIT_WIDTH 46
      #define SWBB_BIT_SPACER 112
      #define SWBB_ACCEPTANCE 40
      #define SWBB_READ_DELAY -10
    #endif
  #endif
#endif

/* Avoid error if any previous defined -------------------------------------- */
#if SWBB_MODE == _SWBB_STANDARD
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH 40
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER 112
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE 40
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY 4
  #endif
#endif
#if SWBB_MODE == _SWBB_FAST
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH 28
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER 66
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE 28
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY 4
  #endif
#endif
#if SWBB_MODE == _SWBB_OVERDRIVE
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH 20
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER 56
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE 20
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY 8
  #endif
#endif

/* The default response timeout setup dedicates the transmission time of 1 byte plus
   1 millisecond to latency and CRC computation. If receiver needs more than
   SWBB_TIMEOUT to compute CRC and answer back ACK, transmitter will not receive
   the incoming synchronous ACK (Uno CRC8 computation: 150 microseconds).
   Higher or lower if necessary! */

#ifndef SWBB_LATENCY
  #define SWBB_LATENCY 1000
#endif

#define SWBB_TIMEOUT ((SWBB_BIT_WIDTH * 9) + SWBB_BIT_SPACER + SWBB_LATENCY)
