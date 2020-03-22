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


list_t* list_create(); //creates a list
int list_add(list_t *list, void* value, size_t size); //add to the list the value
int list_for_each(list_t* list, int (*f) (void*, size_t*)); //applies the function to each element of the list
void* list_get(list_t*, size_t*, int index); //gets the element at index
void* list_as_array(list_t*, size_t* size); //returns the list as an array
size_t list_length(list_t*); //returns the list length
void list_destroy(list_t*); //free the memory used by the list
#endif //SOP_SYNTHESIS_LIST_H
