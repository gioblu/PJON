
/* Updated version of digitalWriteFast library with ATmega 1280/2560 and ATtiny25/45/85 support
   Copyright (c) 2012-2015, Giovanni Blu Mitolo All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
      
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
      
    - All advertising materials mentioning features or use of this software
      must display the following acknowledgement:
      "This product includes digitalWriteFast software developed by Giovanni Blu Mitolo."
      
    - Neither the name of PJON, PJON_ASK, digitalWriteFast nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
   
   This software is provided by the copyright holders and contributors"as is"
   and any express or implied warranties, including, but not limited to, the
   implied warranties of merchantability and fitness for a particular purpose
   are disclaimed. In no event shall the copyright holder or contributors be
   liable for any direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute goods or services;
   loss of use, data, or profits; or business interruption) however caused and on any
   theory of liability, whether in contract, strict liability, or tort (including
   negligence or otherwise) arising in any way out of the use of this software, even if
   advised of the possibility of such damage. */

/* 2016-03-09 update by Esben Soeltoft
   This update includes:
   
    - support for the SAMD Arduino Zero from arduino.cc
      has been able to achieve the following speed by doing the relevant function call
      500 times and comparing the time before and after.
        Time for digitalWrite(): 1282
        Time for digitalWriteFast(): 90
        Time for digitalRead(): 400
        Time for digitalReadFast(): 67
        Time for pinMode(  ,INPUT): 926
        Time for pinMode(  ,INPUT_PULLDOWN): 1033
        Time for pinModeFast(  ,INPUT): 91
        Time for pinModeFast(  ,INPUT_PULLDOWN): 308
     
      It is worth observing that normal INPUT is more than 3 times faster than INPUT_PULLDOWN
      
    - removed local BITREAD function to utilize bitRead() function from Arduino.h include
    
    - the method for INPUT_PULLDOWN and INPUT_PULLUP was changed from digitalWriteFast to
      pinModeFast to function according to original arduino functions.

    - Added include guard for this library
    
    - Added a library comment explaining when the library can boost performance.

   TO-DO
    - modify pinModeFast for AVR to set INPUT_PULLDOWN and INPUT_PULLUP correctly instead
      of in digitalWriteFast.
      
    - rewrite pinModeFast for SAMD to improve the read-write operations speed on the registers
    
*/
   


//==================================================================
// include guard
#ifndef __DIGITALWRITEFAST_H_INCLUDED__
#define __DIGITALWRITEFAST_H_INCLUDED__

/* What can this library accomplish
   If a pin number is known at compile time, it can speed up digital write and read functions.
   e.g. that digitalWrite(11,HIGH) is speeded up by using digitalWriteFast(11,HIGH). 
   On the other hand if digitalWrite is called like this digitalWrite(i,HIGH) such as in a loop
   or in a function call with the pin number as a passed argument digitalWriteFast(i,HIGH) will 
   not be faster, as it will need to fall back to the normal digitalWrite() funtion.
 */

//==================================================================
// include arduino header files to get bitRead included
  #if defined(ARDUINO) && (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
    #include <wiring.h>
  #endif


  #if defined(__SAMD21G18A__) || defined(ARDUINO_SAM_ZERO)
    //==================================================================
    // pin mapping for SAMD chips
  
    // Arduino Zero pins
    //D0 - PORT_PA11
    //D1 - PORT_PA10
    //D2 - PORT_PA14   
    //D3 - PORT_PA09
    //D4 - PORT_PA08   
    //D5 - PORT_PA15
    //D6 - PORT_PA20
    //D7 - PORT_PA21
    //D8 - PORT_PA06
    //D9 - PORT_PA07
    //D10 - PORT_PA18
    //D11 - PORT_PA16
    //D12 - PORT_PA19
    //D13 - PORT_PA17

    //A0 - PORT_PA02
    //A1 - PORT_PB08
    //A2 - PORT_PB09
    //A3 - PORT_PA04
    //A4 - PORT_PA05
    //A5 - PORT_PB02

    //SCK - PORT_PB11
    //MISO - PORT_PA12
    //MOSI - PORT_PB10

    //SCL - PORT_PB23
    //SDA - PORT_PB22

    //ATN - PORT_PA13    


    #define __digitalPinToPortBit(P) \
    (((P) == 0) ? PORT_PA11  : \
    (((P) == 1) ? PORT_PA10  : \
    (((P) == 2) ? PORT_PA14  : \
    (((P) == 3) ? PORT_PA09  : \
    (((P) == 4) ? PORT_PA08  : \
    (((P) == 5) ? PORT_PA15  : \
    (((P) == 6) ? PORT_PA20  : \
    (((P) == 7) ? PORT_PA21  : \
    (((P) == 8) ? PORT_PA06  : \
    (((P) == 9) ? PORT_PA07  : \
    (((P) == 10) ? PORT_PA18 : \
    (((P) == 11) ? PORT_PA16 : \
    (((P) == 12) ? PORT_PA19 : \
    (((P) == 13) ? PORT_PA17 : \
    (((P) == A0) ? PORT_PA02 : \
    (((P) == A1) ? PORT_PB08 : \
    (((P) == A2) ? PORT_PB09 : \
    (((P) == A3) ? PORT_PA04 : \
    (((P) == A4) ? PORT_PA05 : \
    (((P) == A5) ? PORT_PB02 : \
    (((P) == SCK) ? PORT_PB11  : \
    (((P) == MISO) ? PORT_PA12 : \
    (((P) == MOSI) ? PORT_PB10 : \
    (((P) == PIN_WIRE_SCL) ? PORT_PA23 : \
    (((P) == PIN_WIRE_SDA) ? PORT_PA22 : PORT_PA13)))))))))))))))))))))))))

    #define __digitalPinToPortReg(P) \
    (((P) >= 15 && (P) <= 16) || ((P) == 19 ) || ((P) >= 23 && (P) <= 24) ? PORTB : PORTA )

    #define __digitalPinToPortRegOUT(P) \
    (((P) >= 15 && (P) <= 16) || ((P) == 19 ) || ((P) >= 23 && (P) <= 24) ? REG_PORT_OUT1 : REG_PORT_OUT0 )

    #define __digitalPinToPortRegOUTSET(P) \
    (((P) >= 15 && (P) <= 16) || ((P) == 19 ) || ((P) >= 23 && (P) <= 24) ? REG_PORT_OUTSET1 : REG_PORT_OUTSET0 )

    #define __digitalPinToPortRegOUTCLR(P) \
    (((P) >= 15 && (P) <= 16) || ((P) == 19 ) || ((P) >= 23 && (P) <= 24) ? REG_PORT_OUTCLR1 : REG_PORT_OUTCLR0 )

    #define __digitalPinToPortRegPINCFG(P) \
    (((P) >= 15 && (P) <= 16) || ((P) == 19 ) || ((P) >= 23 && (P) <= 24) ? REG_PORT_PINCFG1 : REG_PORT_PINCFG0 )

    #define __digitalPinToPortRegDIRSET(P) \
    (((P) >= 15 && (P) <= 16) || ((P) == 19 ) || ((P) >= 23 && (P) <= 24) ? REG_PORT_DIRSET1 : REG_PORT_DIRSET0 )

    #define __digitalPinToPortRegDIRCLR(P) \
    (((P) >= 15 && (P) <= 16) || ((P) == 19 ) || ((P) >= 23 && (P) <= 24) ? REG_PORT_DIRCLR1 : REG_PORT_DIRCLR0 )

    #define __digitalPinToPortINReg(P) \
    (((P) >= 15 && (P) <= 16) || ((P) == 19 ) || ((P) >= 23 && (P) <= 24) ? REG_PORT_IN1 : REG_PORT_IN0 )


   
    
    //==================================================================
    // functions for read/write/pinmode fast for SAMD chips

    // PWM pins 3, 4, 5, 6, 8, 9, 10, 11, 12, 13 provide 8-bit PWM output with the analogWrite() function
    // analogWrite works on all analog pins and all digital PWM pins. You can supply it any value between 0 and 255.
    // analog pins provide 12bits resolution and provide a value between 0-4096.


    /*
     * INPUT           (0x0)
     * OUTPUT          (0x1)
     * INPUT_PULLUP    (0x2) Set pin to input mode with pull-up resistor enabled
     * INPUT_PULLDOWN  (0x3)
     */
    
    #define pinModeFast(P, V)                                                                                 \
      do {                                                                                                    \
        if (__builtin_constant_p(P) && __builtin_constant_p(V)){                                              \
          if((V == 0x0)){                                                                                     \
            __digitalPinToPortRegPINCFG(P) = (uint8_t)(PORT_PINCFG_INEN) ;                                    \
            __digitalPinToPortRegDIRCLR(P) = (uint32_t)__digitalPinToPortBit(P) ;                             \
          }else if((V == 0x2)){                                                                               \
            PORT->Group[__digitalPinToPortReg(11)].PINCFG[g_APinDescription[11].ulPin].reg=(uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN) ;  \
            __digitalPinToPortRegDIRCLR(P) = (uint32_t)__digitalPinToPortBit(P) ;                             \
            /* Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.7 Data Output Value Set')*/     \
            __digitalPinToPortRegOUTSET(P) = (uint32_t)__digitalPinToPortBit(P) ;                             \
          }else if((V == 0x3)){                                                                               \
            PORT->Group[g_APinDescription[11].ulPort].PINCFG[g_APinDescription[11].ulPin].reg=(uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN) ; \
            __digitalPinToPortRegDIRCLR(P) = (uint32_t)__digitalPinToPortBit(P) ;                             \
            /* Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.6 Data Output Value Clear') */  \
            __digitalPinToPortRegOUTCLR(P) = (uint32_t)__digitalPinToPortBit(P) ;                             \
          }else{                                                                                              \
            __digitalPinToPortRegPINCFG(P) &= ~(uint8_t)(PORT_PINCFG_INEN) ;                                  \
            __digitalPinToPortRegDIRSET(P) = (uint32_t)__digitalPinToPortBit(P) ;                             \
          }                                                                                                   \
        }else                                                                                                 \
          pinMode((P), (V));                                                                                  \
      }while (0)


    #define digitalReadFast(P) ( (int) _digitalReadFast_((P)) )
	  #define _digitalReadFast_(P) (__builtin_constant_p(P) ) ? ( __digitalPinToPortINReg(P) & __digitalPinToPortBit(P) ) : digitalRead((P))


    #define digitalWriteFast(P, V)                                        \
      do {                                                                \
        if (__builtin_constant_p(P) && __builtin_constant_p(V)){          \
          if((V))                                                         \
            __digitalPinToPortRegOUTSET(P) = __digitalPinToPortBit(P);    \
          else                                                            \
            __digitalPinToPortRegOUTCLR(P) = __digitalPinToPortBit(P);    \
        }else                                                             \
          digitalWrite((P), (V));                                         \
      }while (0)


    // Arduino Zero done


  #else
    //==================================================================
    // pin mapping for AVR chips
      
    #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
      // Arduino Mega pins
      #define digitalPinToPortReg(P) \
      (((P) >= 22 && (P) <= 29) ? &PORTA : \
      ((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PORTB : \
      (((P) >= 30 && (P) <= 37) ? &PORTC : \
      ((((P) >= 18 && (P) <= 21) || (P) == 38) ? &PORTD : \
      ((((P) >= 0 && (P) <= 3) || (P) == 5) ? &PORTE : \
      (((P) >= 54 && (P) <= 61) ? &PORTF : \
      ((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PORTG : \
      ((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PORTH : \
      (((P) == 14 || (P) == 15) ? &PORTJ : \
      (((P) >= 62 && (P) <= 69) ? &PORTK : &PORTL))))))))))  

      #define digitalPinToDDRReg(P) \
      (((P) >= 22 && (P) <= 29) ? &DDRA : \
      ((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &DDRB : \
      (((P) >= 30 && (P) <= 37) ? &DDRC : \
      ((((P) >= 18 && (P) <= 21) || (P) == 38) ? &DDRD : \
      ((((P) >= 0 && (P) <= 3) || (P) == 5) ? &DDRE : \
      (((P) >= 54 && (P) <= 61) ? &DDRF : \
      ((((P) >= 39 && (P) <= 41) || (P) == 4) ? &DDRG : \
      ((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &DDRH : \
      (((P) == 14 || (P) == 15) ? &DDRJ : \
      (((P) >= 62 && (P) <= 69) ? &DDRK : &DDRL))))))))))

      #define digitalPinToPINReg(P) \
      (((P) >= 22 && (P) <= 29) ? &PINA : \
      ((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PINB : \
      (((P) >= 30 && (P) <= 37) ? &PINC : \
      ((((P) >= 18 && (P) <= 21) || (P) == 38) ? &PIND : \
      ((((P) >= 0 && (P) <= 3) || (P) == 5) ? &PINE : \
      (((P) >= 54 && (P) <= 61) ? &PINF : \
      ((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PING : \
      ((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PINH : \
      (((P) == 14 || (P) == 15) ? &PINJ : \
      (((P) >= 62 && (P) <= 69) ? &PINK : &PINL))))))))))

      #define __digitalPinToBit(P) \
      (((P) >=  7 && (P) <=  9) ? (P) - 3 : \
      (((P) >= 10 && (P) <= 13) ? (P) - 6 : \
      (((P) >= 22 && (P) <= 29) ? (P) - 22 : \
      (((P) >= 30 && (P) <= 37) ? 37 - (P) : \
      (((P) >= 39 && (P) <= 41) ? 41 - (P) : \
      (((P) >= 42 && (P) <= 49) ? 49 - (P) : \
      (((P) >= 50 && (P) <= 53) ? 53 - (P) : \
      (((P) >= 54 && (P) <= 61) ? (P) - 54 : \
      (((P) >= 62 && (P) <= 69) ? (P) - 62 : \
      (((P) == 0 || (P) == 15 || (P) == 17 || (P) == 21) ? 0 : \
      (((P) == 1 || (P) == 14 || (P) == 16 || (P) == 20) ? 1 : \
      (((P) == 19) ? 2 : \
      (((P) == 5 || (P) == 6 || (P) == 18) ? 3 : \
      (((P) == 2) ? 4 : \
      (((P) == 3 || (P) == 4) ? 5 : 7)))))))))))))))

      // 15 PWM pins
      #define __digitalPinToTimer(P) \
      (((P) == 13 || (P) ==  4) ? &TCCR0A : \
      (((P) == 11 || (P) == 12) ? &TCCR1A : \
      (((P) == 10 || (P) ==  9) ? &TCCR2A : \
      (((P) ==  5 || (P) ==  2 || (P) ==  3) ? &TCCR3A : \
      (((P) ==  6 || (P) ==  7 || (P) ==  8) ? &TCCR4A : \
      (((P) == 46 || (P) == 45 || (P) == 44) ? &TCCR5A : 0))))))
      #define __digitalPinToTimerBit(P) \
      (((P) == 13) ? COM0A1 : (((P) ==  4) ? COM0B1 : \
      (((P) == 11) ? COM1A1 : (((P) == 12) ? COM1B1 : \
      (((P) == 10) ? COM2A1 : (((P) ==  9) ? COM2B1 : \
      (((P) ==  5) ? COM3A1 : (((P) ==  2) ? COM3B1 : (((P) ==  3) ? COM3C1 : \
      (((P) ==  6) ? COM4A1 : (((P) ==  7) ? COM4B1 : (((P) ==  8) ? COM4C1 : \
      (((P) == 46) ? COM5A1 : (((P) == 45) ? COM5B1 : COM5C1))))))))))))))
      
      // end Arduino Mega
    #else
      // Standard Arduino Pins
      #define digitalPinToPortReg(P) \
      (((P) >= 0 && (P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : &PORTC))
      #define digitalPinToDDRReg(P) \
      (((P) >= 0 && (P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : &DDRC))
      #define digitalPinToPINReg(P) \
      (((P) >= 0 && (P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : &PINC))
      #define __digitalPinToBit(P) \
      (((P) >= 0 && (P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P) - 8 : (P) - 14))

      #if defined(__AVR_ATmega8__)
        // 3 PWM pins
        #define __digitalPinToTimer(P) \
        (((P) ==  9 || (P) == 10) ? &TCCR1A : (((P) == 11) ? &TCCR2 : 0))
        #define __digitalPinToTimerBit(P) \
        (((P) ==  9) ? COM1A1 : (((P) == 10) ? COM1B1 : COM21))
        #else  //168,328

        // 6 PWM pins
        #define __digitalPinToTimer(P) \
        (((P) ==  6 || (P) ==  5) ? &TCCR0A : \
        (((P) ==  9 || (P) == 10) ? &TCCR1A : \
        (((P) == 11 || (P) ==  3) ? &TCCR2A : 0)))
        #define __digitalPinToTimerBit(P) \
        (((P) ==  6) ? COM0A1 : (((P) ==  5) ? COM0B1 : \
        (((P) ==  9) ? COM1A1 : (((P) == 10) ? COM1B1 : \
        (((P) == 11) ? COM2A1 : COM2B1)))))
        
        
      #endif // end defined(__AVR_ATmega8__)
    #endif // end Standard Arduino pins

    //==================================================================
    // functions to read/write/pinMode fast for AVR chips
    #define __atomicWrite__(A,P,V) \
    if ( (int)(A) < 0x40) { bitWrite(*(A), __digitalPinToBit(P), (V) );}  \
    else {                                                                \
      uint8_t register saveSreg = SREG;                                   \
      cli();                                                              \
      bitWrite(*(A), __digitalPinToBit(P), (V) );                         \
      SREG=saveSreg;                                                      \
    } 


    #ifndef digitalWriteFast
    #define digitalWriteFast(P, V)                                        \
      do {                                                                \
        if (__builtin_constant_p(P) && __builtin_constant_p(V))           \
          __atomicWrite__((uint8_t*) digitalPinToPortReg(P),P,V)          \
        else                                                              \
          digitalWrite((P), (V));                                         \
      }while (0)
    #endif  //#ifndef digitalWriteFast

    #if !defined(pinModeFast)
    #define pinModeFast(P, V) \
      do {  \
        if (__builtin_constant_p(P) && __builtin_constant_p(V)) __atomicWrite__((uint8_t*) digitalPinToDDRReg(P),P,V) \
        else pinMode((P), (V)); \
      } while (0)
    #endif  //#ifndef pinModeFast


    #ifndef noAnalogWrite
    #define noAnalogWrite(P) \
	    do {  \
        if (__builtin_constant_p(P) )  __atomicWrite((uint8_t*) __digitalPinToTimer(P),P,0) \
		    else turnOffPWM((P));   \
    } while (0)
    #endif  //#ifndef noAnalogWrite		


    #ifndef digitalReadFast
	    #define digitalReadFast(P) ( (int) _digitalReadFast_((P)) )
	    #define _digitalReadFast_(P ) \
	    (__builtin_constant_p(P) ) ? ( \
	    ( bitRead(*digitalPinToPINReg(P), __digitalPinToBit(P))) ) : \
	    digitalRead((P))
    #endif  //#ifndef digitalReadFast

  #endif


#endif  // __DIGITALWRITEFAST_H_INCLUDED__
