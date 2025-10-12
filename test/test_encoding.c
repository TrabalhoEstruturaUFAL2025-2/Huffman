
#ifdef TEST

#include "unity.h"

#include "encoding.h"
#include "Huffman.h"
#include "Vector.h"
#include "PriorityQueue.h"
#include "bin_utils.h"

Vector *ARENA = NULL;
HuffmanNode *ROOT = NULL;

void setUp(void)
{
	char *text = "abacaba";

	ARENA = huffman_arena_new();

	huffman_nodes_from_bytes(ARENA, (unsigned char *) text, strlen(text));
	ROOT = huffman_build_tree(ARENA);
}

void tearDown(void)
{
	huffman_arena_free(ARENA);
}

/* 
Generated Tree From 'abacaba'
  *(7)
      *(3)
          c(1)
          b(2)
      a(4)
*/
void test_Encoder_creates_table(void)
{

	Encoder encoder; 
	init_encoder(&encoder, ROOT);

	struct Codeword *cw;
	cw = encoder_get_codeword(&encoder, 'c');
	TEST_ASSERT_NOT_NULL(cw);
	TEST_ASSERT_EQUAL_INT(0, cw->bits);
	TEST_ASSERT_EQUAL_INT(2, cw->length);

	cw = encoder_get_codeword(&encoder, 'b');
	TEST_ASSERT_NOT_NULL(cw);
	TEST_ASSERT_EQUAL_INT(1, cw->bits);
	TEST_ASSERT_EQUAL_INT(2, cw->length);

	cw = encoder_get_codeword(&encoder, 'a');
	TEST_ASSERT_NOT_NULL(cw);
	TEST_ASSERT_EQUAL_INT(1, cw->bits);
	TEST_ASSERT_EQUAL_INT(1, cw->length);
}

void test_Encoder_writes(void) {
	Encoder encoder; 
	init_encoder(&encoder, ROOT);

	// 1 01 1 00 1 01 1
	// 1011 0010 11
	uint8_t want[2] = {0b00000101, 0b00100000};

	uint8_t got[2] = {0, 0};
	size_t got_size = 2;
	int offset = 5;

	int written;
	struct Codeword *cw;

	cw = encoder_get_codeword(&encoder, 'a');
	written = encoder_write(&encoder, cw, got, got_size, offset);
	TEST_ASSERT_EQUAL_INT(1, written);
	TEST_ASSERT_EQUAL_INT(0b00000100, got[0]);
	offset += written;

	cw = encoder_get_codeword(&encoder, 'b');
	written = encoder_write(&encoder, cw, got, got_size, offset);
	TEST_ASSERT_EQUAL_INT(2, written);
	TEST_ASSERT_EQUAL_INT(0b00000101, got[0]);
	offset += written;

	cw = encoder_get_codeword(&encoder, 'c');
	written = encoder_write(&encoder, cw, got, got_size, offset);
	TEST_ASSERT_EQUAL_INT(2, written);
	TEST_ASSERT_EQUAL_INT(0b00000000, got[1]);
	offset += written;

	cw = encoder_get_codeword(&encoder, 'a');
	written = encoder_write(&encoder, cw, got, got_size, offset);
	offset += written;
	TEST_ASSERT_EQUAL_INT(0b00100000, got[1]);

	TEST_ASSERT_EQUAL_INT(offset, 11);
	TEST_ASSERT_EQUAL_INT(want[0], got[0]);
	TEST_ASSERT_EQUAL_INT(want[1], got[1]);
}

void test_Decoder(void) {
	Encoder encoder; 
	uint8_t got[1] = {0};
	size_t got_size = 1;
	init_encoder(&encoder, ROOT);

	Decoder decoder = new_decoder(ROOT);

	struct Codeword *cw;
	size_t offset = 0;
	cw = encoder_get_codeword(&encoder, 'a');
	offset += encoder_write(&encoder, cw, got, got_size, offset);

	cw = encoder_get_codeword(&encoder, 'b');
	offset += encoder_write(&encoder, cw, got, got_size, offset);

	cw = encoder_get_codeword(&encoder, 'c');
	offset += encoder_write(&encoder, cw, got, got_size, offset);

	cw = encoder_get_codeword(&encoder, 'a');
	offset += encoder_write(&encoder, cw, got, got_size, offset);

	cw = encoder_get_codeword(&encoder, 'c');
	offset += encoder_write(&encoder, cw, got, got_size, offset);

	decoder_feed(&decoder, got[0]);

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
	  ((byte) & 0x80 ? '1' : '0'), \
	  ((byte) & 0x40 ? '1' : '0'), \
	  ((byte) & 0x20 ? '1' : '0'), \
	  ((byte) & 0x10 ? '1' : '0'), \
	  ((byte) & 0x08 ? '1' : '0'), \
	  ((byte) & 0x04 ? '1' : '0'), \
	  ((byte) & 0x02 ? '1' : '0'), \
	  ((byte) & 0x01 ? '1' : '0') 

	// printf(BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(got[0]));

	int consumed;
	uint8_t out;

	consumed = decoder_consume(&decoder, &out);
	TEST_ASSERT_EQUAL_INT(1, consumed);
	TEST_ASSERT_EQUAL_CHAR('a', out);

	consumed = decoder_consume(&decoder, &out);
	TEST_ASSERT_EQUAL_INT(1, consumed);
	TEST_ASSERT_EQUAL_CHAR('b', out);

	consumed = decoder_consume(&decoder, &out);
	TEST_ASSERT_EQUAL_INT(1, consumed);
	TEST_ASSERT_EQUAL_CHAR('c', out);

	consumed = decoder_consume(&decoder, &out);
	TEST_ASSERT_EQUAL_INT(1, consumed);
	TEST_ASSERT_EQUAL_CHAR('a', out);

	consumed = decoder_consume(&decoder, &out);
	TEST_ASSERT_EQUAL_INT(1, consumed);
	TEST_ASSERT_EQUAL_CHAR('c', out);

	consumed = decoder_consume(&decoder, &out);
	TEST_ASSERT_EQUAL_INT(0, consumed);
}


#endif // TEST
