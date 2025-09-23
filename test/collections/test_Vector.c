
#ifdef TEST

#include "unity.h"

#include "Vector.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_Vector(void)
{
	Vector *v = vector_new(sizeof(char*));

	int err = vector_set(v, "a");
	TEST_ASSERT_EQUAL_INT(0, err);

	vector_push(v, "b");
	TEST_ASSERT_EQUAL_INT(0, err);

	int *value;
	err = vector_get(v, 0, (void **) &value);
	TEST_ASSERT_EQUAL_INT(0,  err);
	TEST_ASSERT_EQUAL_STRING("a", *value);

	err = vector_get(v, v->size - 1, (void **) &value);
	TEST_ASSERT_EQUAL_INT(0,  err);
	TEST_ASSERT_EQUAL_STRING("b", *value);

	err = vector_get(v, 999, (void **) &value);
	TEST_ASSERT_EQUAL_INT(-1, err);

	vector_free(v);
}

#endif // TEST
