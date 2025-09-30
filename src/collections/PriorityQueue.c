#include "PriorityQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// typedef struct {
//     int priority;
//     void *value;
// } Node;

// typedef struct {
//     int size;
//     int capacity;
//     Node *nodes;
// } PriorityQueue;

// Cria a Fila
PriorityQueue *priority_queue_new() {
    PriorityQueue *queue = (PriorityQueue*) malloc(sizeof(PriorityQueue));
    queue->size = 0;
    queue->capacity = 10;
    queue->nodes = (Node*)malloc(queue->capacity * sizeof(Node));
    
    if (!queue->nodes) {
        free(queue);
        return NULL;
    }
    return queue;
}
 

// Libera a fila da memória. Use free() para liberar todas
// as estruturas criadas por ela
void priority_queue_free(PriorityQueue *queue) {
    if (queue) {
        free(queue->nodes);
        free(queue);
    }
}

void swap_nodes(Node *a, Node *b) {
    Node temp = *a;
    *a = *b;
    *b = temp;
}

void min_heapify(PriorityQueue *queue, int i) {
    int smallest = i;
    int left_child = 2 * i + 1;
    int right_child = 2*i + 2;

    // Procurando o menor entre i e seus filhos
    if (left_child < queue->size && queue->nodes[left_child].priority < queue->nodes[smallest].priority) {
        smallest = left_child;
    }

    if (right_child < queue->size && queue->nodes[right_child].priority < queue->nodes[smallest].priority) {
        smallest = right_child;
    }

    if (smallest != i) {
        swap_nodes(&queue->nodes[i], &queue->nodes[smallest]);
        min_heapify(queue, smallest);
    }
}

// Insere um item de qualquer tipo na fila
void priority_queue_enqueue(PriorityQueue *queue, void *value, int priority) {
    // Verificando se a capacidade é suficiente, caso não, dobra a capcidade
    if (queue->size == queue->capacity) {
        queue->capacity *= 2;
        queue->nodes = (Node*) realloc(queue->nodes, queue->capacity * sizeof(Node));
    }

    // Adiciona no final do heap
    int index = queue->size;
    queue->nodes[index].priority = priority;
    queue->nodes[index].value = value;
    queue->size++;

    // Vai subindo até voltar a ser um min-heap
    int parent_index = (index - 1) / 2;
    while (index > 0 && queue->nodes[index].priority <= queue->nodes[parent_index].priority) {
        swap_nodes(&queue->nodes[index], &queue->nodes[parent_index]);
        index = parent_index;
        parent_index = (index - 1) / 2;
    }
}

// Remove um item da fila. Retorna a prioridade, sobrescreve
// o ponteiro com o valor do item removido. Dessa forma a função
// devolve duas informações simultaneamente (semelhantemente ao scanf).
// Retorna -1 se a fila estiver vazia e preserva o ponteiro
//
// NOTA: o item com MENOR NÚMERO de prioridade deve ser removido
//
int priority_queue_dequeue(PriorityQueue *queue, void **value) {
    if (queue->size == 0) {
        return -1;
    }

    Node root = queue->nodes[0];
    *value = root.value;

    // Move o útlimo elemento para a raiz e diminui o tamanho do heap
    queue->nodes[0] = queue->nodes[queue->size - 1];
    queue->size--;

    // Tranforma de volta em min-heap
    min_heapify(queue, 0);

    return root.priority;
}

// Retorna o tamanho da fila. Recomendável manter este número
// como parte da estrutura de dados e ir modificando à medida que
// for inserindo e removendo elementos
int priority_queue_size(PriorityQueue *queue) {
    return queue->size;
}

// Wrapper da função acima. Retorna true se a fila estiver vazia
// dá pra fazer em uma linha: `priority_queue_size(q) == 0`
bool priority_queue_is_empty(PriorityQueue *queue) {
    return priority_queue_size(queue) == 0;
}