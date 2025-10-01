
#ifdef TEST

#include "unity.h"

#include "Huffman.h"
#include "Vector.h"
#include "PriorityQueue.h"

Vector *ARENA = NULL;

void print_tree(HuffmanNode *n, int level) {
	if (!n) {
		printf("NULL\n");
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

void tree_flatten(HuffmanNode *n, HuffmanNode *buffer, size_t size, int *top) {
	buffer[*top] = *n;
	*top += 1;

	if (n->left != NULL) {
		tree_flatten(n->left, buffer, size, top);
	}

	if (n->right != NULL) {
		tree_flatten(n->right, buffer, size, top);
	}
}

void setUp(void)
{
	ARENA = huffman_arena_new();
}

void tearDown(void)
{
	huffman_arena_free(ARENA);
}

void test_Huffman_arena_new_leaf(void)
{
	vector_clear(ARENA);
	HuffmanNode *n = huffman_arena_new_leaf(ARENA, 'a', 2);

	TEST_ASSERT_EQUAL_INT(2, n->frequency);
	TEST_ASSERT_EQUAL_CHAR('a', n->symbol);
	TEST_ASSERT_EQUAL_PTR(NULL, n->parent);
	TEST_ASSERT_EQUAL_PTR(NULL, n->left);
	TEST_ASSERT_EQUAL_PTR(NULL, n->right);
}

void test_Huffman_arena_new_internal_node(void)
{
	vector_clear(ARENA);

	HuffmanNode *a = huffman_arena_new_leaf(ARENA, 'a', 2);
	HuffmanNode *b = huffman_arena_new_leaf(ARENA, 'b', 3);
	HuffmanNode *n = huffman_arena_new_internal_node(ARENA, a, b);

	TEST_ASSERT_EQUAL_INT(2 + 3, n->frequency);
	TEST_ASSERT_EQUAL_CHAR('*', n->symbol);
	TEST_ASSERT_EQUAL_PTR(NULL, n->parent);
	TEST_ASSERT_EQUAL_PTR(a, n->left);
	TEST_ASSERT_EQUAL_PTR(a, n->left);
	TEST_ASSERT_EQUAL_PTR(a->parent, n);
	TEST_ASSERT_EQUAL_PTR(b->parent, n);
}

void test_Huffman_arena_gets_frequencies(void)
{
	vector_clear(ARENA);

	char *text = "aacbca";

	int bytes = huffman_nodes_from_bytes(
		ARENA, 
		(unsigned char *) text, 
		strlen(text)
	);
	TEST_ASSERT_EQUAL_INT(3, bytes);

	TEST_ASSERT_EQUAL_INT(3, ARENA->size);

	int found;
	HuffmanNode *n;

	for (int i = 0; i < bytes; i++) {
		vector_get(ARENA, i, (void **) &n);
		switch (n->symbol) {
			case 'a':
				found = n->frequency;
				TEST_ASSERT_EQUAL_INT(3, found);
				found++;
				break;
			case 'b':
				found = n->frequency;
				TEST_ASSERT_EQUAL_INT(1, found);
				found++;
				break;
			case 'c':
				found = n->frequency;
				TEST_ASSERT_EQUAL_INT(2, found);
				found++;
				break;
		}
	}

	TEST_ASSERT_EQUAL_INT(3, found);
}

void test_Huffman_builds_tree(void) {
	vector_clear(ARENA);

	char *text = "abcabcabcedfaeb*||";
	char message[64];

	int bytes = huffman_nodes_from_bytes(
		ARENA, 
		(unsigned char *) text, 
		strlen(text)
	);
	TEST_ASSERT_EQUAL_INT(8, bytes);

	HuffmanNode *root = huffman_build_tree(ARENA);
	print_tree(root, 0);
}

#endif // TEST
