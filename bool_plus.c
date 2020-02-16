#include "bool_plus.h"
#include "utils.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

//internal functions
void binaries(bool value[], int i, sopp *sop, fplus* fun, bool *result);

/**
 * Creates a boolean plus function with the given parameters
 * @param values An array containing all the outputs of the function (values[i] = f(binary(i))
 * @param non_zeros An array with all the index (in binary) of the non-zeros values of the function
 * @param variables Number of variables taken by the function
 * @param size size of the non_zeros array
 * @return A pointer to the function
 */
fplus* fplus_create(int* values, bool** non_zeros, int variables, int size){
    fplus* function = malloc(sizeof(fplus));
    NULL_CHECK(function);
    function -> values = values;
    function -> variables = variables;
    function -> non_zeros = non_zeros;
    function -> size = size;
    return function;
}

/**
 * Creates a boolean plus function with random outputs.
 * Each input has 50% chance of being non zero and its value is random
 * @param variables Number of variables taken by the function
 * @return A pointer to the function
 */
fplus* fplus_create_random(int variables){
    srandom(time(NULL));
    fplus* function = malloc(sizeof(fplus));
    NULL_CHECK(function);
    NULL_CHECK(function -> values = malloc(sizeof(int) * variables));
    NULL_CHECK(function -> non_zeros = malloc(sizeof(bool) * variables * variables));
    int non_zeros_index = 0;
    for(int i = 0; i < variables; i++){
        bool is_non_zero = random() % 2;
        if(is_non_zero) {
            function -> values[i] = (int) random();
            function -> non_zeros[non_zeros_index++] = decimal2binary(i, variables); //end of array
        }else
            function -> values[i] = 0;
    }
    function -> variables = variables;
    function -> size = non_zeros_index; //end of array
    NULL_CHECK(function -> non_zeros = realloc(function -> non_zeros, sizeof(bool) * variables * (non_zeros_index)));
    return function;
}

/**
 * returns the output of the function with the given input
 * @param f A pointer to the boolean plus function
 * @param input The input to the function given as a vector of bool
 * @return The output of the function
 */
int fplus_value_of(fplus* f, bool input[]){
    return f -> values[binary2decimal(input, f -> variables)];
}

/**
 * Creates and initializes a sop plus form
 * @return The sopp form
 */
sopp* sopp_create(){
    sopp* sopp = malloc(sizeof(sopp));
    NULL_CHECK(sopp);
    sopp -> products = list_create();
    sopp -> size = 0;
    return sopp;
}

/**
 * Adds a product to the sop plus form
 * @param sopp The sop plus form
 * @param sop1 The product to add
 * @return true if the operation was successful
 */
int sopp_add(sopp* sopp, product_plus* sop1){
    sopp -> products = list_add(sopp -> products, sop1);
    (sopp -> size)++;
    return sopp -> products == NULL ? 0: 1;
}

/**
 * Checks if the sop plus for the given input equals the value
 * @return true if the output matches
 */
int sopp_value_equals(sopp* sop, bool input[], int value){
    int sopp_value = 0;
    list_t* list = sop -> products;
    while(list != NULL && sopp_value < value){
        product_plus* current_p = list -> value;
        sopp_value += current_p -> coeff * product_of(current_p -> product, input);
        list = list -> next;
    }
    return sopp_value == value;
}


/**
 * Checks if sopp is a correct sop plus form of the function fun
 * time: exponential in number of variables
 * @return true it is valid
 */
bool sop_plus_of(sopp* sopp, fplus* fun){
    bool value[sopp -> size];
    memset(value, 0, sizeof(bool) * sopp -> size);
    bool result = 1;
    binaries(value, sopp -> size, sopp, fun, &result);
    return result;
}

/*
 * rec-fun called by sop_plus_of
 *
 * generates all the possible combinations of the variables
 * and checks if the non zero values of the fun matches the non zero values
 * of the sop plus form
 */
void binaries(bool value[], int i, sopp* sop, fplus* fun, bool* result) {
    if (i == 0) {
        int fvalue = fplus_value_of(fun, value);
        *result = *result && (fvalue == 0 || sopp_value_equals(sop, value, fvalue));
    } else {
        value[i - 1] = 0;
        binaries(value, i - 1, sop, fun, result);
        value[i - 1] = 1;
        binaries(value, i - 1, sop, fun, result);
    }
}

/*
 * compares two bool vectors
 * b1 > b2 <=> sum(b1[i]) > sum(b2[i])
 */
int compare(const bool* b1, const bool* b2, int variables){
    int b1value = 0;
    int b2value = 0;
    for(int i = 0; i < variables; i++){
        b1value += b1[i];
        b2value += b2[i];
    }
    return b2value - b1value; //b1 < b2
}


//standard partition function for quicksort
int partition (bool** arr, int low, int high, int variables){
    // pivot (Element to be placed at right position)
    bool* pivot = arr[high];

    int i = (low - 1);  // Index of smaller element

    for (int j = low; j < high; j++) {
        // If current element is smaller than the pivot
//        if (arr[j] < pivot) {
        if(compare(arr[j], pivot, variables) > 0){
            i++;    // increment index of smaller element
            bool* tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
        }
    }

    bool* tmp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = tmp;
    return i + 1;
}

/*
 * custom quicksort, compares a array of bool vectors.
 * For each element e, it stores in norms the sum(e(i)) for later use
 */
void quickSort(bool** arr, int low, int high, int variables, int* norms) {
    if (low < high) {
        /* pi is partitioning index, arr[pi] is now
           at right place */
        int pi = partition(arr, low, high, variables);
        norms[pi] = binary2decimal(arr[pi], variables);
        quickSort(arr, low, pi - 1, variables, norms);  // Before pi
        quickSort(arr, pi + 1, high, variables, norms); // After pi
    }
}

/**
 * Quine–McCluskey algorithm for finding prime implicants
 * @param f
 * @return
 */
implicant_plus* prime_implicants(fplus* f){
    int* norms = malloc(sizeof(int) * f -> size);
    NULL_CHECK(norms);
    bool** non_zeros = f -> non_zeros;
    int size = f -> size;

    //sort the non zero values
    quickSort(non_zeros, 0, size - 1, f -> variables, norms);

    //split them in classes
    list_t* norms_split[size];
    for(int i = 0; i < size; i++)
        norms_split[i] = list_create();
    int index = 0;
    norms_split[0] = list_add(norms_split[0], non_zeros[0]);
    for(int i = 1; i < size; i++){
        if(norms[i] > norms[i - 1])
            index++;
        norms_split[index] = list_add(norms_split[index], non_zeros[i]);
    }

    //TODO: join vectors with only one different value


    //TODO: iterate
    return 0;
}
