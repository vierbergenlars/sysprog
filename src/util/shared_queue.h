#ifndef _SHARED_QUEUE_H_
#define _SHARED_QUEUE_H_
#include "list.h"
#include "queue.h"
typedef list_t shared_queue;
shared_queue* shared_queue_create(queue*);
void shared_queue_enqueue(shared_queue* q, void* el);
queue* shared_queue_fork(shared_queue* sq);
void shared_queue_free(shared_queue*);
#endif
