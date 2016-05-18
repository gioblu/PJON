
/* SoftwareBitBang One wire interrupts-less implementation
   Part of the PJON framework (included in version v3.0)
   Copyright (c) 2012-2016, Giovanni Blu Mitolo All rights reserved.


Performance

Using only digitalWriteFast() micros() and delayMicroseconds() fast cross-architecture
communication speed can be achieved. SoftwareBitBang works in 3 different communication
modes, STANDARD, FAST and OVERDRIVE:
STANDARD: 16944Bd or 2.12kB/s cross-architecture, promiscuous clock compatible.
FAST: 25157Bd or 3.15kB/s cross-architecture, promiscuous clock compatible.
OVERDRIVE: Runs a specific architecture at its maximum limits (non cross-architecture
compatible). Every architecture has its own limits, Arduino Duemilanove for example
runs at 33898Bd or 4.23kB/s, Arduino Zero can reach 48000Bd or 6.00kB/s.


Compatibility

- ATmega88/168/328 16Mhz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)
- ATmega2560 16Mhz (Arduino Mega)
- ATmega16u4/32u4 16Mhz (Arduino Leonardo)
- ATtiny45/85 8Mhz, see https://github.com/gioblu/PJON/wiki/ATtiny-interfacing
- SAMD (Arduino Zero)
- ESP8266 v.1-7 80Mhz "AI-THINKER AT" firmware, see https://github.com/esp8266/Arduino
- ESP8266 NodeMCU v0.9-1.0 80Mhz, see https://github.com/esp8266/Arduino
- MK20DX256 96Mhz (Teensy 3.1)


Why not interrupts?

Usage of libraries is really extensive in the Arduino environment and often the
end user is not able to go over collisions or redefinitions. Very often a library
is using hardware resources of the microcontroller as timers or interrupts, colliding
or interrupting other libraries. This happens because in general Arduino boards have
limited hardware resources. To have a universal and reliable communication medium
in this sort of environment, software emulated bit-banging, I think, is a good, stable
and reliable solution that leads to "more predictable" results than interrupt driven
systems coexisting on small microcontrollers without the original developer and the
end user knowing about it.


Known issues

1 A pull down resistor in the order of mega ohms could be necessary on the bus to
reduce interference. See https://github.com/gioblu/PJON/wiki/Deal-with-interference

2 Consider that this is not an interrupt driven system and so all the time passed
in delay or executing something a certain amount of packets could be potentially
lost unheard, the packet manager of PJON will do its job scheduling the packet
to be sent again in future until is received or MAX_ATTEMPTS sending attempts is
reached, but a certain amount of bandwidth can be wasted. Structure intelligently
your loop cycle to avoid huge blind timeframes.

3 SoftwareBitBang strategy can have some compatibility issue with codebases that
are using interrupts in their procedure, like for example the Servo library.
Reliability or bandwidth loss can be experienced because of the cyclical
interruptions made by third party interrupt driven software to the PJON code.


STANDARD mode performance:
   Transfer speed: 16.944kBb or 2.12kB/s
   Absolute  communication speed: 1.81kB/s (data length 20 of characters)
   Data throughput: 1.51kB/s (data length 20 of characters) */
#define _SWBB_STANDARD  0

/* FAST mode performance:
   Transfer speed: 25.157kBd or 3.15kB/s
   Absolute  communication speed: 2.55kB/s (data length 20 of characters)
   Data throughput: 2.13kB/s (data length 20 of characters) */
#define _SWBB_FAST      1

/* OVERDRIVE mode performance:
   Architecture / setup dependant, see Timing.h */
#define _SWBB_OVERDRIVE 2

/* Set here the mode you want to use - default STANDARD */
#define _SWBB_MODE _SWBB_STANDARD

#include "Timing.h"

class SoftwareBitBang {
  public:

    /* Check if the channel is free for transmission:
    If receiving 10 bits no 1s are detected
    there is no active transmission */

    static inline __attribute__((always_inline))
    boolean can_start(uint8_t input_pin, uint8_t output_pin) {
      pinModeFast(input_pin, INPUT);
      for(uint8_t i = 0; i < 9; i++) {
        if(digitalReadFast(input_pin))
          return false;
        delayMicroseconds(SWBB_BIT_WIDTH);
      }
      if(digitalReadFast(input_pin)) return false;
      pinModeFast(output_pin, OUTPUT);
      return true;
    }


    /* Every byte is prepended with 2 synchronization padding bits. The first
       is a longer than standard logic 1 followed by a standard logic 0.
       __________ ___________________________
      | SyncPad  | Byte                      |
      |______    |___       ___     _____    |
      | |    |   |   |     |   |   |     |   |
      | | 1  | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
      |_|____|___|___|_____|___|___|_____|___|
        |
       ACCEPTANCE

    The reception tecnique is based on finding a logic 1 as long as the
    first padding bit within a certain threshold, synchronizing to its
    falling edge and checking if it is followed by a logic 0. If this
    pattern is recognised, reception starts, if not, interference,
    synchronization loss or simply absence of communication is
    detected at byte level. */

    static inline __attribute__((always_inline))
    void send_byte(uint8_t b, uint8_t input_pin, uint8_t output_pin) {
      digitalWriteFast(output_pin, HIGH);
      delayMicroseconds(SWBB_BIT_SPACER);
      digitalWriteFast(output_pin, LOW);
      delayMicroseconds(SWBB_BIT_WIDTH);
      for(uint8_t mask = 0x01; mask; mask <<= 1) {
        digitalWriteFast(output_pin, b & mask);
        delayMicroseconds(SWBB_BIT_WIDTH);
      }
    }


    /* Syncronize with transmitter:
     This function is used only in byte syncronization.
     READ_DELAY has to be tuned to correctly send and
     receive transmissions because this variable shifts
     in which portion of the bit, the reading will be
     executed by the next read_byte function */

    static inline __attribute__((always_inline))
    uint8_t syncronization_bit(uint8_t pin) {
      delayMicroseconds((SWBB_BIT_WIDTH / 2) - SWBB_READ_DELAY);
      uint8_t bit_value = digitalReadFast(pin);
      delayMicroseconds(SWBB_BIT_WIDTH / 2);
      return bit_value;
    }


    /* Read a byte from the pin */

    static inline __attribute__((always_inline))
    uint8_t read_byte(uint8_t pin) {
      uint8_t byte_value = B00000000;
      /* Delay until the center of the first bit */
      delayMicroseconds(SWBB_BIT_WIDTH / 2);
      for(uint8_t i = 0; i < 7; i++) {
        /* Read in the center of the n one */
        byte_value += digitalReadFast(pin) << i;
        /* Delay until the center of the next one */
        delayMicroseconds(SWBB_BIT_WIDTH);
      }
      /* Read in the center of the last one */
      byte_value += digitalReadFast(pin) << 7;
      /* Delay until the end of the bit */
      delayMicroseconds(SWBB_BIT_WIDTH / 2);
      return byte_value;
    }


    /* Check if a byte is coming from the pin:
     This function is looking for padding bits before a byte.
     If value is 1 for more than ACCEPTANCE and after
     that comes a 0 probably a byte is coming:
      ________
     |  Init  |
     |--------|
     |_____   |
     |  |  |  |
     |1 |  |0 |
     |__|__|__|
        |
      ACCEPTANCE */

    static inline __attribute__((always_inline))
    uint16_t receive_byte(uint8_t input_pin, uint8_t output_pin) {
      /* Initialize the pin and set it to LOW to reduce interference */
      pullDownFast(input_pin);
      uint32_t time = micros();
      /* Do nothing until the pin goes LOW or passed more time than SWBB_BIT_SPACER duration */
      while(digitalReadFast(input_pin) && (uint32_t)(micros() - time) <= SWBB_BIT_SPACER);
      /* Save how much time passed */
      time = micros() - time;
      /* is for sure equal or less than SWBB_BIT_SPACER, and if is more than ACCEPTANCE
         (a minimum HIGH duration) and what is coming after is a LOW bit
         probably a byte is coming so try to receive it. */
      if(time >= SWBB_ACCEPTANCE && !syncronization_bit(input_pin))
        return (uint8_t)read_byte(input_pin);
      return FAIL;
    }


    /* Receive byte response */

    static inline __attribute__((always_inline))
    uint16_t receive_response(uint8_t input_pin, uint8_t output_pin) {
      uint16_t response = FAIL;
      uint32_t time = micros();
      while(response == FAIL && (uint32_t)(time + SWBB_BIT_SPACER + SWBB_BIT_WIDTH) >= micros())
        response = receive_byte(input_pin, output_pin);
      return response;
    }


    /* Send byte response to package transmitter */

    static inline __attribute__((always_inline))
    void send_response(uint8_t response, uint8_t input_pin, uint8_t output_pin) {
      pinModeFast(output_pin, OUTPUT);
      send_byte(response, input_pin, output_pin);
      pullDownFast(output_pin);
    }
};
