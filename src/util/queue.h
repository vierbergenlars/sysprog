#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stddef.h>
typedef struct queue queue;
queue* queue_create(size_t max_size);
void queue_free(queue* q);
void queue_enqueue(queue* q, queue_data_type* element);
size_t queue_size(queue* q);
queue_data_type* queue_top(queue* q);
queue_data_type* queue_dequeue(queue* q);
#endif
