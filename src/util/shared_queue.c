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

shared_queue* shared_queue_create(queue* q)
{
    list_t* ls = list_create(_shared_queue_copy, _shared_queue_free, NULL, NULL);
    if(ls == NULL)
        return NULL;
    if(list_insert_at_index(ls, q, 0) == NULL) 
        list_free(&ls);
    return ls;
}


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

void shared_queue_free(shared_queue* sq)
{
    list_free(&sq);
}
