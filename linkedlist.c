/*
 * Created by Alessio on 3/24/20.
 */

#include <assert.h>
#include <values.h>
#include "linkedlist.h"
#include "utils.h"

//internal function
bool lower_literals(productp_t* p1, productp_t* p2);

/**
 * Creates the linked list
 * @return A pointer to the linked list
 */
llist_t* llist_create(){
    llist_t* list;
    MALLOC(list, sizeof(llist_t), ;);
    list->head = NULL;
    list->length = 0;
    return list;
}

/**
 * Adds the element to the head of the list
 * @param list The linked list
 * @param p The product to add to the list
 * @return true if the operation was successful
 */
int llist_add(llist_t* list, productp_t* p){
    NULL_CHECK(list);
    node_t* node = list->head;
    MALLOC(list->head, sizeof(node_t), ;);
    list->head->next = node;
    list->head->parent = NULL;
    list->head->product = p;
    list->head->indexes = binary2decimals(p->b_product->product, p->b_product->variables, &list->head->size);
    list->length++;
    if(node)
        node->parent = list->head;
    return 1;
}

/**
 * Extracts the product with the minimum of the maximum of its weights
 * Then updates the values of the function and removes the implicants covering
 * 0 points
 * @param list The linked list
 * @param value Sets the coefficient chosen
 * @param f The function
 * @return The product extracted
 */
productp_t* llist_max_product(llist_t* list, int* value, fplus_t* f){
    NULL_CHECK(list);
    NULL_CHECK(list->head);
    node_t* max_node = list->head;
    node_t* current_node = list->head;
    int max = INT_MIN;
    //select max
    while(current_node != NULL){
        int min = INT_MAX;
        for (int j = 0; j < current_node->size; j++) {
            assert(current_node);
            int fvalue = fplus_value_at(f, current_node->indexes[j]);
            if(fvalue == 0){
                //implicants covering 0 points has to be removed
                min = INT_MIN; //ending point
                if(current_node->next)
                    current_node->next->parent = current_node->parent;
                if(current_node->parent)
                    current_node->parent->next = current_node->next;
                else
                    list->head = current_node->next;
                free(current_node->indexes);
                node_t* to_free = current_node;
                current_node = current_node->next;
                free(to_free);
                list->length--;
                break;
            }
            if (fvalue < min && fvalue != F_DONT_CARE_VALUE)
                min = fvalue;
        }
        if(min != INT_MIN) {
            //node was not deleted
            if (min != INT_MAX &&
                (min > max || (min == max && lower_literals(current_node->product, max_node->product)))) {
                max = min;
                max_node = current_node;
            }
            current_node = current_node->next;
        }
    }
    //no implicant was valid
    if(max == INT_MIN)
        return NULL;
    *value = max;
    productp_t* return_value = max_node->product;
    //update f values
    for (int i = 0; i < max_node->size; i++)
        fplus_sub2value_dsopp(f, max_node->indexes[i], max);
    if(max_node->next)
        max_node->next->parent = max_node->parent;
    if(max_node->parent)
        max_node->parent->next = max_node->next;
    else
        //no parent => first node
        list->head = max_node->next;
    free(max_node->indexes);
    free(max_node);
    list->length--;
    return return_value;
}

/**
 * @return The length of a linked list
 */
size_t llist_length(llist_t* list){
    NULL_CHECK(list);
    return list->length;
}

/**
 * Frees all the memory used by the list
 */
void llist_destroy(llist_t* list){
    if(list != NULL){
        while(list->head != NULL){
            node_t* to_free = list->head;
            free(to_free->indexes);
            list->head = list->head->next;
            list->length--;
            free(to_free);
        }
        free(list);
    }
}

/**
 * @return true if p1 has lower literals than p2
 */
bool lower_literals(productp_t* p1, productp_t* p2){
    int sum1 = 0;
    int sum2 = 0;
    for(int i = 0; i < p1->b_product->variables; i++){
        sum1 += p1->b_product->product[i] < 2;
        sum2 += p2->b_product->product[i] < 2;
    }
    return sum1 < sum2;
}