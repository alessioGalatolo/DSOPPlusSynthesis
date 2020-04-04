/*
 * Created by Alessio on 3/24/20.
 *
 * A library implementing a linked list. It is designed to be used only with
 * products plus list in the context of dsopp synthesis
 */

#ifndef SOP_SYNTHESIS_LINKEDLIST_H
#define SOP_SYNTHESIS_LINKEDLIST_H

#include <stddef.h>
#include "bool_plus.h"

typedef struct _node{
    struct _node* next;
    struct _node* parent;
    productp_t* product;
    int* indexes; //indexes covered by the product
    int size; //size of above array
}node_t;

//non recursive struct
typedef struct{
    node_t* head;
    size_t length;
}llist_t;

llist_t* llist_create(); //creates the list
int llist_add(llist_t*, productp_t*); //adds a product to the list
productp_t* llist_max_product(llist_t *list, int *value, fplus_t *f); //extract proper max from the list
size_t llist_length(llist_t*); //length of the list
void llist_destroy(llist_t*); //frees the memory

#endif //SOP_SYNTHESIS_LINKEDLIST_H
