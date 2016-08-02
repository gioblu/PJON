
/* PJON v4.0 OverSampling strategy Transmission Timing table
   Copyright (c) 2016, Giovanni Blu Mitolo All rights reserved.

   All benchmarks should be executed with NetworkAnalysis and SpeedTest examples.

   The following constants setup is quite conservative and determined only
   with a huge amount of time and blind testing (without oscilloscope)
   tweaking values and analysing results. Theese can be changed to obtain
   faster speed. Probably you need experience, time and an oscilloscope. */

#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #if _OS_MODE == _STXRX882_STANDARD
    #if F_CPU == 16000000L
      #define _OS_BIT_WIDTH 512
      #define _OS_BIT_SPACER 328
    #endif
  #endif
#endif

#ifndef _OS_BIT_WIDTH
  #define _OS_BIT_WIDTH  512
#endif

#ifndef _OS_BIT_SPACER
  #define _OS_BIT_SPACER 328
#endif
