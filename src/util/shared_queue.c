#include "shared_queue.h"
#include "list.h"
#include "queue.h"
#include <assert.h>

void _shared_queue_copy(void** dest, void* src)
{
    *dest = src;
}

void _shared_queue_free(void** q)
{
    // NOP
}

/**
 * Creates a shared queue from a normal queue
 *
 * The normal queue may only be read from, no new elements may be enqueued externally.
 * @return list_t*|NULL
 */
shared_queue* shared_queue_create(queue* q)
{
    list_t* ls = list_create(_shared_queue_copy, _shared_queue_free, NULL, NULL);
    if(ls == NULL)
        return NULL;
    if(list_insert_at_index(ls, q, 0) == NULL) 
        list_free(&ls);
    return ls;
}


/**
 * Enqueues an element in all shared queues
 *
 * The element gets enqueued in the first queue,
 * the other queues get forwarded, so the pointers
 * to the data line up.
 */
void shared_queue_enqueue(shared_queue* q, void* el)
{
    struct list_node *node = q->first;
    assert(node != NULL);
    queue_enqueue(node->element, el);
    node = node->next;
    while(node != NULL) {
        queue_forward(node->element);
        node = node->next;
    }
}

/**
 * Creates a new copy of the queue that has its data synchronized
 * with the other queues in this shared queue.
 *
 * This queue should only be read from, not written to.
 * @return queue*|NULL
 */
queue* shared_queue_fork(shared_queue* sq)
{
    queue* q = list_get_element_at_index(sq, 0);
    if(q == NULL)
        return NULL;
    queue* q2 = queue_fork(q);
    if(q2 == NULL)
        return NULL;
    
    if(list_insert_at_index(sq, q2, 0) == NULL) {
        queue_unfork(q2);
        return NULL;
    }
    return q2;
}

/**
 * Frees the shared queue.
 * The main queue and forked queues should be freed separately with
 * queue_free() and queue_unfork() respectively.
 */
void shared_queue_free(shared_queue* sq)
{
    list_free(&sq);
}
