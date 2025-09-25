
#ifdef TEST

#include "unity.h"

#include "HuffmanNode.h"

#define NEW_NODE() ((HuffmanNode *) malloc(sizeof(HuffmanNode)))

void setUp(void)
{
}

void tearDown(void)
{
}

void test_HuffmanNode_combines(void)
{
	HuffmanNode a, b, c;

	huffhuffman_node_leaf_init(&a, 3, 'a');
	huffman_node_leaf_init(&b, 4, 'b');

	huffman_node_combine(&c, &a, &b);

	TEST_ASSERT_EQUAL_INT(7, c.frequency);
	TEST_ASSERT_EQUAL_CHAR('*', c.symbol);
	TEST_ASSERT_EQUAL_PTR(&a, c.left);
	TEST_ASSERT_EQUAL_PTR(&b, c.right);

	TEST_ASSERT_EQUAL_PTR(&c, a.parent);
	TEST_ASSERT_EQUAL_PTR(&c, b.parent);
}

#endif // TEST
