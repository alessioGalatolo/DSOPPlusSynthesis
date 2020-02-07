//
// Created by alessio on 2/7/20.
//

#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>

//sop* list_get(list_t *list){
//    NULL_CHECK(list);
//
//    char *ret = (*queue) -> s;
//    queue_t *to_free = *queue;
//    *queue = (*queue) -> next;
//    free(to_free);
//    return ret;
//}


list_t* list_create(){
    return NULL;
}

list_t* list_add(list_t *list, void* value){
    list_t* new_node = malloc(sizeof(list_t));
    NULL_CHECK(new_node);
    new_node -> value = value;
    new_node -> next = list;
    return new_node;
}

void list_destroy(list_t* list){
    while(list != NULL){
        list_t* next = list -> next;
        free(list);
        //TODO: free also void* ?
        list = next;
    }
}