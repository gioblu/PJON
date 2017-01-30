
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

#ifdef BROADCAST
  #if (BROADCAST != 0)
    #error BROADCAST constant is set previously to a non compliant value (BROADCAST should be decimal value 0)
  #endif
#endif

#ifdef MASTER_ID
  #if (MASTER_ID != 254)
    #error MASTER_ID constant is set previously to a non compliant value (MASTER_ID should be decimal value 254)
  #endif
#endif

#ifdef NOT_ASSIGNED
  #if (NOT_ASSIGNED != 255)
    #error NOT_ASSIGNED constant is set previously to a non compliant value (NOT_ASSIGNED should be decimal value 255)
  #endif
#endif
