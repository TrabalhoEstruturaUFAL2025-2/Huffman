
#ifdef TEST

#include "unity.h"

#include "serialization.h"
#include "Vector.h"
#include "PriorityQueue.h"
#include "Huffman.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_serialization(void)
{
	uint8_t want[17] = {
		'*', 
		'*', 
		1, 'C', 
		1, 'B', 
		'*', 
		'*', 
		'*', 
		1, 'F', 
		1, 'E', 
		1, 'D', 
		1, 'A'
	};
	size_t want_len = 17;

	uint8_t got[want_len];
	memset(got, 0, want_len);

	Vector *arena = huffman_arena_new();
	HuffmanNode *root = deserialize_tree(arena, want, want_len);
	print_tree(root, 0);
	serialize_tree(root, got, want_len);

	for (int i = 0; i < 11; i++) {
		TEST_ASSERT_EQUAL_INT(want[i], got[i]);
	}
}

#endif // TEST
