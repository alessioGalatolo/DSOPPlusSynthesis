//
// Created by alessio on 3/24/20.
//

#ifndef SOP_SYNTHESIS_LINKEDLIST_H
#define SOP_SYNTHESIS_LINKEDLIST_H

#include <stddef.h>
#include "bool_plus.h"

typedef struct _node{
    struct _node* next;
    struct _node* parent;
    productp_t* product;
    int* indexes;
    int size;
}node_t;

typedef struct{
    node_t* head;
    size_t length;
}llist_t;

llist_t* llist_create();
int llist_add(llist_t*, productp_t*);
productp_t* llist_max_product(llist_t *list, int *value, fplus_t *f);
size_t llist_length(llist_t*);
void llist_destroy(llist_t*);


#endif //SOP_SYNTHESIS_LINKEDLIST_H
