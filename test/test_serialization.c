
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

void print_tree(HuffmanNode *n, int level) {
	if (!n) {
		return;
	}

	for (int i = 0; i < level; i++) {
		putchar(' ');
	}

	if (n->left == NULL && n->right == NULL) {
		printf("%c(%d)\n", n->symbol, n->frequency);
	} else {
		printf("%c(%d)\n", n->symbol, n->frequency);
		print_tree(n->left, level + 4);
		print_tree(n->right, level + 4);
	}
	
}

void test_serialization(void)
{
	uint8_t want[] = {
		'*', '*', 'C', 'B', '*', '*',
		'*', '\\', '*', '\\', '\\', 'A', 'E'
	};
	size_t want_len = sizeof(want);

	uint8_t got[want_len];
	memset(got, 0, want_len);

	Vector *arena = huffman_arena_new();
	HuffmanNode *root = deserialize_tree(arena, want, want_len);
	print_tree(root, 0);
	int err = serialize_tree(root, got, want_len);
	TEST_ASSERT_NOT_EQUAL_INT(err, -1);

	char msg[64];
	for (int i = 0; i < want_len; i++) {
		snprintf(msg, sizeof(msg), "want: %c, got: %c; idx: %d", want[i], got[i], i);
		TEST_ASSERT_MESSAGE(want[i] == got[i], msg);
	}
}

/*
void test_serialization_tree_writing(void) {
	uint8_t flat_tree[] = {
		'*', 
		'*', 
		'C', 
		'B', 
		'*', 
		'*', 
		'*', 
		'F', 
		'E', 
		'D', 
		'A'
	};
	size_t flat_tree_len = sizeof(flat_tree);

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
*/

#endif // TEST
