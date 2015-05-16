#include "list.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

int list_errno;

list_node_t* list_get_reference_at_index_null( list_t* list, int index );
list_node_t* list_get_last_reference( list_t* list );
int list_find_index(list_t* list, int (*equals)(void* element));

list_t* list_create 	( // callback functions
			  void (*element_copy)(void**, void*),
			  void (*element_free)(void**),
			  int (*element_compare)(void*, void*),
			  void (*element_print)(void*)
			) {
    list_errno = LIST_NO_ERROR;
    list_t* list = malloc(sizeof(list_t));
    if(list == NULL) {
        list_errno = LIST_MEMORY_ERROR;
        return NULL;
    }
    list->first = NULL;
    list->length = 0;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    list->element_print = element_print;
    return list;
}


void list_free( list_t** list ){
    list_errno = LIST_NO_ERROR;
    if(list == NULL) {
        list_errno = LIST_INVALID_ERROR;
        return;
    }
    while(list_size(*list) > 0) {
        list_free_at_index(*list, 0);
    }
    free(*list);
    *list = NULL;
}

int list_size( list_t* list ){
    list_errno = LIST_NO_ERROR;
    if(list == NULL) {
        list_errno = LIST_INVALID_ERROR;
        return -1;
    }
    return list->length;
}

list_t* list_insert_at_index( list_t* list, void* element, int index){
    list_errno = LIST_NO_ERROR;
    if(list == NULL) {
        list_errno = LIST_INVALID_ERROR;
        return list;
    }
    // Fetch the node that currently resides at this index
    list_node_t* ref = list_get_reference_at_index_null(list, index);
    // Allocate a new node
    list_node_t* new = malloc(sizeof(list_node_t));
    if(new == NULL) {
        list_errno = LIST_MEMORY_ERROR;
        return NULL;
    }
    //new->element = element;
    list->element_copy(&new->element, element);

    if(ref == NULL) { 
        // Inserting at the end of the list
        new->prev = list_get_last_reference(list); 
        new->next = NULL;
    } else {
        new->prev = ref->prev;
        new->next = ref;
    }

    if(list->first == ref) {
        // Inserting at the front of the list
        list->first = new;
    }

    // Update pointers of previous and next elements
    if(new->prev != NULL)
        new->prev->next = new;
    if(new->next != NULL)
        new->next->prev = new;
    
    list->length++;
    return list;
}

list_t* list_remove_at_index( list_t* list, int index){
    list_errno = LIST_NO_ERROR;
    if(list == NULL) {
        list_errno = LIST_INVALID_ERROR;
        return list;
    }
    list_node_t* ref = list_get_reference_at_index(list, index);
    if(ref == NULL) {
        list_errno = LIST_EMPTY_ERROR;
        return list;
    }

    if(ref->prev == NULL) {
        // Removing from the front of the list
        list->first = ref->next;
    } else {
        ref->prev->next = ref->next;
    }

    if(ref->next == NULL) {
        // Removing from the end of the list
        // NOP
    } else {
        ref->next->prev = ref->prev;
    }

    free(ref);
    
    list->length--;
    return list;
}

list_t* list_free_at_index( list_t* list, int index){
    list_errno = LIST_NO_ERROR;
    if(list == NULL) {
        list_errno = LIST_INVALID_ERROR;
        return list;
    }
    void* element = list_get_element_at_index(list, index);
    list->element_free(&element);
    list_remove_at_index(list, index);
    return list;
}

list_node_t* list_get_reference_at_index(list_t* list, int index) {
    return list_get_reference_at_index_null(list, index)?:list_get_last_reference(list);
}

list_node_t* list_get_reference_at_index_null( list_t* list, int index ){
    list_errno = LIST_NO_ERROR;
    if(list == NULL) {
        list_errno = LIST_INVALID_ERROR;
        return NULL;
    }
    
    list_node_t* node = list->first;
    if(index <= 0)
        return node;
    for(;index > 0&&node != NULL; index--)
        node = node->next;
    return node;
}

void* list_get_element_at_index( list_t* list, int index ){
    list_errno = LIST_NO_ERROR;
    if(list == NULL) {
        list_errno = LIST_INVALID_ERROR;
        return NULL;
    }
    list_node_t* node = list_get_reference_at_index(list, index);
    if(node != NULL)
        return node->element;
    list_errno = LIST_EMPTY_ERROR;
    return NULL;
}

int list_get_index_of_element( list_t* list, void* element ){
    int equals(void* el) {
        return list->element_compare(el, element) == 0;
    }
    return list_find_index(list, &equals);
}

void list_print( list_t* list ){
    printf("List(%p,%lu) -> %p\n", list, list->length, list->first);
    list_node_t* next = list->first;
    while(next != NULL) {
        printf("%p <- Node(%p,%p) -> %p\n", next->prev, next, next->element, next->next);
        list->element_print(next->element);
        next = next->next;
    }
}

// EXTRA
list_node_t* list_get_last_reference( list_t* list ) {
    list_node_t* node = list->first;
    if(node == NULL)
        return NULL;
    while(node->next != NULL)
        node = node->next;
    return node;
}

int list_find_index(list_t* list, int (*equals)(void* element)) {
    list_errno = LIST_NO_ERROR;
    if(list == NULL) {
        list_errno = LIST_INVALID_ERROR;
        return -1;
    }
    list_node_t* node = list->first;
    int index = 0;
    while(node != NULL) {
        if(equals(node->element))
            return index;
        node = node->next;
        index++;
    }
    return -1;
}

void* list_find(list_t* list, int (*equals)(void* element)) {
    int index = list_find_index(list, equals);
    if(index >= 0)
        return list_get_element_at_index(list, index);
    return NULL;
}
