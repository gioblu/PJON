
#pragma once

/* Compute CRC8 with a table-less implementation: */

struct PJON_crc8 {

  static inline uint8_t roll(char input_byte, uint8_t crc) {
    for(uint8_t i = 8; i; i--, input_byte >>= 1) {
      uint8_t result = (crc ^ input_byte) & 0x01;
      crc >>= 1;
      if(result) crc ^= 0x8C;
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
