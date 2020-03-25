#include "arraylist.h"
#include "utils.h"

#define LIST_INIT_SIZE 100
#define LIST_INCREMENT 2

/**
 * Creates a list object
 * @return The pointer to the newly created list
 */
alist_t* alist_create(){
    alist_t* list;
    MALLOC(list, sizeof(alist_t), ;);
    MALLOC(list -> list, sizeof(void*) * LIST_INIT_SIZE,free(list));
    MALLOC(list -> sizes, sizeof(size_t) * LIST_INIT_SIZE,free(list -> list); free(list));
    list -> current_length = 0;
    list -> max_length = LIST_INIT_SIZE;
    return list;
}

/**
 * Adds the value to the list, keeps only the pointer
 * @param list The list
 * @param value The value to add to the list
 * @param size The size of values
 * @return the outcome of the operation
 */
int alist_add(alist_t *list, void* value, size_t size){
    if(!list)
        return 0;
    //reached max length
    if(list -> current_length == list -> max_length) {
        REALLOC(list -> list, sizeof(void*) * (list -> max_length) * LIST_INCREMENT, return 0;);
        REALLOC(list -> sizes, sizeof(size_t) * (list -> max_length) * LIST_INCREMENT, return 0;);
        list -> max_length *= LIST_INCREMENT;
    }
    list -> list[list -> current_length] = value;
    list -> sizes[list -> current_length] = size;
    (list -> current_length)++;
    return 1;
}

/**
 * @param size Will contain the size of the object
 * @return The index-sm element of the list
 */
void* alist_get(alist_t* list, size_t* size, int index){
    if(!list)
        return 0;
    if(size)
        *size = list -> sizes[index];
    return list -> list[index];
}

/**
 * @return The length of the list
 */
size_t alist_length(alist_t* list){
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
int alist_for_each(alist_t* list, int (*f) (void*, size_t*)){
    for(size_t i = 0; i < list -> current_length; i++){
        if(!f(list -> list[i], list -> sizes + i))
            return 0;
    }
    return 1;
}

/**
 * @param size Will contain the size of the array
 * @return The list as array
 */
void* alist_as_array(alist_t* list, size_t* size){
    if(size)
        *size = list -> current_length;
    return list -> list;
}

/**
 * Frees the list from the heap
 */
void alist_destroy(alist_t* list) {
    if (list != NULL) {
        free(list -> list);
        free(list -> sizes);
        free(list);
    }
}