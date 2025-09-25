
#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H
#include <stdbool.h>

typedef struct {
    int priority;
    void *value;
} Node;

typedef struct PriorityQueue {
	int size;
    int capacity;
    Node *nodes;
} PriorityQueue;

PriorityQueue *priority_queue_new();
void priority_queue_free(PriorityQueue *queue);

void priority_queue_enqueue(PriorityQueue *queue, void *item, int priority);
int priority_queue_dequeue(PriorityQueue *queue, void **item);

int priority_queue_size(PriorityQueue *queue);
bool priority_queue_is_empty(PriorityQueue *queue);

#endif // PRIORITYQUEUE_H
