#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>

#define LIST_INIT_SIZE 100
#define LIST_INCREMENT 2

/**
 * Creates a list object
 * @return The pointer to the newly created list
 */
list_t* list_create(){
    list_t* list;
    MALLOC(list, sizeof(list_t),;);
    MALLOC(list -> list, sizeof(void*) * LIST_INIT_SIZE,free(list));
    MALLOC(list -> sizes, sizeof(size_t) * LIST_INIT_SIZE,free(list -> list); free(list));
    list -> current_length = 0;
    list -> max_length = LIST_INIT_SIZE;
    return list;
}

/**
 * Adds the value to the list, keeps only the pointer
 * @param l The list
 * @param value The value to add to the list
 * @param size The size of values
 * @return the outcome of the operation
 */
int list_add(list_t *l, void* value, size_t size){
    if(!l)
        return 0;
    //reached max length
    if(l -> current_length == l -> max_length) {
        REALLOC(l -> list, sizeof(void*) * (l -> max_length) * LIST_INCREMENT, return 0;);
        REALLOC(l -> sizes, sizeof(size_t) * (l -> max_length) * LIST_INCREMENT, return 0;);
        l -> max_length *= LIST_INCREMENT;
    }

//    MALLOC(l -> list[l -> current_length], size,;);
//    memcpy(l -> list[l -> current_length], value, size);
    l -> list[l -> current_length] = value;
    l -> sizes[l -> current_length] = size;
    (l -> current_length)++;
    return 1;
}

/**
 * @param size Will contain the size of the object
 * @return The index-sm element of the list
 */
void* list_get(list_t* l, size_t* size, int index){
    if(!l)
        return 0;
    if(size)
        *size = l -> sizes[index];
    return l -> list[index];
}

/**
 * @return The length of the list
 */
int list_length(list_t* list){
    if(!list)
        return 0;
    return list -> current_length;
}

/**
 * Applies the given function to each member of the list
 * @param f A function that gets the object and its size
 *      and returns true if the operation is completed successfully
 * @return true if all the operation were completed successfully
 */
int list_for_each(list_t* list, int (*f) (void*, size_t*)){
    for(size_t i = 0; i < list -> current_length; i++){
        if(!f(list -> list[i], list -> sizes + i))
            return 0;
    }
    return 1;
}

/**
 * NOT YET IMPLEMENTED
 */
int list_remove(list_t* l, void* obj){
//    for(int i = 0; i < )
    return 0;
}

/**
 * @param size Will contain the size of the array
 * @return The list as array
 */
void* list_as_array(list_t* list, size_t* size){
    if(size)
        *size = list -> current_length;
    return list -> list;
}

/**
 * Frees the list from the heap
 */
void list_destroy(list_t* list) {
    if (list != NULL) {
        free(list -> list);
        free(list -> sizes);
        free(list);
    }
}