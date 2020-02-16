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

list_t* list_create();
//char* list_get(list_t **list);
list_t* list_add(list_t *list, void* value);
//void queue_print(queue_t *queue);
void list_destroy(list_t*);
#endif //SOP_SYNTHESIS_LIST_H
