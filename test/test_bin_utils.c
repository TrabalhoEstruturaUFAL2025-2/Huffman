
#ifdef TEST

#include "unity.h"

#include "bin_utils.h"
#include <stdlib.h>
#include <string.h>

void setUp(void)
{
}

void tearDown(void)
{
}

void test_bin_utils_write_bits(void)
{
	unsigned char buffer[4] = {
		0x00, 0x00, 0x00, 0x00
	};

	write_bits(0b00000111, 3, buffer, 0);
	TEST_ASSERT_EQUAL_INT(0b11100000, buffer[0]);
	memset(buffer, 0x00, sizeof(buffer));

	// 0123 4567 8910
	// 0000 0010 0100 
	// 0000 0000 0000
	write_bits(0b00001001, 4, buffer, 6);
	TEST_ASSERT_EQUAL_INT(0b00000010, buffer[0]);
	TEST_ASSERT_EQUAL_INT(0b01000000, buffer[1]);

	buffer[0] = 0x00;
	write_bits(0b00000001, 2, buffer, 0);
	TEST_ASSERT_EQUAL_INT(0b01000000, buffer[0]);

}

void test_bin_utils_nth_bit(void)
{
	TEST_ASSERT_EQUAL_INT(NTH_BIT(0b00010000, 3), 1);
	TEST_ASSERT_EQUAL_INT(NTH_BIT(0b10000000, 1), 0);
	TEST_ASSERT_EQUAL_INT(NTH_BIT(0b11111110, 7), 0);
}

void test_bin_utils_writes_uint16_bits(void)
{
	uint8_t buffer [2] = {
		0x00, 0x00
	};


	write_bits_int16(0xFFFF, 12, buffer, 4);

	printf("buffer[0]: %d\n", buffer[0]);
	printf("buffer[1]: %d\n", buffer[1]);

	TEST_ASSERT_EQUAL_INT(0x0F, buffer[0]);
	TEST_ASSERT_EQUAL_INT(0xFF, buffer[1]);
}

#endif // TEST
