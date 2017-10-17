
#pragma once

/* Compute CRC8 with a table-less implementation:
   Copyright Giovanni Blu Mitolo giorscarab@gmail.com 2017

   CRC8 C2, source Baicheva98 (implicit + 1 representation)
   0x97 = (x + 1)(x^7 + x^6 + x^5 + x^2 + 1)^2
   Chosen because it has the largest possible length (119 bit) at which
   HD=4 can be achieved with 8-bit CRC. */

struct PJON_crc8 {

  static inline uint8_t roll(uint8_t input_byte, uint8_t crc) {
    for(uint8_t i = 8; i; i--, input_byte >>= 1) {
      uint8_t result = (crc ^ input_byte) & 0x01;
      crc >>= 1;
      if(result) crc ^= 0x97;
    }
    return crc;
  };


  static inline uint8_t compute(const uint8_t *input_byte, uint16_t length) {
    uint8_t crc = 0;
    for(uint16_t b = 0; b < length; b++)
      crc = roll(input_byte[b], crc);
    return crc;
  };

};
