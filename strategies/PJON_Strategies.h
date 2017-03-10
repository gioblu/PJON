
#pragma once

/* Strategy related constant predefinition memory optimization */
#if defined(PJON_INCLUDE_AS)
  #include "AnalogSampling/AnalogSampling.h"
#endif
#if defined(PJON_INCLUDE_ETCP)
  #include "EthernetTCP/EthernetTCP.h"
#endif
#if defined(PJON_INCLUDE_LUPD)
  #include "LocalUDP/LocalUDP.h"
#endif
#if defined(PJON_INCLUDE_OS)
  #include "OverSampling/OverSampling.h"
#endif
#if defined(PJON_INCLUDE_SWBB)
  #include "SoftwareBitBang/SoftwareBitBang.h"
#endif
#if defined(PJON_INCLUDE_TS)
  #include "ThroughSerial/ThroughSerial.h"
#endif
#if defined(PJON_INCLUDE_NONE)
  /* None for custom strategy inclusion */
#endif
#if !defined(PJON_INCLUDE_AS) && !defined(PJON_INCLUDE_ETCP) && \
    !defined(PJON_INCLUDE_LUPD) && !defined(PJON_INCLUDE_OS) && \
    !defined(PJON_INCLUDE_SWBB) && !defined(PJON_INCLUDE_TS) && \
    !defined(PJON_INCLUDE_NONE)
  #include "AnalogSampling/AnalogSampling.h"
  #include "OverSampling/OverSampling.h"
  #include "SoftwareBitBang/SoftwareBitBang.h"
  #include "ThroughSerial/ThroughSerial.h"
  /* Avoid ATtiny 45/85 error missing inclusion error */
  #if !defined(__AVR_ATtiny45__) && !defined(__AVR_ATtiny85__)
    #include "EthernetTCP/EthernetTCP.h"
    #include "LocalUDP/LocalUDP.h"
  #endif
#endif
