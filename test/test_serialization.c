
#ifdef TEST

#include "unity.h"

#include "serialization.h"
#include "Vector.h"
#include "PriorityQueue.h"
#include "Huffman.h"

int TREE_SIZE = 11;

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
	serialize_tree(root, got, want_len);

	for (int i = 0; i < 11; i++) {
		TEST_ASSERT_EQUAL_INT(want[i], got[i]);
	}
}

void test_serialization_tree_writing(void) {
	uint8_t flat_tree[17] = {
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
	size_t flat_tree_len = 17;

	uint8_t got[64];
	size_t got_len = 64;
	memset(got, 0, 30);

	Vector *arena = huffman_arena_new();
	HuffmanNode *root = deserialize_tree(arena, flat_tree, flat_tree_len);

	struct TreeData data = get_tree_data(root);
	int bytes = write_tree(&data, got, got_len, 0);
	TEST_ASSERT_EQUAL_INT(bytes, sizeof(uint16_t) + flat_tree_len);

	TEST_ASSERT_EQUAL_INT((got[0] << 8) | got[1], TREE_SIZE);

	for (int i = 0; i < TREE_SIZE; i++) {
		TEST_ASSERT_EQUAL_INT(flat_tree[i], got[i + 2]);
	}
}

#endif // TEST
