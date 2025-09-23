#ifdef TEST

#include "unity.h"
#include "PriorityQueue.h"

enum Item {
    ITEM_A,
    ITEM_B,
    ITEM_C,
    ITEM_D
};

PriorityQueue *q;
enum Item item_a = ITEM_A;
enum Item item_b = ITEM_B;
enum Item item_c = ITEM_C;
enum Item item_d = ITEM_D;

void setUp(void)
{
    q = priority_queue_new();
}

void tearDown(void)
{
    priority_queue_free(q);
}

// Testa se a fila começa vazia e com tamanho 0.
void test_queue_is_initially_empty(void)
{
    TEST_ASSERT_EQUAL_INT(0, priority_queue_size(q));
    TEST_ASSERT_TRUE(priority_queue_is_empty(q));
}

// Testa o comportamento ao tentar remover de uma fila vazia.
// A função deve retornar -1, conforme a especificação.
void test_dequeue_from_empty_queue_returns_error(void)
{
    enum Item *got;
    int priority = priority_queue_dequeue(q, (void **) &got);

    TEST_ASSERT_EQUAL_INT(-1, priority);
    TEST_ASSERT_EQUAL_INT(0, priority_queue_size(q));
}

// Testa a inserção e remoção de um único elemento.
void test_enqueue_and_dequeue_single_item(void)
{
    priority_queue_enqueue(q, (void *) &item_a, 5);
    TEST_ASSERT_FALSE(priority_queue_is_empty(q));
    TEST_ASSERT_EQUAL_INT(1, priority_queue_size(q));

    enum Item *got;
    int priority = priority_queue_dequeue(q, (void **) &got);

    TEST_ASSERT_EQUAL_INT(5, priority);
    TEST_ASSERT_EQUAL_PTR(&item_a, got); // Verifica se o ponteiro é o mesmo
    TEST_ASSERT_EQUAL_INT(ITEM_A, *got); // Verifica se o valor apontado é o mesmo
    TEST_ASSERT_TRUE(priority_queue_is_empty(q));
}

// Testa a principal regra: o item de menor prioridade numérica sai primeiro.
void test_items_are_dequeued_by_lowest_priority(void)
{
    // Insere em ordem não ordenada de prioridade
    priority_queue_enqueue(q, (void *) &item_a, 10);
    priority_queue_enqueue(q, (void *) &item_b, -5);
    priority_queue_enqueue(q, (void *) &item_c, 0);
    
    TEST_ASSERT_EQUAL_INT(3, priority_queue_size(q));

    enum Item *got;
    int priority;

    // Deve sair o item B (prioridade -5)
    priority = priority_queue_dequeue(q, (void **) &got);
    TEST_ASSERT_EQUAL_INT(-5, priority);
    TEST_ASSERT_EQUAL_INT(ITEM_B, *got);
    TEST_ASSERT_EQUAL_INT(2, priority_queue_size(q));

    // Deve sair o item C (prioridade 0)
    priority = priority_queue_dequeue(q, (void **) &got);
    TEST_ASSERT_EQUAL_INT(0, priority);
    TEST_ASSERT_EQUAL_INT(ITEM_C, *got);
    TEST_ASSERT_EQUAL_INT(1, priority_queue_size(q));

    // Deve sair o item A (prioridade 10)
    priority = priority_queue_dequeue(q, (void **) &got);
    TEST_ASSERT_EQUAL_INT(10, priority);
    TEST_ASSERT_EQUAL_INT(ITEM_A, *got);
    TEST_ASSERT_EQUAL_INT(0, priority_queue_size(q));
}

// Testa se, para prioridades iguais, a ordem de inserção (FIFO) é mantida.
// Nota: Uma implementação padrão de Heap não garante essa estabilidade.
// Se este teste falhar, pode ser uma característica do algoritmo, não um erro.
void test_stability_with_duplicate_priorities(void)
{
    priority_queue_enqueue(q, (void *) &item_a, 5);
    priority_queue_enqueue(q, (void *) &item_b, 1); // Inserido primeiro com prioridade 1
    priority_queue_enqueue(q, (void *) &item_c, 10);
    priority_queue_enqueue(q, (void *) &item_d, 1); // Inserido depois com prioridade 1

    enum Item *got;
    int priority;

    // Deve remover o primeiro item inserido com prioridade 1 (item_b)
    priority = priority_queue_dequeue(q, (void **) &got);
    TEST_ASSERT_EQUAL_INT(1, priority);
    TEST_ASSERT_EQUAL_INT(ITEM_B, *got);

    // Deve remover o segundo item inserido com prioridade 1 (item_d)
    priority = priority_queue_dequeue(q, (void **) &got);
    TEST_ASSERT_EQUAL_INT(1, priority);
    TEST_ASSERT_EQUAL_INT(ITEM_D, *got);

    // Deve remover o item com a próxima prioridade mais baixa (item_a)
    priority = priority_queue_dequeue(q, (void **) &got);
    TEST_ASSERT_EQUAL_INT(5, priority);
    TEST_ASSERT_EQUAL_INT(ITEM_A, *got);
}

#endif // TEST