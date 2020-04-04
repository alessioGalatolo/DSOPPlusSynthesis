#include "bool_utils.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
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
 * @return The output of the function, given the binary input
 */
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
bool_product* product_create(bool product[], unsigned variables){
    bool_product* new_prod;
    MALLOC(new_prod, sizeof(bool_product), ;);
    MALLOC(new_prod -> product, sizeof(bool) * variables, free(new_prod));
    memcpy(new_prod -> product, product, sizeof(bool) * variables);
    new_prod -> variables = variables;
    return new_prod;
}

/**
 * @return the output of the product with the given variable values
 */
bool product_of(bool_product* product, const bool input[]){
    bool result = true;
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

/**
 * @return true if b1[i] == b2[i] for each i
 */
bool bvector_equals(const bool* b1, const bool* b2, unsigned variables){
    if(!b1 || !b2)
        return false;
    for(int i = 0; i < variables; i++)
        if(b1[i] != b2[i] && (b1[i] <= 1 || b2[i] <= 1))
            return false;
    return true;
}

/**
 * calculates the norm 1 of a boolean vector
 * @param b The vector
 * @param variables The size of the vector
 * @return The sum of the elements of the vector
 */
int norm1(const bool* b, unsigned variables){
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
 * @return The decimal corresponding to the binary or -1 in case of error
 */
int binary2decimal(const bool *values, unsigned variables) {
    int number = 0;
    for(int i = 0; i < variables; i++){
        if(values[variables - i - 1] > 1) {
            return -1;
        }
        number += values[variables - i - 1] * (int) exp2(i);
    }
    return number;
}

/**
 * Given
 * Note: array will be in descending order
 * @param values
 * @param variables
 * @param return_size
 * @return
 */
int* binary2decimals(const bool *values, unsigned  variables, int* return_size){
    *return_size = 1;
    for(int i = 0; i < variables; i++){
        if(values[i] == dash || values[i] == not_present)
            *return_size *= 2;
    }

    int dash_found = 1;

    int* numbers;
    MALLOC(numbers, sizeof(int) * *return_size,;); //will contain all the values
    memset(numbers, 0, sizeof(int) * *return_size);

    //for each index of values, adds the power of 2 to each index of numbers
    for(int i = 0; i < variables; i++){

        //if dash then half the numbers have to be updated
        if(values[variables - i - 1] == dash || values[variables - i - 1] == not_present){
            for(int j = 0; j < *return_size; j += 2 * dash_found){
                for(int k = 0; k < dash_found; k++){
                    numbers[j + k] += (int) exp2(i);
                }
            }

            dash_found *= 2;
        }else{
            for(int j = 0; j < *return_size; j++){
                numbers[j] += values[variables - i - 1] * (int) exp2(i);
            }
        }
    }
    return numbers;
}

/**
 * Get the binary representation of the given number
 */
bvector decimal2binary(int value, unsigned variables){
    bool* binary;
    MALLOC(binary, sizeof(bool) * variables, ;);
    for(int i = 0; i < variables; i++){
        binary[variables - i - 1] = value % 2;
        value /= 2;
    }
    return binary;
}
