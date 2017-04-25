
/* Faster digitalWrite, digitalRead, pinMode for Arduino
   https://codebender.cc/library/digitalWriteFast#bonus%2FdigitalWrite%2Fdigital_write_macros.h
   - Basic scheme was developed by Paul Stoffregen with digitalWrite.
   - Extended to pinMode by John Raines
   - Extended to digitalRead by John Raines with considerable assistance
     by William Westfield
  Copyright (c) 2008-2010 PJRC.COM, LLC

  List of supported MCUs:
  - ATmega8/88/168/328/1280/2560 (Duemilanove, Uno, Nano, Mini, Pro, Mega)
      Added by Giovanni Blu Mitolo

  - ATmega16U4/32U4 (Leonardo, Micro)
      Added by Giovanni Blu Mitolo

  - ATtiny45/85 (Trinket, Digispark)
      Added by Giovanni Blu Mitolo

  - SAMD21G18A (Arduino Zero)
      Added by Esben Soeltoft 03/09/2016

Renamed since v7.0 to avoid naming collisions and so subtle bugs and anomalies
if used along with third-party software using older/different versions of
digitalWriteFast. All methods are defined in uppercase style to implicitly
inform the reader of their definition as macros in the global scope.
 ______________________________________________________________________________

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE. */

#pragma once

/* AVR ATmega1280/2560 - Arduino Mega ------------------------------------- */

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define PJON_IO_PIN_TO_PORT_REG(P) \
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

  #define PJON_IO_PIN_TO_DDR_REG(P) \
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

  #define PJON_IO_PIN_TO_PIN_REG(P) \
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

  #ifndef PJON_IO_PIN_TO_BIT
    #define PJON_IO_PIN_TO_BIT(P) \
      ((P >=  7 && P <=  9) ? P - 3 : ((P >= 10 && P <= 13) ? P - 6 : \
      ((P >= 22 && P <= 29) ? P - 22 : ((P >= 30 && P <= 37) ? 37 - P : \
      ((P >= 39 && P <= 41) ? 41 - P : ((P >= 42 && P <= 49) ? 49 - P : \
      ((P >= 50 && P <= 53) ? 53 - P : ((P >= 54 && P <= 61) ? P - 54 : \
      ((P >= 62 && P <= 69) ? P - 62 : ((P == 0 || P == 15 || P == 17 || P == 21) \
      ? 0 : ((P == 1 || P == 14 || P == 16 || P == 20) ? 1 : ((P == 19) ? 2 : \
      ((P == 5 || P == 6 || P == 18) ? 3 : ((P == 2) ? 4 : \
      ((P == 3 || P == 4) ? 5 : 7)))))))))))))))
    #endif

  // 15 PWM
  #define PJON_IO_PIN_TO_TIMER(P) \
    ((P == 13 || P ==  4) ? &TCCR0A : \
    ((P == 11 || P == 12) ? &TCCR1A : \
    ((P == 10 || P ==  9) ? &TCCR2A : \
    ((P ==  5 || P ==  2 || P ==  3) ? &TCCR3A : \
    ((P ==  6 || P ==  7 || P ==  8) ? &TCCR4A : \
    ((P == 46 || P == 45 || P == 44) ? &TCCR5A : 0))))))

  #define PJON_IO_PIN_TO_TIMER_BIT(P) \
    ((P == 13) ? COM0A1 : ((P ==  4) ? COM0B1 : \
    ((P == 11) ? COM1A1 : ((P == 12) ? COM1B1 : \
    ((P == 10) ? COM2A1 : ((P ==  9) ? COM2B1 : \
    ((P ==  5) ? COM3A1 : ((P ==  2) ? COM3B1 : ((P ==  3) ? COM3C1 : \
    ((P ==  6) ? COM4A1 : ((P ==  7) ? COM4B1 : ((P ==  8) ? COM4C1 : \
    ((P == 46) ? COM5A1 : ((P == 45) ? COM5B1 : COM5C1))))))))))))))
#endif

/* AVR ATmega8 ------------------------------------------------------------ */

#if defined(__AVR_ATmega8__)
  #define PJON_IO_PIN_TO_TIMER(P) \
    ((P ==  9 || P == 10) ? &TCCR1A : ((P == 11) ? &TCCR2 : 0))
  #define PJON_IO_PIN_TO_TIMER_BIT(P) \
    ((P ==  9) ? COM1A1 : ((P == 10) ? COM1B1 : COM21))
#endif

/* AVR ATmega88/168/328/328P - Arduino Duemilanove, Uno, Nano, Mini, Pro -- */

#if defined(__AVR_ATmega88__)  || defined(__AVR_ATmega168__) || \
    defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  #define PJON_IO_PIN_TO_PORT_REG(P) \
    ((P >= 0 && P <= 7) ? &PORTD : ((P >= 8 && P <= 13) ? &PORTB : &PORTC))
  #define PJON_IO_PIN_TO_DDR_REG(P) \
    ((P >= 0 && P <= 7) ? &DDRD : ((P >= 8 && P <= 13) ? &DDRB : &DDRC))
  #define PJON_IO_PIN_TO_PIN_REG(P) \
    ((P >= 0 && P <= 7) ? &PIND : ((P >= 8 && P <= 13) ? &PINB : &PINC))
  #ifndef PJON_IO_PIN_TO_BIT
    #define PJON_IO_PIN_TO_BIT(P) \
      ((P >= 0 && P <= 7) ? P : ((P >= 8 && P <= 13) ? P - 8 : P - 14))
  #endif

  #define PJON_IO_PIN_TO_TIMER(P) \
    ((P == 6 || P == 5) ? &TCCR0A : ((P == 9 || P == 10) ? \
    &TCCR1A : ((P == 11 || P == 3) ? &TCCR2A : 0)))
  #define PJON_IO_PIN_TO_TIMER_BIT(P) \
    ((P == 6) ? COM0A1 : ((P == 5) ? COM0B1 : ((P == 9) ? \
    COM1A1 : ((P == 10) ? COM1B1 : ((P == 11) ? COM2A1 : COM2B1)))))
#endif

/* AVR ATmega16U4/32U4 - Arduino Leonardo, Micro -------------------------- */

#if defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
  #define PJON_IO_PIN_TO_PORT_REG(P) \
    (((P >= 0 && P <= 4) || P == 6 || P == 12 || P == 24 || P == 25 || P == 29) \
    ? &PORTD : ((P == 5 || P == 13) ? &PORTC : ((P >= 18 && P <= 23)) ? &PORTF : \
    ((P == 7) ? &PORTE : &PORTB)))
  #define PJON_IO_PIN_TO_DDR_REG(P) \
    (((P >= 0 && P <= 4) || P == 6 || P == 12 || P == 24 || P == 25 || P == 29) \
    ? &DDRD : ((P == 5 || P == 13) ? &DDRC : ((P >= 18 && P <= 23)) ? \
    &DDRF : ((P == 7) ? &DDRE : &DDRB)))
  #define PJON_IO_PIN_TO_PIN_REG(P) \
    (((P >= 0 && P <= 4) || P == 6 || P == 12 || P == 24 || P == 25 || P == 29) \
    ? &PIND : ((P == 5 || P == 13) ? &PINC : ((P >= 18 && P <= 23)) ? \
    &PINF : ((P == 7) ? &PINE : &PINB)))
  #ifndef PJON_IO_PIN_TO_BIT
    #define PJON_IO_PIN_TO_BIT(P) \
      ((P >= 8 && P <= 11) ? P - 4 : ((P >= 18 && P <= 21) ? 25 - P : \
      ((P == 0) ? 2 : ((P == 1) ? 3 : ((P == 2) ? 1 : ((P == 3) ? 0 : \
      ((P == 4) ? 4 : ((P == 6) ? 7 : ((P == 13) ? 7 : ((P == 14) ? 3 : \
      ((P == 15) ? 1 : ((P == 16) ? 2 : ((P == 17) ? 0 : ((P == 22) ? 1 : \
      ((P == 23) ? 0 : ((P == 24) ? 4 : ((P == 25) ? 7 : ((P == 26) ? 4 : \
      ((P == 27) ? 5 : 6 )))))))))))))))))))
  #endif
#endif

/* AVR ATtiny45/85 - Trinket, Digispark ----------------------------------- */

#if defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  #define PJON_IO_PIN_TO_PORT_REG(P) &PORTB
  #define PJON_IO_PIN_TO_DDR_REG(P)  &DDRB
  #define PJON_IO_PIN_TO_PIN_REG(P)  &PINB
  #ifndef PJON_IO_PIN_TO_BIT
    #define PJON_IO_PIN_TO_BIT(P) P - 6
  #endif
#endif

/* SAMD21G18A - Arduino Zero ---------------------------------------------- */

#if defined(__SAMD21G18A__) || defined(ARDUINO_SAM_ZERO)  // Arduino Zero pins
  #define PJON_IO_PIN_TO_PORT_BIT(P) \
    ((P == 0) ? PORT_PA11  : ((P == 1) ? PORT_PA10  : ((P == 2) ? PORT_PA14  : \
    ((P == 3) ? PORT_PA09  : ((P == 4) ? PORT_PA08  : ((P == 5) ? PORT_PA15  : \
    ((P == 6) ? PORT_PA20  : ((P == 7) ? PORT_PA21  : ((P == 8) ? PORT_PA06  : \
    ((P == 9) ? PORT_PA07  : ((P == 10) ? PORT_PA18 : ((P == 11) ? PORT_PA16 : \
    ((P == 12) ? PORT_PA19 : ((P == 13) ? PORT_PA17 : ((P == A0) ? PORT_PA02 : \
    ((P == A1) ? PORT_PB08 : ((P == A2) ? PORT_PB09 : ((P == A3) ? PORT_PA04 : \
    ((P == A4) ? PORT_PA05 : ((P == A5) ? PORT_PB02 : ((P == SCK) ? PORT_PB11  : \
    ((P == MISO) ? PORT_PA12 : ((P == MOSI) ? PORT_PB10 : ((P == PIN_WIRE_SCL) ? \
    PORT_PA23 : ((P == PIN_WIRE_SDA) ? PORT_PA22 : PORT_PA13 \
    )))))))))))))))))))))))))

  #define PJON_IO_PIN_TO_PORT_REG(P) \
    ((P >= 15 && P <= 16) || (P == 19) || (P >= 23 && P <= 24) ? PORTB : PORTA )
  #define PJON_IO_PIN_TO_PORT_REGOUT(P) \
    ((P >= 15 && P <= 16) || (P == 19) || (P >= 23 && P <= 24) ? \
      REG_PORT_OUT1 : REG_PORT_OUT0)
  #define PJON_IO_PIN_TO_PORT_REGOUTSET(P) \
    ((P >= 15 && P <= 16) || (P == 19) || (P >= 23 && P <= 24) ? \
      REG_PORT_OUTSET1 : REG_PORT_OUTSET0)
  #define PJON_IO_PIN_TO_PORT_REGOUTCLR(P) \
    ((P >= 15 && P <= 16) || (P == 19) || (P >= 23 && P <= 24) ? \
      REG_PORT_OUTCLR1 : REG_PORT_OUTCLR0)
  #define PJON_IO_PIN_TO_PORT_REGPINCFG(P) \
    ((P >= 15 && P <= 16) || (P == 19) || (P >= 23 && P <= 24) ? \
      REG_PORT_PINCFG1 : REG_PORT_PINCFG0)
  #define PJON_IO_PIN_TO_PORT_REGDIRSET(P) \
    ((P >= 15 && P <= 16) || (P == 19) || (P >= 23 && P <= 24) ? \
      REG_PORT_DIRSET1 : REG_PORT_DIRSET0)
  #define PJON_IO_PIN_TO_PORT_REGDIRCLR(P) \
    ((P >= 15 && P <= 16) || (P == 19) || (P >= 23 && P <= 24) ? \
      REG_PORT_DIRCLR1 : REG_PORT_DIRCLR0)
  #define PJON_IO_PIN_TO_PORT_IN_REG(P) \
    ((P >= 15 && P <= 16) || (P == 19) || (P >= 23 && P <= 24) ? \
      REG_PORT_IN1 : REG_PORT_IN0)

  /* functions for read/write/pinmode fast for SAMD chips
   PWM pins 3, 4, 5, 6, 8, 9, 10, 11, 12, 13 provide 8-bit PWM output with the
   analogWrite() function analogWrite works on all analog pins and all digital
   PWM pins. You can supply it any value between 0 and 255. analog pins provide
   12bits resolution and provide a value between 0-4096.

   INPUT (0x0),
   OUTPUT (0x1),
   INPUT_PULLUP (0x2) Set pin to input mode with pull-up resistor enabled,
   INPUT_PULLDOWN (0x3) */

  #define PJON_IO_MODE(P, V) \
    do { if (__builtin_constant_p(P) && __builtin_constant_p(V)) { \
        if(V == 0x0) { \
          PJON_IO_PIN_TO_PORT_REGPINCFG(P) = (uint8_t)(PORT_PINCFG_INEN); \
          PJON_IO_PIN_TO_PORT_REGDIRCLR(P) = (uint32_t)PJON_IO_PIN_TO_PORT_BIT(P); \
        } else if(V == 0x2) { \
          PORT->Group[PJON_IO_PIN_TO_PORT_REG(11)].PINCFG[g_APinDescription[11].ulPin].reg = \
          (uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN); \
          PJON_IO_PIN_TO_PORT_REGDIRCLR(P) = (uint32_t)PJON_IO_PIN_TO_PORT_BIT(P); \
          PJON_IO_PIN_TO_PORT_REGOUTSET(P) = (uint32_t)PJON_IO_PIN_TO_PORT_BIT(P); \
        } else if(V == 0x3) { \
          PORT->Group[g_APinDescription[11].ulPort].PINCFG[g_APinDescription[11].ulPin].reg = \
          (uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN) ; \
          PJON_IO_PIN_TO_PORT_REGDIRCLR(P) = (uint32_t)PJON_IO_PIN_TO_PORT_BIT(P); \
          PJON_IO_PIN_TO_PORT_REGOUTCLR(P) = (uint32_t)PJON_IO_PIN_TO_PORT_BIT(P); \
        } else { \
          PJON_IO_PIN_TO_PORT_REGPINCFG(P) &= ~(uint8_t)(PORT_PINCFG_INEN); \
          PJON_IO_PIN_TO_PORT_REGDIRSET(P) = (uint32_t)PJON_IO_PIN_TO_PORT_BIT(P); \
        } \
      } else pinMode(P, V); \
    } while (0)

  #define PJON_IO_READ(P) ( (int) _PJON_IO_READ_(P) )
  #define _PJON_IO_READ_(P) \
    (__builtin_constant_p(P)) ? \
      (PJON_IO_PIN_TO_PORT_IN_REG(P) & PJON_IO_PIN_TO_PORT_BIT(P)) : digitalRead(P)

  #define PJON_IO_WRITE(P, V) \
    do { if (__builtin_constant_p(P) && __builtin_constant_p(V)) { \
        if(V) PJON_IO_PIN_TO_PORT_REGOUTSET(P) = PJON_IO_PIN_TO_PORT_BIT(P); \
        else PJON_IO_PIN_TO_PORT_REGOUTCLR(P) = PJON_IO_PIN_TO_PORT_BIT(P); \
      } else digitalWrite(P, V); \
    } while(0)

  #define PJON_IO_PULL_DOWN(P) \
    do { if(__builtin_constant_p(P)) { \
        PJON_IO_MODE(P, INPUT_PULLDOWN); \
      } else pinMode(P, INPUT_PULLDOWN); \
    } while(0)
#endif

/* AVR related ------------------------------------------------------------ */

#ifdef __AVR__
  #define PJON_IO_ATOMIC_WRITE(A, P, V) \
    if ((int)(A) < 0x40) { bitWrite(*(A), PJON_IO_PIN_TO_BIT(P), V); }  \
    else { \
      uint8_t register saveSreg = SREG; \
      cli(); \
      bitWrite(*(A), PJON_IO_PIN_TO_BIT(P), V); \
      SREG = saveSreg; \
    }

  #ifndef PJON_IO_WRITE
    #define PJON_IO_WRITE(P, V) \
      do { \
        if(__builtin_constant_p(P) && __builtin_constant_p(V)) \
          PJON_IO_ATOMIC_WRITE((uint8_t*) PJON_IO_PIN_TO_PORT_REG(P), P, V) \
        else digitalWrite(P, V); \
      } while(0)
  #endif

  #if !defined(PJON_IO_MODE)
    #define PJON_IO_MODE(P, V) \
    do { \
      if(__builtin_constant_p(P) && __builtin_constant_p(V)) \
        PJON_IO_ATOMIC_WRITE((uint8_t*) PJON_IO_PIN_TO_DDR_REG(P), P, V) \
      else pinMode(P, V); \
    } while(0)
  #endif

  #ifndef PJON_IO_READ
  	#define PJON_IO_READ(P) ((int) _PJON_IO_READ_(P))
  	#define _PJON_IO_READ_(P) \
    	(__builtin_constant_p(P)) ? ( \
    	((((*PJON_IO_PIN_TO_PIN_REG(P)) >> (PJON_IO_PIN_TO_BIT(P))) & 0x01))) : \
    	digitalRead(P)
  #endif

  #define PJON_IO_PULL_DOWN(P) \
    do { if(__builtin_constant_p(P)) { \
        PJON_IO_WRITE(P, LOW); \
        PJON_IO_MODE(P, INPUT); \
      } else \
        digitalWrite(P, LOW); \
        pinMode(P, INPUT); \
    } while(0)
#endif
