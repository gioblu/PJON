
/* OverSampling Two wires interrupts-less implementation
   Part of the PJON framework (included in version v3.0)
   Copyright (c) 2012-2016, Giovanni Blu Mitolo All rights reserved.

  STXRX882:
    Medium: STX882/SRX882 433Mhz ASK/FSK radio modules
      RX http://nicerf.com/manage/upfile/indexbanner/635331970881921250.pdf
      TX http://nicerf.com/manage/upfile/indexbanner/635169453223382155.pdf
      Timing for other hardware can be easily implemented in Timing.h

  Performance:
    Transfer speed: 1620Bb or 202B/s
    Absolute  communication speed: 180B/s (data length 20 of characters)
    Data throughput: 150B/s (data length 20 of characters)
    Range: 250m with no direct line of sight, 3km with direct line of sight */

#define _STXRX882_STANDARD 0

#define _OS_MODE _STXRX882_STANDARD

#include "Timing.h"

class OverSampling {
  public:

    /* Check if the channel is free for transmission:
    If receiving 10 bits no 1s are detected
    there is no active transmission */

    static inline __attribute__((always_inline))
    boolean can_start(uint8_t pin) {
      pinModeFast(pin, INPUT);
      for(uint8_t i = 0; i < 9; i++) {
        if(digitalReadFast(pin))
          return false;
        delayMicroseconds(_OS_BIT_WIDTH);
      }
      if(digitalReadFast(pin)) return false;
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
    void send_byte(uint8_t b, uint8_t pin) {
      digitalWriteFast(pin, HIGH);
      delayMicroseconds(_OS_BIT_SPACER);
      digitalWriteFast(pin, LOW);
      delayMicroseconds(_OS_BIT_WIDTH);
      for(uint8_t mask = 0x01; mask; mask <<= 1) {
        digitalWriteFast(pin, b & mask);
        delayMicroseconds(_OS_BIT_WIDTH);
      }
    }


    /* Read a byte from the pin */

    static inline __attribute__((always_inline))
    uint8_t read_byte(uint8_t pin) {
      uint8_t byte_value = B00000000;
      for(uint8_t i = 0; i < 8; i++) {
        unsigned long time = micros();
        float value = 0.5;
        /* (freak condition used to avoid micros() overflow bug) */
        while((uint32_t)(time + _OS_BIT_WIDTH) > micros())
          value = ((value * 0.999) + (digitalReadFast(pin) * 0.001));
        byte_value += (value > 0.5) << i;
      }
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
    uint16_t receive_byte(uint8_t pin) {
      float value = 0.5;
      unsigned long time = micros();
      /* Update pin value until the pin stops to be HIGH or passed more time than
         BIT_SPACER duration (freak condition used to avoid micros() overflow bug) */
      while(((uint32_t)(time + _OS_BIT_SPACER) > micros()) && digitalReadFast(pin))
        value = (value * 0.999)  + (digitalReadFast(pin) * 0.001);
      /* Save how much time passed */
      time = micros();
      /* If pin value is in average more than 0.5, is a 1, and if is more than
         ACCEPTANCE (a minimum HIGH duration) and what is coming after is a LOW bit
         probably a byte is coming so try to receive it. */
      if(value > 0.5) {
        value = 0.5;
        while((uint32_t)(time + _OS_BIT_WIDTH) > micros())
          value = (value * 0.999)  + (digitalReadFast(pin) * 0.001);
        if(value < 0.5) return read_byte(pin);
      }
      return FAIL;
    }


    /* Get byte response from receiver */

    static inline __attribute__((always_inline))
    uint16_t get_response(uint8_t pin) {
      uint16_t response = FAIL;
      uint32_t time = micros();
      while(response == FAIL && (uint32_t)(time + _OS_BIT_SPACER + _OS_BIT_WIDTH) >= micros())
        response = receive_byte(pin);
      return response;
    }
};
