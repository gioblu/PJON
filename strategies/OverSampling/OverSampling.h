
/* OverSampling Two wires interrupts-less implementation
   Part of the PJON framework (included in version v3.0)
   Copyright (c) 2012-2016, Giovanni Blu Mitolo All rights reserved. */

#define _STXRX882_STANDARD 0

/* _STXRX882_STANDARD:
   Medium: STX882/SRX882 433Mhz ASK/FSK modules or 315/433 MHz modules (green)
    RX http://nicerf.com/manage/upfile/indexbanner/635331970881921250.pdf
    TX http://nicerf.com/manage/upfile/indexbanner/635169453223382155.pdf
    Timing for other hardware can be easily implemented in Timing.h

   Performance:
    Transfer speed: 1620Bb or 202B/s
    Absolute  communication speed: 180B/s (data length 20 of characters)
    Data throughput: 150B/s (data length 20 of characters)
    Range: 250m with no direct line of sight, 5km with direct line of sight */

#define _OS_MODE _STXRX882_STANDARD

#include "Timing.h"

class OverSampling {
  public:

    /* Check if the channel is free for transmission:
    If receiving 10 bits no 1s are detected
    there is no active transmission */

    boolean can_start(uint8_t input_pin, uint8_t output_pin) {
      float value = 0.5;
      unsigned long time = micros();
      pinModeFast(input_pin, INPUT);
      for(uint8_t i = 0; i < 9; i++) {
        while((uint32_t)(micros() - time) < _OS_BIT_WIDTH)
          value = (value * 0.999)  + (digitalReadFast(input_pin) * 0.001);
        if(value > 0.5)
          return false;
      }
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

    void send_byte(uint8_t b, uint8_t input_pin, uint8_t output_pin) {
      digitalWriteFast(output_pin, HIGH);
      delayMicroseconds(_OS_BIT_SPACER);
      digitalWriteFast(output_pin, LOW);
      delayMicroseconds(_OS_BIT_WIDTH);
      for(uint8_t mask = 0x01; mask; mask <<= 1) {
        digitalWriteFast(output_pin, b & mask);
        delayMicroseconds(_OS_BIT_WIDTH);
      }
    }


    /* Read a byte from the pin */

    uint8_t read_byte(uint8_t pin) {
      uint8_t byte_value = B00000000;
      for(uint8_t i = 0; i < 8; i++) {
        unsigned long time = micros();
        float value = 0.5;
        while((uint32_t)(micros() - time) < _OS_BIT_WIDTH)
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

    uint16_t receive_byte(uint8_t input_pin, uint8_t output_pin) {
      pullDownFast(input_pin);

      if(output_pin != NOT_ASSIGNED && output_pin != input_pin)
        pullDownFast(output_pin);

      float value = 0.5;
      unsigned long time = micros();
      /* Update pin value until the pin stops to be HIGH or passed more time than
         BIT_SPACER duration */
      while(((uint32_t)(micros() - time) < _OS_BIT_SPACER) && digitalReadFast(input_pin))
        value = (value * 0.999)  + (digitalReadFast(input_pin) * 0.001);
      /* Save how much time passed */
      time = micros();
      /* If pin value is in average more than 0.5, is a 1, and if is more than
         ACCEPTANCE (a minimum HIGH duration) and what is coming after is a LOW bit
         probably a byte is coming so try to receive it. */
      if(value > 0.5) {
        value = 0.5;
        while((uint32_t)(micros() - time) < _OS_BIT_WIDTH)
          value = (value * 0.999)  + (digitalReadFast(input_pin) * 0.001);
        if(value < 0.5) return read_byte(input_pin);
      }
      return FAIL;
    }


    /* Receive byte response */

    uint16_t receive_response(uint8_t input_pin, uint8_t output_pin) {
      digitalWriteFast(input_pin, LOW);

      if(output_pin != NOT_ASSIGNED && output_pin != input_pin)
        digitalWriteFast(output_pin, LOW);

      uint16_t response = FAIL;
      uint32_t time = micros();
      while(response == FAIL && (uint32_t)((micros() - _OS_BIT_SPACER) - _OS_BIT_WIDTH) <= time)
        response = receive_byte(input_pin, output_pin);
      return response;
    }


    /* Send byte response to package transmitter */

    void send_response(uint8_t response, uint8_t input_pin, uint8_t output_pin) {
      pinModeFast(output_pin, OUTPUT);
      send_byte(response, input_pin, output_pin);
      digitalWriteFast(output_pin, LOW);
    }
};
