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
    bool_f* f;
    MALLOC(f, sizeof(bool_f), ;);
    MALLOC(f -> values, sizeof(bool) * variables, free(f));
    NULL_CHECK(memcpy(f -> values, values, sizeof(bool) * variables));
    f -> variables = variables;
    return f;
}

/**
 * Generates a boolean plus function with random outputs.
 * //TODO: FUNCTION IS WRONG
 * @param variables Number of variables taken by the function
 * @return A pointer to the function
 */
bool_f* f_create_random(int variables){
//TODO: FUNCTION IS WRONG
    srandom(time(NULL));
    bool_f* f = malloc(sizeof(bool_f));
    NULL_CHECK(f);
    NULL_CHECK(f -> values = malloc(sizeof(bool) * variables));
    for(int i = 0; i < variables; i++){
        *((f -> values) + i) = random() % 2;
    }
    f -> variables = variables;
    return f;
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
    bool_product* new_prod;
    MALLOC(new_prod, sizeof(bool_product), ;);
    MALLOC(new_prod -> product, sizeof(bool) * size, free(new_prod));
    memcpy(new_prod -> product, product, sizeof(bool) * size);
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
                result = result && !input[i];
                break;
            case 1:
                result = result && input[i];
                break;
            default:
                break;
        }
        i++;
    }
    return result;
}

bool bvector_equals(bvector b1, bvector b2, int variables){
    if(!b1 || !b2)
        return false;
    for(int i = 0; i < variables; i++)
        if(b1[i] != b2[i])
            return false;
    return true;
}

/**
 * calculates the norm 1 of a boolean vector
 * @param b The vector
 * @param variables The size of the vector
 * @return The sum of the elements of the vector
 */
int norm1(const bool* b, int variables){
    int sum = 0;
    for(int i = 0; i < variables; i++){
        if(b[i] <= 1)
            sum += b[i];
    }
    return sum;
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

//array will be in descending order
int* binary2decimals(const bool *values, int size, int* return_size){
    *return_size = 1;
    for(int i = 0; i < size; i++){
        if(values[i] == dash || values[i] == not_present)
            *return_size *= 2;
    }

    int dash_found = 1;

    int* numbers;
    MALLOC(numbers, sizeof(int) * *return_size,;); //will contain all the values
    memset(numbers, 0, sizeof(int) * *return_size);

    //for each index of values, adds the power of 2 to each index of numbers
    for(int i = 0; i < size; i++){

        //if dash then half the numbers have to be updated
        if(values[size - i - 1] == dash || values[size - i - 1] == not_present){
            for(int j = 0; j < *return_size; j += 2 * dash_found){
                for(int k = 0; k < dash_found; k++){
                    numbers[j + k] += (int) exp2(i);
                }
            }

            dash_found *= 2;
        }else{
            for(int j = 0; j < *return_size; j++){
                numbers[j] += values[size - i - 1] * (int) exp2(i);
            }
        }
    }
    return numbers;
}


/**
 * Get the binary representation of the given number
 */
bvector decimal2binary(int value, int size){
    bool* binary;
    MALLOC(binary, sizeof(bool) * size, ;);
    for(int i = 0; i < size; i++){
        binary[size - i - 1] = value % 2;
        value /= 2;
    }
    return binary;
}
