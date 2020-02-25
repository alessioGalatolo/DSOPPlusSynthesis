#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>

#define LIST_INIT_SIZE 100
#define LIST_INCREMENT 2
list_t* list_create(){
    list_t* list;
    MALLOC(list, sizeof(list_t),;);
    MALLOC(list -> list, sizeof(void*) * LIST_INIT_SIZE,free(list));
    MALLOC(list -> sizes, sizeof(size_t) * LIST_INIT_SIZE,free(list -> list); free(list));
    list -> current_length = 0;
    list -> max_length = LIST_INIT_SIZE;
    return list;
}

int list_add(list_t *l, void* value, size_t size){
    if(!l)
        return 0;
    //reached max length
    if(l -> current_length >= l -> max_length) {
        REALLOC(l -> list, sizeof(void*) * l -> max_length * LIST_INCREMENT,;);
        l -> max_length *= LIST_INCREMENT;
    }

    MALLOC(l -> list[l -> current_length], size,;);
    memcpy(l -> list[l -> current_length], value, size);
    l -> sizes[l -> current_length] = size;
    l -> current_length++;
    return 1;
}

void* list_get(list_t* l, size_t* size, int index){
    if(!l)
        return 0;
    if(size)
        *size = l -> sizes[index];
    return l -> list[index];
}

int list_length(list_t* list){
    if(!list)
        return 0;
    return list -> current_length;
}

int list_for_each(list_t* list, int (*f) (void*, size_t*)){
    for(size_t i = 0; i <  list -> current_length; i++){
        if(!f(list -> list[i], list -> sizes + i))
            return 0;
    }
    return 1;
}

int list_remove(list_t* l, void* obj){
//    for(int i = 0; i < )
    return 0;
}

void* list_as_array(list_t* list, size_t* size){
    if(size)
        *size = list -> current_length;
    return list -> list;
}

/**
 * Frees the list from the heap, also frees all the elements pointed
 */
void list_destroy(list_t* list){
    for(size_t i = 0; i < list -> current_length; i++){
        free(list -> list[i]);
    }
    free(list -> sizes);
    free(list);
}