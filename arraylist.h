/**
 * Library for a generic list
 */

#ifndef DSOPP_SYNTHESIS_ARRAYLIST_H
#define DSOPP_SYNTHESIS_ARRAYLIST_H

#include <stddef.h>

typedef struct _list{
    void** list;
    size_t* sizes;
    size_t current_length;
    size_t max_length;
}alist_t;


alist_t* alist_create(); //creates a list
int alist_add(alist_t *list, void* value, size_t size); //add to the list the value
int alist_for_each(alist_t* list, int (*f) (void*, size_t*)); //applies the function to each element of the list
void* alist_get(alist_t*, size_t*, int index); //gets the element at index
void* alist_as_array(alist_t*, size_t* size); //returns the list as an array
size_t alist_length(alist_t*); //returns the list length
void alist_destroy(alist_t*); //free the memory used by the list
#endif //DSOPP_SYNTHESIS_ARRAYLIST_H
