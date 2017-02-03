
/* General constants predefinition errors: */

#ifdef ACK
  #if (ACK != 6)
    #error ACK constant is set previously to a non compliant value (ACK should be decimal value 6)
  #endif
#endif

#ifdef NAK
  #if (NAK != 21)
    #error NAK constant is set previously to a non compliant value (NAK should be decimal value 21)
  #endif
#endif
