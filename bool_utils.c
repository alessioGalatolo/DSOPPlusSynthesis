#include "bool_utils.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/**
 * Creates a boolean function given the output as values
 * @param values The output of the function. Ex: values[0] = f(0,...,0), values[1] = f(0,...,0,1), ecc..
 * @param variables The number of variables taken by the function
 * @return A pointer to a struct representing the function
 */
bool_f* f_create(bool values[], int variables){
    bool_f* boolf = malloc(sizeof(bool_f));
    NULL_CHECK(boolf);
//    NULL_CHECK(boolf -> values = malloc(sizeof(bool) * variables));
//    NULL_CHECK(memcpy(boolf -> values, values, sizeof(int) * variables));
    boolf -> values = values;
    boolf -> variables = variables;
    return boolf;
}

/**
 * Generates a boolean plus function with random outputs.
 * //TODO: FUNCTION IS WRONG
 * @param variables Number of variables taken by the function
 * @return A pointer to the function
 */

//TODO: FUNCTION IS WRONG
bool_f* f_create_random(int variables){
    srandom(time(NULL));
    bool_f* boolf = malloc(sizeof(bool_f));
    NULL_CHECK(boolf);
    NULL_CHECK(boolf -> values = malloc(sizeof(bool) * variables));
    for(int i = 0; i < variables; i++){
        *((boolf -> values) + i) = random() % 2;
    }
    boolf -> variables = variables;
    return boolf;
}

//Returns the output of the function, given the binary input
bool f_get_value(bool_f* function, bool input[]){
    NULL_CHECK(function);
    int decimal = binary2decimal(input, function -> variables);
    return function -> values[decimal];
}

/**
 * Creates and initializes the boolean product
 * @param product The product to be assigned
 * @param size The number of variables of the product
 * @return a pointer to the product
 */
bool_product* product_create(bool product[], int size){
    bool_product* new_prod = malloc(sizeof(bool_product));
    NULL_CHECK(new_prod);
    new_prod -> product = product;
    new_prod -> variables = size;
    return new_prod;
}

/**
 * @return the output of the product with the given variable values
 */
bool product_of(bool_product* product, const bool input[]){
    bool result = 1;
    int i = 0;
    while(result && i < product -> variables){
        switch(product -> product[i]){
            case 0:
                result = !input[i];
                break;
            case 1:
                result = input[i];
                break;
        }
        i++;
    }
    return result;
}

/**
 * Get the decimal representation of the given binary number
 * @param values an array of booleans
 * @param size of the array
 */
int binary2decimal(const bool *values, int size) {
    int number = 0;
    for(int i = 0; i < size; i++){
        number += values[size - i - 1] * (int) exp2(i);
    }
    return number;
}

int* binary2decimals(const bool *values, int size, int* returnsize){
    *returnsize = 1;
    for(int i = 0; i < size; i++){
        if(values[i] == dash)
            *returnsize *= 2;
    }

    int dash_found = 1;

    int* numbers = malloc(sizeof(int) * *returnsize);
    NULL_CHECK(numbers);
    memset(numbers, 0, sizeof(int) * *returnsize);

    for(int i = 0; i < size; i++){
        if(values[size - i - 1] == dash){
            for(int j = 0; j < size; j += 2){
                numbers[j] += (int) exp2(i);
                //TODO: not working
            }
            dash_found *= 2;
        }else{
            for(int j = 0; j < size; j++){
                numbers[j] += values[size - i - 1] * (int) exp2(i);
            }
        }
    }
    return numbers;
}


/**
 * Get the binary representation of the given number
 */
bool* decimal2binary(int value, int size){
    bool* binary = malloc(sizeof(bool) * size);
    NULL_CHECK(binary);
    for(int i = 0; i < size; i++){
        binary[size - i - 1] = value % (int) exp2(i);
    }
    return binary;
}
