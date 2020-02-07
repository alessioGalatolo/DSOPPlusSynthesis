//
// Created by alessio on 2/7/20.
//

#include "bool_utils.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

bool_f* f_create(int values[], int variables){
    bool_f* boolf = malloc(sizeof(bool_f));
    NULL_CHECK(boolf);
    NULL_CHECK(boolf -> values = malloc(sizeof(int) * variables));
    NULL_CHECK(memcpy(boolf -> values, values, sizeof(int) * variables));
    boolf -> size = variables;
    return boolf;
}

bool_f* f_create_random(int variables){
    srandom(time(NULL));
    bool_f* boolf = malloc(sizeof(bool_f));
    NULL_CHECK(boolf);
    NULL_CHECK(boolf -> values = malloc(sizeof(int) * variables));
    for(int i = 0; i < variables; i++){
        *((boolf -> values) + i) = random() ;
    }
    boolf -> size = variables;
    return boolf;
}


bool_product* product_create(int product[], int size){
    bool_product* new_prod = malloc(sizeof(bool_product));
    NULL_CHECK(new_prod);
    new_prod -> product = product;
    new_prod -> size = size;
    return new_prod;
}