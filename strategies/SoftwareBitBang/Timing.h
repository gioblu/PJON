
/* PJON SoftwareBitBang strategy Transmission Timing table
   Copyright (c) 2012-2017, Giovanni Blu Mitolo All rights reserved.

   Often timing in two different architectures doesn't match. This happens because
   PJON's code execution time can variate, and also time measurement can be
   be not perfectly equal. Arduino Duemilanove / Uno / Nano timing is considered
   as the master. All benchmarks should be executed with NetworkAnalysis
   and SpeedTest examples.

   SWBB_STANDARD mode: 16.944kBb or 2.12kB/s
   SWBB_FAST mode: 21.504kBd or 2.68kB/s
   SWBB_OVERDRIVE mode: Architecture / Toolchain dependant */

/* ATmega88/168/328 - Arduino Duemilanove, Uno, Nano, Mini, Pro, Pro mini --- */
#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #if SWBB_MODE == SWBB_STANDARD
    #if F_CPU == 16000000L
      #define SWBB_BIT_WIDTH 40
      #define SWBB_BIT_SPACER 112
      #define SWBB_ACCEPTANCE 40
      #define SWBB_READ_DELAY 4
    #endif
  #endif
  #if SWBB_MODE == SWBB_FAST
    #if F_CPU == 16000000L
      #define SWBB_BIT_WIDTH 32
      #define SWBB_BIT_SPACER 84
      #define SWBB_ACCEPTANCE 32
      #define SWBB_READ_DELAY 4
    #endif
  #endif
  #if SWBB_MODE == SWBB_OVERDRIVE
    #if F_CPU == 16000000L
    /* Speed: 31.250kBd or 3.906kB/s */
      #define SWBB_BIT_WIDTH  20
      #define SWBB_BIT_SPACER 76
      #define SWBB_ACCEPTANCE 20
      #define SWBB_READ_DELAY 8
    #endif
  #endif
#endif

/* ATmega16/32U4 - Arduino Leonardo/Micro ----------------------------------- */
#if defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
  /* Working nominally on pin 2 - 4 - 8 - 12 */
  #if SWBB_MODE == SWBB_STANDARD
    #define SWBB_BIT_WIDTH 40
    #define SWBB_BIT_SPACER 112
    #define SWBB_ACCEPTANCE 40
    #define SWBB_READ_DELAY 8
  #endif
  #if SWBB_MODE == SWBB_FAST
    /* Working nominally on pin 2 - 4 - 8 - 12 */
    #define SWBB_BIT_WIDTH 32
    #define SWBB_BIT_SPACER 84
    #define SWBB_ACCEPTANCE 32
    #define SWBB_READ_DELAY 9
  #endif
#endif

/* ATmega1280/2560 - Arduino Mega/Mega-nano --------------------------------- */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if SWBB_MODE == SWBB_STANDARD
    #define SWBB_BIT_WIDTH 38
    #define SWBB_BIT_SPACER 110
    #define SWBB_ACCEPTANCE 38
    #define SWBB_READ_DELAY 11
  #endif
  #if SWBB_MODE == SWBB_FAST || SWBB_MODE == SWBB_OVERDRIVE
    #define SWBB_BIT_WIDTH 30
    #define SWBB_BIT_SPACER 82
    #define SWBB_ACCEPTANCE 30
    #define SWBB_READ_DELAY 8
  #endif
#endif

/* ATtiny45/85 -------------------------------------------------------------- */
#if defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  #if SWBB_MODE == SWBB_STANDARD
    #if F_CPU == 16000000L
      /* External 16MHz oscillator */
      #define SWBB_BIT_WIDTH  40
      #define SWBB_BIT_SPACER 112
      #define SWBB_ACCEPTANCE 40
      #define SWBB_READ_DELAY 4
    #endif
  #endif
  #if SWBB_MODE == SWBB_FAST
    #if F_CPU == 16000000L
      /* External 16MHz oscillator */
      #define SWBB_BIT_WIDTH 32
      #define SWBB_BIT_SPACER 84
      #define SWBB_ACCEPTANCE 32
      #define SWBB_READ_DELAY 4
    #endif
  #endif
#endif

/* Arduino Zero ------------------------------------------------------------- */
#if defined(ARDUINO_SAMD_ZERO)
  #if SWBB_MODE == SWBB_STANDARD
  /* Added by Esben Soeltoft - 03/09/2016 */
    #define SWBB_BIT_WIDTH 40
    #define SWBB_BIT_SPACER 112
    #define SWBB_ACCEPTANCE 40
    #define SWBB_READ_DELAY 4
  #endif
  #if SWBB_MODE == SWBB_OVERDRIVE
  /* Added by Esben Soeltoft - 09/03/2016
     Speed: 48.000kBd or 6.00kB/s */
    #define SWBB_BIT_WIDTH 12
    #define SWBB_BIT_SPACER 36
    #define SWBB_ACCEPTANCE 12
    #define SWBB_READ_DELAY 1
  #endif
#endif

/* NodeMCU, generic ESP8266 ------------------------------------------------- */
#if defined(ESP8266)
  #if SWBB_MODE == SWBB_STANDARD
  /* Added by github user 240974a - 09/03/2016  */
    #if F_CPU == 80000000L
      #define SWBB_BIT_WIDTH  44
      #define SWBB_BIT_SPACER 110
      #define SWBB_ACCEPTANCE 35
      #define SWBB_READ_DELAY 4
    #endif
  #endif
#endif

/* MK20DX256 - Teensy ------------------------------------------------------- */
#if defined(__MK20DX256__)
  #if SWBB_MODE == SWBB_STANDARD
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
#if SWBB_MODE == SWBB_STANDARD
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
#if SWBB_MODE == SWBB_FAST
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
#if SWBB_MODE == SWBB_OVERDRIVE
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH 17
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER 52
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE 17
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY 8
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
  #define SWBB_MAX_ATTEMPTS 20
#endif

/* Back-off exponential degree */
#ifndef SWBB_BACK_OFF_DEGREE
  #define SWBB_BACK_OFF_DEGREE 4
#endif
