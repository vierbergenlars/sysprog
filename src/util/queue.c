#include "queue.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#define Q_CLAMP(queue, current) ((current)%((queue)->max_size))
#define Q_LAST(queue) Q_CLAMP(queue, (queue)->first + (queue)->length)
#define Q_NEXT_VALUE(queue, current) Q_CLAMP(queue, (current)+1)

#ifdef NDEBUG
#define DEBUG_PRINTF(fmt, args...)
#else
#define DEBUG_PRINTF(fmt, args...) fprintf(stderr, "%s:%d:%s: " fmt "\n", __FILE__, __LINE__, __func__, ##args )
#endif

#define LOCKABLE(lock) pthread_rwlock_t lock##_lock
#define LOCK(type, lock) \
    do { \
        int res = pthread_rwlock_##type(&lock##_lock);\
        switch(res) {\
            case EINVAL:\
                        DEBUG_PRINTF("Invalid lock "#lock);\
            abort();\
            break;\
            case EDEADLK:\
                         DEBUG_PRINTF("Deadlock detected "#lock);\
            abort();\
            break;\
            case EPERM:\
                       DEBUG_PRINTF("No lock was held "#lock);\
            case 0:\
                   break;\
            default:\
                    DEBUG_PRINTF("Unknown error code %d "#lock, res);\
            break;\
        }\
    } while(0)
#define RD_LOCK(lock) LOCK(rdlock, lock)
#define WR_LOCK(lock) LOCK(wrlock, lock)
#define UNLOCK(lock) LOCK(unlock, lock)

struct queue {
    char* arr;
    size_t max_size;
    size_t first;
    size_t length;
    size_t element_size;
    LOCKABLE(first);
    LOCKABLE(length);
};

queue* queue_create(size_t max_size, size_t element_size)
{
    queue* q = malloc(sizeof(queue));
    if(q == NULL)
        return NULL;
    q->arr = calloc(element_size, max_size);
    if(q->arr == NULL)
        goto err_out_1;
    q->max_size = max_size;
    q->first = 0;
    q->length = 0;
    q->element_size = element_size;

    if(pthread_rwlock_init(&q->first_lock, NULL) != 0)
        goto err_out_2;
    if(pthread_rwlock_init(&q->length_lock, NULL) != 0)
        goto err_out_3;
    return q;

err_out_3:
    pthread_rwlock_destroy(&q->first_lock);
err_out_2:
    free(q->arr);
err_out_1:
    free(q);
    return NULL;
}

queue* queue_fork(queue* q)
{
    queue* q2 = queue_create(q->max_size, q->element_size);
    if(q2 == NULL)
        return NULL;
    free(q2->arr);
    q2->arr = q->arr;
    return q2;
}

void queue_unfork(queue* q)
{
    free(q);
}

void queue_free(queue* q)
{
    pthread_rwlock_destroy(&q->first_lock);
    pthread_rwlock_destroy(&q->length_lock);
    free(q->arr);
    free(q);
}

void queue_enqueue(queue* q, void* element)
{
   memcpy(queue_forward(q), element, q->element_size);
}

void* queue_forward(queue* q)
{
   WR_LOCK(q->length);
   WR_LOCK(q->first);
   void* pos = q->arr+Q_LAST(q)*q->element_size;
   if(q->length == q->max_size) {
       q->first = Q_NEXT_VALUE(q, q->first);
   } else {
       q->length++;
   }
   UNLOCK(q->first);
   UNLOCK(q->length);
   return pos;
}


size_t queue_size(queue* q)
{
    RD_LOCK(q->length);
    size_t len = q->length;
    UNLOCK(q->length);
    return len;
}

void* queue_top(queue* q)
{
    RD_LOCK(q->first);
    void* e = q->arr+q->first*q->element_size;
    void* c = malloc(q->element_size);
    if(c != NULL)
        memcpy(c, e, q->element_size);
    UNLOCK(q->first);
    return c;
}

void* queue_dequeue(queue* q)
{
    WR_LOCK(q->length);
    WR_LOCK(q->first);
    if(q->length == 0)
        return NULL;
    void* el = q->arr+q->first*q->element_size;
    void* c = malloc(q->element_size);
    if(c != NULL)
        memcpy(c, el, q->element_size);
    q->first = Q_NEXT_VALUE(q, q->first);
    q->length--;
    UNLOCK(q->first);
    UNLOCK(q->length);
    return c;
}
