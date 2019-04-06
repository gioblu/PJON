#pragma once

/* CRC32 table-less implementation
   See: http://www.hackersdelight.org/hdcodetxt/crc.c.txt

  Polynomial

  0x82608edb = x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
  (0x82608edb; 0x104c11db7) <=> (0xedb88320; 0x1db710641)
                                    |
                                  bit-reversed polynomial implicit +1 notation
                                  or reverse reciprocal notation */

struct PJON_crc32 {

  static inline uint32_t compute(
    const uint8_t *data,
    uint16_t length,
    uint32_t previousCrc32 = 0
  ) {
    uint8_t bits;
    uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
    uint8_t * current = (uint8_t*) data;
    while(length--) {
      crc ^= *current++;
      bits = 8;
      while(bits--) {
        if(crc & 1) crc = (crc >> 1) ^ 0xEDB88320;
        else crc =  crc >> 1;
      }
    }
    return ~crc; // same as crc ^ 0xFFFFFFFF
  };


  static inline bool compare(
    const uint32_t computed,
    const uint8_t *received
  ) {
    for(uint8_t i = 4; i > 0; i--)
      if(
        (uint8_t)(computed >> (8 * (i - 1))) !=
        (uint8_t)(received[3 - (i - 1)])
      ) return false;
    return true;
  };

};
