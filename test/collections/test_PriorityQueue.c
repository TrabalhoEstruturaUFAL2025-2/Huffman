
#ifdef TEST

#include "unity.h"

#include "PriorityQueue.h"

enum Item {
	FOO,
	BAR,
	BAZ
};

void setUp(void)
{
}

void tearDown(void)
{
}

// Should remove most recent element of lowest priority number
void test_PriorityQueueBehaviour(void)
{
	enum Item foo = FOO;
	enum Item bar = BAR;
	enum Item baz = BAZ;

	PriorityQueue *q = priority_queue_new();

	priority_queue_enqueue(q, (void *) foo, 3);
	priority_queue_enqueue(q, (void *) bar, 1);
	priority_queue_enqueue(q, (void *) baz, 1);

	enum Item got;
	int priority = priority_queue_dequeue(q, (void **) &got);

	TEST_ASSERT_EQUAL_INT(bar, got);
	TEST_ASSERT_EQUAL_INT(1, priority);
}

#endif // TEST
