// crc32.h
#ifndef CRC32_H
#define CRC32_H

#include <sys/types.h>
#include <stdint.h>

#define CRC32_INITIAL 0xFFFFFFFFU
uint32_t crc32(void *buff, size_t nbytes, uint32_t CRC = CRC32_INITIAL);

#endif
