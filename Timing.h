
/* PJON v2.0 Communication Timing table
   Copyright (c) 2016, Giovanni Blu Mitolo All rights reserved.

   All benchmarks should be executed with NetworkAnalysis and SpeedTest examples.

   STANDARD mode: Transfer speed: 16.944kBb or 2.12kB/s
   Absolute communication speed: 1.81kB/s
   Data throughput: 1.51kB/s

   FAST mode: Transfer speed: 25.157kBd or 3.15kB/s
   Absolute  communication speed: 2.55kB/s
   Data throughput: 2.13kB/s

   OVERDRIVE mode: Architecture / Toolchain dependant */

/* Arduino Duemilanove, Uno, Nano, Mini, Pro, Pro mini ecc. ----------------- */
#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #if MODE == STANDARD
    #if F_CPU == 16000000L
      #define BIT_WIDTH 40
      #define BIT_SPACER 112
      #define ACCEPTANCE 40
      #define READ_DELAY 4
    #endif
  #endif
  #if MODE == FAST
    #if F_CPU == 16000000L
      #define BIT_WIDTH 28
      #define BIT_SPACER 66
      #define ACCEPTANCE 28
      #define READ_DELAY 4
    #endif
  #endif
  #if MODE == OVERDRIVE
    #if F_CPU == 16000000L
    /* Transfer speed: 33.898kBd or 4.24kB/s
       Absolute  communication speed: 3.23kB/s
       Data throughput: 2.68kB/s */
      #define BIT_WIDTH 20
      #define BIT_SPACER 56
      #define ACCEPTANCE 20
      #define READ_DELAY 8
    #endif
  #endif
#endif

/* Arduino Leonardo / Micro ------------------------------------------------- */
#if defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
  /* Working nominally on pin 2 - 4 - 8 - 12 */
  #if MODE == STANDARD
    #define BIT_WIDTH 40
    #define BIT_SPACER 112
    #define ACCEPTANCE 40
    #define READ_DELAY 8
  #endif
  #if MODE == FAST
    /* Working nominally on pin 2 - 4 - 8 - 12 */
    #define BIT_WIDTH 28
    #define BIT_SPACER 66
    #define ACCEPTANCE 28
    #define READ_DELAY 14
  #endif
#endif

/* Arduino Mega / Mega nano ------------------------------------------------- */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if MODE == STANDARD
    #define BIT_WIDTH 38
    #define BIT_SPACER 110
    #define ACCEPTANCE 38
    #define READ_DELAY 11
  #endif
  #if MODE == FAST || MODE == OVERDRIVE
    #define BIT_WIDTH 26
    #define BIT_SPACER 64
    #define ACCEPTANCE 26
    #define READ_DELAY 12
  #endif
#endif

/* ATtiny85 ----------------------------------------------------------------- */
#if defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  #if MODE == STANDARD
    #if F_CPU == 8000000L
      #define BIT_WIDTH  35
      #define BIT_SPACER 108
      #define ACCEPTANCE 28
      #define READ_DELAY 15
    #endif
  #endif
#endif

/* Arduino Zero ------------------------------------------------------------- */
#if defined(ARDUINO_SAMD_ZERO)
  #if MODE == STANDARD
  /* Added by Esben Soeltoft - 03/09/2016 */
    #define BIT_WIDTH 40
    #define BIT_SPACER 112
    #define ACCEPTANCE 40
    #define READ_DELAY 4
  #endif
  #if MODE == OVERDRIVE
  /* Added by Esben Soeltoft - 09/03/2016
     Transfer speer: 48.000kBd or 6.00kB/s
     Absolute communication speed: 6.00kB/s
     Data throughput: 5.00kB/s */
    #define BIT_WIDTH 12
    #define BIT_SPACER 36
    #define ACCEPTANCE 12
    #define READ_DELAY 1
  #endif
#endif

/* NodeMCU, generic ESP8266 ------------------------------------------------- */
#if defined(ESP8266)
  #if MODE == STANDARD
  /* Added by github user 240974a - 09/03/2016  */
    #if F_CPU == 80000000L
      #define BIT_WIDTH  44
      #define BIT_SPACER 110
      #define ACCEPTANCE 35
      #define READ_DELAY 4
    #endif
  #endif
#endif
