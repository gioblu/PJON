/* digitalWriteFast library

   List of supported MCUs:
     - ATmega8/88/168/328/1280/2560/16U4/32U4 (Duemilanove, Uno, Nano, Mini, Pro, Mega, Leonardo, Micro)
     - ATtiny45/85 (Trinket / Digispark)
     - SAMD21G18A (Arduino Zero) - Added by Esben Soeltoft 03/09/2016

   Copyright 2012-2017 Giovanni Blu Mitolo

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#ifndef __DIGITALWRITEFAST_H_INCLUDED__
  #define __DIGITALWRITEFAST_H_INCLUDED__

  #define BIT_READ(value, bit) (((value) >> (bit)) & 0x01)
  #define BIT_SET(value, bit) ((value) |= (1UL << (bit)))
  #define BIT_CLEAR(value, bit) ((value) &= ~(1UL << (bit)))
  #define BIT_WRITE(value, bit, bitvalue) (bitvalue ? BIT_SET(value, bit) : BIT_CLEAR(value, bit))

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // Arduino Mega Pins
    #define digitalPinToPortReg(P) \
      ((P >= 22 && P <= 29) ? &PORTA : \
      (((P >= 10 && P <= 13) || (P >= 50 && P <= 53)) ? &PORTB : \
      ((P >= 30 && P <= 37) ? &PORTC : \
      (((P >= 18 && P <= 21) || P == 38) ? &PORTD : \
      (((P >= 0 && P <= 3) || P == 5) ? &PORTE : \
      ((P >= 54 && P <= 61) ? &PORTF : \
      (((P >= 39 && P <= 41) || P == 4) ? &PORTG : \
      (((P >= 6 && P <= 9) || P == 16 || P == 17) ? &PORTH : \
      ((P == 14 || P == 15) ? &PORTJ : \
      ((P >= 62 && P <= 69) ? &PORTK : &PORTL))))))))))

    #define digitalPinToDDRReg(P) \
      ((P >= 22 && P <= 29) ? &DDRA : \
      (((P >= 10 && P <= 13) || (P >= 50 && P <= 53)) ? &DDRB : \
      ((P >= 30 && P <= 37) ? &DDRC : \
      (((P >= 18 && P <= 21) || P == 38) ? &DDRD : \
      (((P >= 0 && P <= 3) || P == 5) ? &DDRE : \
      ((P >= 54 && P <= 61) ? &DDRF : \
      (((P >= 39 && P <= 41) || P == 4) ? &DDRG : \
      (((P >= 6 && P <= 9) || P == 16 || P == 17) ? &DDRH : \
      ((P == 14 || P == 15) ? &DDRJ : \
      ((P >= 62 && P <= 69) ? &DDRK : &DDRL))))))))))

    #define digitalPinToPINReg(P) \
      ((P >= 22 && P <= 29) ? &PINA : \
      (((P >= 10 && P <= 13) || (P >= 50 && P <= 53)) ? &PINB : \
      ((P >= 30 && P <= 37) ? &PINC : \
      (((P >= 18 && P <= 21) || P == 38) ? &PIND : \
      (((P >= 0 && P <= 3) || P == 5) ? &PINE : \
      ((P >= 54 && P <= 61) ? &PINF : \
      (((P >= 39 && P <= 41) || P == 4) ? &PING : \
      (((P >= 6 && P <= 9) || P == 16 || P == 17) ? &PINH : \
      ((P == 14 || P == 15) ? &PINJ : \
      ((P >= 62 && P <= 69) ? &PINK : &PINL))))))))))

    #ifndef __digitalPinToBit
      #define __digitalPinToBit(P) \
        ((P >=  7 && P <=  9) ? P - 3 : ((P >= 10 && P <= 13) ? P - 6 : \
        ((P >= 22 && P <= 29) ? P - 22 : ((P >= 30 && P <= 37) ? 37 - P : \
        ((P >= 39 && P <= 41) ? 41 - P : ((P >= 42 && P <= 49) ? 49 - P : \
        ((P >= 50 && P <= 53) ? 53 - P : ((P >= 54 && P <= 61) ? P - 54 : \
        ((P >= 62 && P <= 69) ? P - 62 : ((P == 0 || P == 15 || P == 17 || P == 21) ? 0 : \
        ((P == 1 || P == 14 || P == 16 || P == 20) ? 1 : ((P == 19) ? 2 : \
        ((P == 5 || P == 6 || P == 18) ? 3 : ((P == 2) ? 4 : \
        ((P == 3 || P == 4) ? 5 : 7)))))))))))))))
      #endif

    // 15 PWM
    #define __digitalPinToTimer(P) \
      ((P == 13 || P ==  4) ? &TCCR0A : \
      ((P == 11 || P == 12) ? &TCCR1A : \
      ((P == 10 || P ==  9) ? &TCCR2A : \
      ((P ==  5 || P ==  2 || P ==  3) ? &TCCR3A : \
      ((P ==  6 || P ==  7 || P ==  8) ? &TCCR4A : \
      ((P == 46 || P == 45 || P == 44) ? &TCCR5A : 0))))))

    #define __digitalPinToTimerBit(P) \
      ((P == 13) ? COM0A1 : ((P ==  4) ? COM0B1 : \
      ((P == 11) ? COM1A1 : ((P == 12) ? COM1B1 : \
      ((P == 10) ? COM2A1 : ((P ==  9) ? COM2B1 : \
      ((P ==  5) ? COM3A1 : ((P ==  2) ? COM3B1 : ((P ==  3) ? COM3C1 : \
      ((P ==  6) ? COM4A1 : ((P ==  7) ? COM4B1 : ((P ==  8) ? COM4C1 : \
      ((P == 46) ? COM5A1 : ((P == 45) ? COM5B1 : COM5C1))))))))))))))
  #endif

  #if defined(__AVR_ATmega8__) // PWM ATmega 8
    #define __digitalPinToTimer(P) ((P ==  9 || P == 10) ? &TCCR1A : ((P == 11) ? &TCCR2 : 0))
    #define __digitalPinToTimerBitP ((P ==  9) ? COM1A1 : ((P == 10) ? COM1B1 : COM21))
  #endif

  #if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
    #define digitalPinToPortReg(P) ((P >= 0 && P <= 7) ? &PORTD : ((P >= 8 && P <= 13) ? &PORTB : &PORTC))
    #define digitalPinToDDRReg(P) ((P >= 0 && P <= 7) ? &DDRD : ((P >= 8 && P <= 13) ? &DDRB : &DDRC))
    #define digitalPinToPINReg(P) ((P >= 0 && P <= 7) ? &PIND : ((P >= 8 && P <= 13) ? &PINB : &PINC))
    #ifndef __digitalPinToBit
      #define __digitalPinToBit(P) ((P >= 0 && P <= 7) ? P : ((P >= 8 && P <= 13) ? P - 8 : P - 14))
    #endif

    #define __digitalPinToTimer(P) \
      ((P == 6 || P == 5) ? &TCCR0A : ((P == 9 || P == 10) ? \
      &TCCR1A : ((P == 11 || P == 3) ? &TCCR2A : 0)))
    #define __digitalPinToTimerBit(P) \
      ((P == 6) ? COM0A1 : ((P == 5) ? COM0B1 : ((P == 9) ? \
      COM1A1 : ((P == 10) ? COM1B1 : ((P == 11) ? COM2A1 : COM2B1)))))
  #endif

  #if defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
    #define digitalPinToPortReg(P) \
      (((P >= 0 && P <= 4) || P == 6 || P == 12 || P == 24 || P == 25 || P == 29) ? &PORTD : \
      ((P == 5 || P == 13) ? &PORTC : ((P >= 18 && P <= 23)) ? &PORTF : ((P == 7) ? &PORTE : &PORTB)))
    #define digitalPinToDDRReg(P) \
      (((P >= 0 && P <= 4) || P == 6 || P == 12 || P == 24 || P == 25 || P == 29) ? &DDRD : \
      ((P == 5 || P == 13) ? &DDRC : ((P >= 18 && P <= 23)) ? &DDRF : ((P == 7) ? &DDRE : &DDRB)))
    #define digitalPinToPINReg(P) \
      (((P >= 0 && P <= 4) || P == 6 || P == 12 || P == 24 || P == 25 || P == 29) ? &PIND : \
      ((P == 5 || P == 13) ? &PINC : ((P >= 18 && P <= 23)) ? &PINF : ((P == 7) ? &PINE : &PINB)))
    #ifndef __digitalPinToBit
      #define __digitalPinToBit(P) \
        ((P >= 8 && P <= 11) ? P - 4 : ((P >= 18 && P <= 21) ? 25 - P : \
        ((P == 0) ? 2 : ((P == 1) ? 3 : ((P == 2) ? 1 : ((P == 3) ? 0 : \
        ((P == 4) ? 4 : ((P == 6) ? 7 : ((P == 13) ? 7 : ((P == 14) ? 3 : \
        ((P == 15) ? 1 : ((P == 16) ? 2 : ((P == 17) ? 0 : ((P == 22) ? 1 : \
        ((P == 23) ? 0 : ((P == 24) ? 4 : ((P == 25) ? 7 : ((P == 26) ? 4 : \
        ((P == 27) ? 5 : 6 )))))))))))))))))))
    #endif
  #endif

  #if defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    #define digitalPinToPortReg(P) &PORTB
    #define digitalPinToDDRReg(P)  &DDRB
    #define digitalPinToPINReg(P)  &PINB
    #ifndef __digitalPinToBit
      #define __digitalPinToBit(P)   P - 6
    #endif
  #endif

  #if defined(__SAMD21G18A__) || defined(ARDUINO_SAM_ZERO)  // Arduino Zero pins
    #define __digitalPinToPortBit(P) \
      ((P == 0) ? PORT_PA11  : ((P == 1) ? PORT_PA10  : ((P == 2) ? PORT_PA14  : \
      ((P == 3) ? PORT_PA09  : ((P == 4) ? PORT_PA08  : ((P == 5) ? PORT_PA15  : \
      ((P == 6) ? PORT_PA20  : ((P == 7) ? PORT_PA21  : ((P == 8) ? PORT_PA06  : \
      ((P == 9) ? PORT_PA07  : ((P == 10) ? PORT_PA18 : ((P == 11) ? PORT_PA16 : \
      ((P == 12) ? PORT_PA19 : ((P == 13) ? PORT_PA17 : ((P == A0) ? PORT_PA02 : \
      ((P == A1) ? PORT_PB08 : ((P == A2) ? PORT_PB09 : ((P == A3) ? PORT_PA04 : \
      ((P == A4) ? PORT_PA05 : ((P == A5) ? PORT_PB02 : ((P == SCK) ? PORT_PB11  : \
      ((P == MISO) ? PORT_PA12 : ((P == MOSI) ? PORT_PB10 : ((P == PIN_WIRE_SCL) ? PORT_PA23 : \
      ((P == PIN_WIRE_SDA) ? PORT_PA22 : PORT_PA13)))))))))))))))))))))))))

    #define __digitalPinToPortReg(P) ((P >= 15 && P <= 16) || (P == 19 ) || (P >= 23 && P <= 24) ? PORTB : PORTA )
    #define __digitalPinToPortRegOUT(P) ((P >= 15 && P <= 16) || (P == 19 ) || (P >= 23 && P <= 24) ? REG_PORT_OUT1 : REG_PORT_OUT0 )
    #define __digitalPinToPortRegOUTSET(P) ((P >= 15 && P <= 16) || (P == 19 ) || (P >= 23 && P <= 24) ? REG_PORT_OUTSET1 : REG_PORT_OUTSET0 )
    #define __digitalPinToPortRegOUTCLR(P) ((P >= 15 && P <= 16) || (P == 19 ) || (P >= 23 && P <= 24) ? REG_PORT_OUTCLR1 : REG_PORT_OUTCLR0 )
    #define __digitalPinToPortRegPINCFG(P) ((P >= 15 && P <= 16) || (P == 19 ) || (P >= 23 && P <= 24) ? REG_PORT_PINCFG1 : REG_PORT_PINCFG0 )
    #define __digitalPinToPortRegDIRSET(P) ((P >= 15 && P <= 16) || (P == 19 ) || (P >= 23 && P <= 24) ? REG_PORT_DIRSET1 : REG_PORT_DIRSET0 )
    #define __digitalPinToPortRegDIRCLR(P) ((P >= 15 && P <= 16) || (P == 19 ) || (P >= 23 && P <= 24) ? REG_PORT_DIRCLR1 : REG_PORT_DIRCLR0 )
    #define __digitalPinToPortINReg(P) ((P >= 15 && P <= 16) || (P == 19 ) || (P >= 23 && P <= 24) ? REG_PORT_IN1 : REG_PORT_IN0 )

    /* functions for read/write/pinmode fast for SAMD chips
     PWM pins 3, 4, 5, 6, 8, 9, 10, 11, 12, 13 provide 8-bit PWM output with the analogWrite() function
     analogWrite works on all analog pins and all digital PWM pins. You can supply it any value between 0 and 255.
     analog pins provide 12bits resolution and provide a value between 0-4096.
     INPUT (0x0), OUTPUT (0x1), INPUT_PULLUP (0x2) Set pin to input mode with pull-up resistor enabled, INPUT_PULLDOWN (0x3) */

    #define pinModeFast(P, V) \
      do { if (__builtin_constant_p(P) && __builtin_constant_p(V)) { \
          if(V == 0x0) { \
            __digitalPinToPortRegPINCFG(P) = (uint8_t)(PORT_PINCFG_INEN); \
            __digitalPinToPortRegDIRCLR(P) = (uint32_t)__digitalPinToPortBit(P); \
          } else if(V == 0x2) { \
            PORT->Group[__digitalPinToPortReg(11)].PINCFG[g_APinDescription[11].ulPin].reg = (uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN); \
            __digitalPinToPortRegDIRCLR(P) = (uint32_t)__digitalPinToPortBit(P); \
            /* Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.7 Data Output Value Set')*/ \
            __digitalPinToPortRegOUTSET(P) = (uint32_t)__digitalPinToPortBit(P); \
          } else if(V == 0x3) { \
            PORT->Group[g_APinDescription[11].ulPort].PINCFG[g_APinDescription[11].ulPin].reg = (uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN) ; \
            __digitalPinToPortRegDIRCLR(P) = (uint32_t)__digitalPinToPortBit(P); \
            /* Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.6 Data Output Value Clear') */  \
            __digitalPinToPortRegOUTCLR(P) = (uint32_t)__digitalPinToPortBit(P); \
          } else { \
            __digitalPinToPortRegPINCFG(P) &= ~(uint8_t)(PORT_PINCFG_INEN); \
            __digitalPinToPortRegDIRSET(P) = (uint32_t)__digitalPinToPortBit(P); \
          } \
        } else pinMode(P, V); \
      } while (0)

    #define digitalReadFast(P) ( (int) _digitalReadFast_(P) )
    #define _digitalReadFast_(P) (__builtin_constant_p(P) ) ? ( __digitalPinToPortINReg(P) & __digitalPinToPortBit(P) ) : digitalRead(P)

    #define digitalWriteFast(P, V) \
      do { if (__builtin_constant_p(P) && __builtin_constant_p(V)) { \
          if(V) __digitalPinToPortRegOUTSET(P) = __digitalPinToPortBit(P); \
          else __digitalPinToPortRegOUTCLR(P) = __digitalPinToPortBit(P); \
        } else digitalWrite(P, V); \
      } while(0)

    #define pullDownFast(P) \
      do { if(__builtin_constant_p(P)) { \
          pinModeFast(P, INPUT_PULLDOWN); \
        } else pinMode(P, INPUT_PULLDOWN); \
      } while(0)
  #endif

  #ifdef __AVR__
    #define __atomicWrite__(A, P, V) \
      if ( (int)(A) < 0x40) { bitWrite(*(A), __digitalPinToBit(P), V); }  \
      else { \
        uint8_t register saveSreg = SREG; \
        cli(); \
        bitWrite(*(A), __digitalPinToBit(P), V); \
        SREG = saveSreg; \
      }

    #ifndef digitalWriteFast
      #define digitalWriteFast(P, V) \
        do { if(__builtin_constant_p(P) && __builtin_constant_p(V)) __atomicWrite__((uint8_t*) digitalPinToPortReg(P), P, V) \
          else digitalWrite(P, V); \
        } while(0)
    #endif

    #if !defined(pinModeFast)
      #define pinModeFast(P, V) \
      do { if(__builtin_constant_p(P) && __builtin_constant_p(V)) __atomicWrite__((uint8_t*) digitalPinToDDRReg(P), P, V) \
        else pinMode(P, V); \
      } while(0)
    #endif

    #ifndef noAnalogWrite
      #define noAnalogWrite(P) \
      	do { if(__builtin_constant_p(P) )  __atomicWrite((uint8_t*) __digitalPinToTimer(P), P, 0) \
      		else turnOffPWM(P); \
        } while(0)
    #endif

    #ifndef digitalReadFast
    	#define digitalReadFast(P) ((int) _digitalReadFast_(P))
    	#define _digitalReadFast_(P) \
      	(__builtin_constant_p(P)) ? ( \
      	(BIT_READ(*digitalPinToPINReg(P), __digitalPinToBit(P)))) : \
      	digitalRead(P)
    #endif

    #define pullDownFast(P) \
      do { if(__builtin_constant_p(P)) { \
          digitalWriteFast(P, LOW); \
          pinModeFast(P, INPUT); \
        } else \
          digitalWrite(P, LOW); \
          pinMode(P, INPUT); \
      } while(0)

  #else
    #if !defined(digitalWriteFast)
      #define digitalWriteFast digitalWrite
    #endif

    #if !defined(digitalReadFast)
      #define digitalReadFast digitalRead
    #endif

    #if !defined(pinModeFast)
      #define pinModeFast pinMode
    #endif

    #if !defined(pullDownFast)
      #define pullDownFast(P) \
        do { \
          digitalWrite(P, LOW); \
          pinMode(P, INPUT); \
        } while(0)
    #endif
  #endif
#endif
