
/* PJON v2.0 Communication Timing table
   Copyright (c) 2016, Giovanni Blu Mitolo All rights reserved.

   All benchmarks should be executed with NetworkAnalysis and SpeedTest examples.

   STANDARD mode: Transfer speed: 16.944Bb or 2.12kB/s
   Absolute communication speed: 1.81kB/s
   Data throughput: 1.51kB/s

   FAST mode: Transfer speed: 25.157Bd or 3.15kB/s
   Absolute  communication speed: 2.55kB/s
   Data throughput: 2.13kB/s

   OVERDRIVE mode: Architecture / Toolchain dependant */

#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #if MODE == STANDARD
    #if F_CPU == 8000000L
    // To be implemented
    #endif
    #if F_CPU == 16000000L
      #define BIT_WIDTH 40
      #define BIT_SPACER 112
      #define ACCEPTANCE 40
      #define READ_DELAY 4
    #endif
  #endif

  #if MODE == FAST
    #if F_CPU == 8000000L
    // To be implemented
    #endif
    #if F_CPU == 16000000L
      #define BIT_WIDTH 28
      #define BIT_SPACER 66
      #define ACCEPTANCE 28
      #define READ_DELAY 4
    #endif
  #endif

  #if MODE == OVERDRIVE
    #if F_CPU == 8000000L
      // To be implemented
    #endif
    #if F_CPU == 16000000L
    /* Transfer speed: 33.898Bd or 4.24kB/s
       Absolute  communication speed: 3.23kB/s
       Data throughput: 2.68kB/s */
      #define BIT_WIDTH 20
      #define BIT_SPACER 56
      #define ACCEPTANCE 20
      #define READ_DELAY 8
    #endif
  #endif
#endif


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


#if defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  #if MODE == STANDARD
    #if F_CPU == 8000000L
      #define BIT_WIDTH  35
      #define BIT_SPACER 107
      #define ACCEPTANCE 35
      #define READ_DELAY 15
    #endif
    #if F_CPU == 16000000L
      // To be implemented
    #endif
  #endif

  #if MODE == FAST
    #if F_CPU == 8000000L
      // To be implemented (hopefully fast enough)
    #endif
    #if F_CPU == 16000000L
      // To be implemented
    #endif
  #endif

  #if MODE == OVERDRIVE
    #if F_CPU == 8000000L
      // To be implemented (maybe senseless)
    #endif
    #if F_CPU == 16000000L
      // To be implemented
    #endif
  #endif
#endif


#if defined(ARDUINO_SAMD_ZERO)
  #if MODE == STANDARD
  /* Added by Esben Soeltoft - 03/09/2016 */
    #define BIT_WIDTH 40
    #define BIT_SPACER 112
    #define ACCEPTANCE 40
    #define READ_DELAY 4
  #endif

  #if MODE == FAST
    // To be implemented
  #endif

  #if MODE == OVERDRIVE
  /* Added by Esben Soeltoft - 03/09/2016
     Transfer speer: 47.616Bd or 5.95kB/s
     Absolute communication speed: 5.21kB/s
     Data throughput: 4.34kB/s */
    #define BIT_WIDTH 12
    #define BIT_SPACER 36
    #define ACCEPTANCE 12
    #define READ_DELAY 1
  #endif
#endif


#if defined(ESP8266)
  #if MODE == STANDARD
  /* Added by github user 240974a - 03/09/2016  */
    #if F_CPU == 80000000L
      #define BIT_WIDTH  44
      #define BIT_SPACER 110
      #define ACCEPTANCE 35
      #define READ_DELAY 4
    #endif
  #endif

  #if MODE == FAST
    // To be implemented
  #endif

  #if MODE == OVERDRIVE
    // To be implemented
  #endif
#endif
