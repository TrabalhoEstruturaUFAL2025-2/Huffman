#ifdef TEST

#include "unity.h"
#include "Vector.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_Vector_OriginalComInts(void)
{
    Vector *v = vector_new(sizeof(int));
    TEST_ASSERT_NOT_NULL(v);

    int err;
    
    int val_a = 42;
    err = vector_push(v, &val_a);
    TEST_ASSERT_EQUAL_INT(0, err);

    int val_b = 99;
    err = vector_push(v, &val_b);
    TEST_ASSERT_EQUAL_INT(0, err);

    int *result_ptr;
    err = vector_get(v, 0, (void**)&result_ptr);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(42, *result_ptr);

    vector_free(v);
}

void test_Vector_ResizingIntegrity(void)
{
    Vector *v = vector_new(sizeof(int));
    
    for (int i = 0; i < 9; i++) {
        int err = vector_push(v, &i);
        TEST_ASSERT_EQUAL_INT(0, err);
    }
    
    TEST_ASSERT_EQUAL_INT(9, v->size);
    TEST_ASSERT_EQUAL_INT(16, v->capacity);

    int *result_ptr;
    vector_get(v, 2, (void**)&result_ptr); 
    TEST_ASSERT_EQUAL_INT(2, *result_ptr);

    vector_get(v, 8, (void**)&result_ptr);
    TEST_ASSERT_EQUAL_INT(8, *result_ptr);

    vector_free(v);
}

void test_Vector_WithStrings(void)
{
    Vector *v = vector_new(sizeof(char*));

    char *str1 = "hello";
    char *str2 = "world";

    vector_push(v, &str1);
    vector_push(v, &str2);

    char **resultado;
    vector_get(v, 0, (void**)&resultado);
    TEST_ASSERT_EQUAL_STRING("hello", *resultado);

    vector_get(v, 1, (void**)&resultado);
    TEST_ASSERT_EQUAL_STRING("world", *resultado);

    vector_free(v);
}

#endif // TEST