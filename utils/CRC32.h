
 /* CRC32 table-less implementation
    See: http://www.hackersdelight.org/hdcodetxt/crc.c.txt */

uint32_t compute_crc_32(const uint8_t *data, uint16_t length, uint32_t previousCrc32 = 0) {
  uint8_t bits;
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  unsigned char* current = (unsigned char*) data;
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
