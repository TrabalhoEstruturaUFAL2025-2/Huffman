
#include "bin_utils.h"
#include <stdio.h>


int LEFTMOST_BIT_MASK = 1 << (BYTE_SIZE - 1);

int write_bits(uint8_t B, int n, uint8_t *buffer, int offset) {
    for (int i = n - 1; i >= 0; --i) {
        uint8_t bit_to_write = (B >> i) & 1;

        int dest_bit_pos = offset;
        int dest_byte_index = dest_bit_pos / 8;
        int dest_bit_in_byte = dest_bit_pos % 8;

		printf("bit_to_write %d, dest_byte_index %d, dest_bit_in_byte %d\n", bit_to_write, dest_byte_index, dest_bit_in_byte);

        if (bit_to_write) {
            buffer[dest_byte_index] |= (LEFTMOST_BIT_MASK >> dest_bit_in_byte);
        } else {
            buffer[dest_byte_index] &= ~(LEFTMOST_BIT_MASK >> dest_bit_in_byte);
        }

		offset++;
    }

	return n;
}

int write_bits_int16(uint16_t B, int n, uint8_t *buffer, int offset) {
	n = n > BYTE_SIZE * 2 ? BYTE_SIZE * 2 : n;

	if (n > BYTE_SIZE) {
		offset += write_bits(B >> BYTE_SIZE, n - BYTE_SIZE, buffer, offset);
	}

	write_bits(B & 0xFF, BYTE_SIZE, buffer, offset);

	return n;
}
