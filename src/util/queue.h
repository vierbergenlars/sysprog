#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stddef.h>
typedef struct queue queue;
queue* queue_create(size_t, size_t);
queue* queue_fork(queue*);
void queue_unfork(queue*);
void queue_free(queue* q);
void queue_enqueue(queue* q, void* element);
void* queue_forward(queue* q);
size_t queue_size(queue* q);
void* queue_top(queue* q);
void* queue_dequeue(queue* q);
#endif
