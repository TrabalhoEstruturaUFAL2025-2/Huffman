
#ifndef BIN_UTILS_H
#define BIN_UTILS_H

#include <stdint.h>

#define BYTE_SIZE 8
#define NTH_BIT(byte, n) ((byte >> (BYTE_SIZE - n - 1)) & 1)

int write_bits(uint8_t bits, int num_bits, uint8_t *buffer, int offset);
int write_bits_int16(uint16_t B, int n, uint8_t *buffer, int offset);
int write_bits_from_uint32(uint32_t B, int n, uint8_t *buffer, int offset);

#endif // BIN_UTILS_H
