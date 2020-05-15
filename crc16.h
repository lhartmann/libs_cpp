#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>

#define crc16InitialValue 0xffffU

// Calculate CRC16 for len bytes od data.
uint16_t crc16(const uint8_t *data, unsigned len, uint16_t crc=crc16InitialValue);

#endif
