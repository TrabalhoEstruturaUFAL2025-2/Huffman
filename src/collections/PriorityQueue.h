
#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

typedef struct PriorityQueue {
	/**/
} PriorityQueue;

PriorityQueue *priority_queue_new();
void priority_queue_free(PriorityQueue *queue);

int priority_queue_enqueue(PriorityQueue *queue, void *item, int priority);
int priority_queue_dequeue(PriorityQueue *queue, void **item);

int priority_queue_size(PriorityQueue *queue);
int priority_queue_is_empty(PriorityQueue *queue);

#endif // PRIORITYQUEUE_H
