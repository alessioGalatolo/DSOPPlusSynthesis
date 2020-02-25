/**
 * Library for a generic LIFO list
 */

#ifndef SOP_SYNTHESIS_LIST_H
#define SOP_SYNTHESIS_LIST_H

#include <stddef.h>
#include "bool_utils.h"



typedef struct _list{
    void** list;
    size_t* sizes;
    size_t current_length;
    size_t max_length;
}list_t;

//TODO: change list structure as array

list_t* list_create();
//char* list_get(list_t **list);
int list_add(list_t *list, void* value, size_t size);
int list_for_each(list_t* list, int (*f) (void*, size_t*));
void* list_get(list_t*, size_t*, int index);
//int list_remove_at(list_t*, int index);
int list_remove(list_t*, void* obj);
void* list_as_array(list_t*, size_t* size); //returns the list as an array
int list_length(list_t*);
void list_destroy(list_t*);
#endif //SOP_SYNTHESIS_LIST_H
