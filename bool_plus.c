#include "bool_plus.h"
#include "utils.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <math.h>


//internal functions
void binaries(bool value[], int i, sopp *sop, fplus* fun, bool *result);
bool* joinable_vectors(const bool*, const bool*, int size);
sop_t* implicants2sop(bool**, int size, int variables);

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
    int f_size = (int) exp2(variables);
    NULL_CHECK(function);
    NULL_CHECK(function -> values = malloc(sizeof(int) * f_size));
    NULL_CHECK(function -> non_zeros = malloc(sizeof(bool) * variables * f_size));
    int non_zeros_index = 0;
    for(int i = 0; i < f_size; i++){
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
    int non_zeros_size = f -> size;
    bool** result = NULL;
    int result_size = 0;

    while(non_zeros_size > 0) {
        //sort non zero values
        quickSort(non_zeros, 0, non_zeros_size - 1, f -> variables, norms);

        //split them in classes
        list_t *norms_split[non_zeros_size]; //list of lists
        for (int i = 0; i < non_zeros_size; i++)
            norms_split[i] = list_create();
        int norms_splits_size = 0;
        norms_split[0] = list_add(norms_split[0], non_zeros[0]);
        for (int i = 1; i < non_zeros_size; i++) {
            if (norms[i] > norms[i - 1])
                norms_splits_size++;
            norms_split[norms_splits_size] = list_add(norms_split[norms_splits_size], non_zeros[i]);
        }

//        free(non_zeros);
        NULL_CHECK(non_zeros = malloc(sizeof(bool *) * non_zeros_size)); //new array
        non_zeros_size = 0;

        //join matching vectors
        int old_list_size, list2_size;
        bool **old_list = list_as_array(norms_split[0], &old_list_size); //will store old latest list retrieved
        for (int norms_index = 1; norms_index < norms_splits_size; norms_index++) {
            bool **list2 = list_as_array(norms_split[norms_index], &list2_size);

            for (int i = 0; i < old_list_size; i++) {
                for (int j = 0; j < list2_size; j++) {
                    bool *elem = joinable_vectors(old_list[i], list2[j], f->variables); //TODO: may there be some duplicates?
                    if (elem) {
                        non_zeros[non_zeros_size++] = elem;
                    }
                }
            }

            old_list = list2;
            old_list_size = list2_size;
        }


        if(non_zeros_size == 0){//end of cycle
            bool** gigarrey[norms_splits_size];
            int sizes[norms_splits_size];
            result_size = 0;
            for(int i = 0; i < norms_splits_size; i++){
                gigarrey[i] = list_as_array(norms_split[i], sizes + i);
                result_size += sizes[i];
            }

            NULL_CHECK(result = malloc(sizeof(bool*) * result_size));

            int current_list = 0;
            int current_index = 0;
            for(int i = 0; i < result_size; i++){
                if(current_index < sizes[current_list])
                    result[i] = gigarrey[current_list][current_index++];
                else{
                    current_list++;
                    current_index = 0;
                    result[i] = gigarrey[current_list][current_index];
                }
            }

//            free(gigarrey); //???
        }/*else
        for(int i = 0; i < norms_splits_size; i++)
            list_destroy(norms_split[i]);*/
    }

    implicant_plus* implicants = malloc(sizeof(implicant_plus));
    NULL_CHECK(implicants);
    implicants -> implicants = result;
    implicants -> size = result_size;

    return implicants;
}

/**
 * Checks if the two vectors are joinable
 * Two vectors v1, v2 are joinable <=> Exists j / v1(j) = !v2(j) and for each i != j v1(i) = v2(i)
 * The joined vector will be equal to v1(or v2) for each i != j and will a dash value for i = j
 * @param size The size of the vectors
 * @return The joined vector (in heap) if possible, NULL otherwise
 */
bool* joinable_vectors(const bool* v1, const bool* v2, int size){
    bool* join = malloc(sizeof(bool) * size);
    NULL_CHECK(join);
    int counter = 0; //counts the complimentary bits found
    for(int i = 0; i < size; i++){
        if(v1[i] == v2[i]){
            join[i] = v1[i];
        }else if(v1[i] == dash && v2[i] == dash) {
            free(join);
            return NULL;
        }else{
            counter++;
            if(counter > 1) {
                free(join);
                return NULL; //are not joinable
            }
            join[i] = dash;
        }
    }
    return join;
}

/**
 * Finds the essential implicants of the function from the prime implicants
 * @return a pointer to a struct containing the essential points and the essential prime implicants
 */
essentials* essential_implicants(fplus* f, implicant_plus* implicants){
    int f_size = (int) exp2(f -> variables);
    list_t* points[f_size]; //each index represent a point of f, the list will contain the implicants covering that point
    bool** essential_implicants = malloc(sizeof(bool*) * implicants -> size); //stores the essential points
    NULL_CHECK(essential_implicants);
    int ei_index = 0; //index of above and below array;
    bool** essential_points = malloc(sizeof(bool*) * implicants -> size);
    NULL_CHECK(essential_points);

    //init the array of points
    for(int i = 0; i < f_size; i++){
        points[i] = list_create();
    }

    //count occurrences of various points
    for(int i = 0; i < implicants -> size; i++){
        int size = 0;
        int* indexes = binary2decimals(implicants -> implicants[i], f -> variables, &size);
        for(int j = 0; j < size; j++){
            points[indexes[j]] = list_add(points[indexes[j]], implicants -> implicants[i]);
        }
    }

    //store points and implicants in array
    for(int i = 0; i < f -> size; i++){
        int index = binary2decimal(f -> non_zeros[i], f -> variables);
        int l_size = list_size(points[index]);
        if(l_size == 0){
            fprintf(stderr, "Some error occurred, an essential point has not been covered");
            return NULL;
        }else if(l_size == 1){
            essential_implicants[ei_index] = list_get(points[index]);
            essential_points[ei_index++] = f -> non_zeros[i];
        }
    }

    //resize memory
    NULL_CHECK(essential_implicants = realloc(essential_implicants, sizeof(bool*) * ei_index));
    NULL_CHECK(essential_points = realloc(essential_points, sizeof(bool*) * ei_index));

    essentials* e = malloc(sizeof(essentials));
    e -> implicants = implicants2sop(essential_implicants, ei_index, f -> variables);
    e -> points = essential_points;
    e -> size = ei_index;
//    free(essential_implicants);
    return e;
}


sop_t* implicants2sop(bool** implicants, int size, int variables){

}

