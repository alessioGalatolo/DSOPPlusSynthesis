/**
 * Library for a generic LIFO list
 */

#ifndef SOP_SYNTHESIS_LIST_H
#define SOP_SYNTHESIS_LIST_H

#include "bool_utils.h"

typedef struct _list{
    void *value;
    struct _list *next;
}list_t;

//TODO: change list structure as array

list_t* list_create();
//char* list_get(list_t **list);
list_t* list_add(list_t *list, void* value);
void* list_get(list_t*); //returns first value
void* list_as_array(list_t*, int* size); //returns the list as an array
int list_size(list_t*);
void list_destroy(list_t*);
#endif //SOP_SYNTHESIS_LIST_H
