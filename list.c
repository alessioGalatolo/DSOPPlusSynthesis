#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>



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

void* list_get(list_t* l){
    if(!l)
        return 0;
    else
        return l -> value;
}

int list_size(list_t* list){
    int size = 0;
    while(list != NULL){
        list = list -> next;
        size++;
    }
    return size;
}


void* list_as_array(list_t* list, int* size){
    *size = list_size(list);
    void** array = malloc(sizeof(void*) * (*size));
    for(int i = 0; i < *size; i++){
        array[*size - i - 1] = list -> value;
        list = list -> next;
    }
    return array;
}

/**
 * Frees the list from the heap, also frees all the elements pointed
 */
void list_destroy(list_t* list){
    while(list != NULL){
        list_t* next = list -> next;
        free(list -> value);
        free(list);
        list = next;
    }
}