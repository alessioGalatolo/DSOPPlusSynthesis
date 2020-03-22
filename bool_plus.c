#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include "bool_plus.h"
#include "utils.h"

#define PROBABILITY_NON_ZERO_VALUE 50

//parameter for sopp representation in semi-hashtable
#define INIT_SIZE 100
#define GOOD_LOAD 0.5

//heap macros. TODO: replace with shift
#define HEAP_PARENT(x) (floor((double) x / 2))
#define HEAP_LEFT(x) (x*2)
#define HEAP_RIGHT(x) (x*2 + 1)

//internal functions
void sopp_binaries(bool *value, int i, sopp_t *sop, fplus_t* fun, bool *result);
void dsopp_binaries(bool *value, int i, dsopp_t *sop, fplus_t* fun, bool *result);
bvector joinable_vectors(const bool*, const bool*, int size);
productp_t** implicants2sop(bvector*, int size, int variables, int* final_size);
long product_hashcode(productp_t *p);
bool r_cover_s(int *r_indexes, int r_size, int *s_indexes, int s_size, fplus_t *f, int* non_zero_values);
bool lower_literals(productp_t* p1, productp_t* p2);

/**
 * Creates a boolean plus function with the given parameters
 * @param values An array containing all the outputs of the function (values[i] = f(binary(i))
 * @param non_zeros An array with all the index (in binary) of the non-zeros values of the function
 * @param variables Number of variables taken by the function
 * @param size size of the non_zeros array
 * @return A pointer to the function
 */
fplus_t* fplus_create(int* values, bvector* non_zeros, int variables, int size){
    fplus_t* function = malloc(sizeof(fplus_t));
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
fplus_t* fplus_create_random(int variables, int max_value){
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    srandom(spec.tv_nsec);
    fplus_t* function;
    long f_size = (long) exp2l(variables);
    int non_zeros_index = 0;

    MALLOC(function, sizeof(fplus_t), ;);
    MALLOC(function -> values, sizeof(int) * f_size, free(function));
    MALLOC(function -> non_zeros, sizeof(bvector) * f_size, free(function -> values); free(function));

    for(size_t i = 0; i < f_size; i++){
        bool is_non_zero = random() % 100;
        if(is_non_zero < PROBABILITY_NON_ZERO_VALUE) {
            function -> values[i] = (int) ((random()) % max_value) + 1;
            function -> non_zeros[non_zeros_index++] = decimal2binary(i, variables); //end of array
        }else
            function -> values[i] = 0;
    }
    function -> variables = variables;
    function -> size = non_zeros_index; //end of array
    REALLOC(function -> non_zeros, sizeof(bvector) * non_zeros_index, ;);
    return function;
}

/**
 * returns the output of the function with the given input (binary)
 * @param f A pointer to the boolean plus function
 * @param input The input to the function given as a vector of bool
 * @return The output of the function
 */
int fplus_value_of(fplus_t* f, bool input[]){
    return f -> values[binary2decimal(input, f -> variables)];
}

/**
 * returns the output of the function at the given input (decimal)
 * @param f A pointer to the boolean plus function
 * @param index The input, given as decimal
 * @return The output of the function
 */
int fplus_value_at(fplus_t* f, int index){
    return f -> values[index];
}

/**
 * Prints function as Karnaugh map if n of variables = 4
 * if n != 4 will print the function as a matrix
 */
void fplus_print(fplus_t* f){
    if(f -> variables != 4){
        long size = (long) exp2l(f -> variables / 2);
        printf("Function table: \n");
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                printf("%d\t", f -> values[(i * size) + j]);
            }
            printf("\n");
        }
        return;
    }
    assert(f -> variables == 4);
    int matrix[4][4];

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(i < 2)
                if(j < 2)
                    matrix[j][i] = f -> values[i * 4 + j];
                else
                    matrix[4 - (j + 1)/2][i] = f -> values[i * 4 + j];
            else if(j < 2)
                matrix[j][4 - (i + 1)/2] = f -> values[i * 4 + j];
            else
                matrix[4 - (j + 1)/2][4 - (i + 1)/2] = f -> values[i * 4 + j];
        }
    }

    printf("Karnaugh map: \n");
    for(int i = 0; i < f -> variables; i++) {
        for (int j = 0; j < f -> variables; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
}

/**
 * Frees the memory used for the function
 * @param f The function to destroy
 */
void fplus_destroy(fplus_t* f){
    for(int i = 0; i < f -> size; i++){
        free(f -> non_zeros[i]);
    }
    free(f -> non_zeros);
    free(f -> values);
    free(f);
}

/**
 * Creates a copy of the given boolean plus function
 * Note: The copy shares with the original the actual values
 * of the bvector in f->non_zeros (the pointer is the same)
 * @param f function to copy
 * @return A new copy of the function
 */
fplus_t* fplus_copy(fplus_t* f){
    fplus_t* f_copy;
    MALLOC(f_copy, sizeof(fplus_t), ;);
    f_copy -> variables = f -> variables;
    f_copy -> size = f -> size;
    int values_size = (int) exp2(f -> variables);
    MALLOC(f_copy -> values, sizeof(int) * values_size, free(f_copy););
    MALLOC(f_copy -> non_zeros, sizeof(bool*) * f -> size, free(f_copy -> values); free(f_copy););
    memcpy(f_copy -> values, f -> values, sizeof(int) * values_size);
    memcpy(f_copy -> non_zeros, f -> non_zeros, sizeof(bool*) * f -> size);
    return f_copy;
}

/**
 * Frees the memory used by the function
 * @param f A function created with the method fplus_copy
 */
void fplus_copy_destroy(fplus_t* f){
    free(f -> non_zeros);
    free(f -> values);
    free(f);
}


/**
 * Adds increment to the output of the function when index is the input
 * If new output is below 0, the input becomes a don't care point
 * @param f The function
 * @param index The input to which change the output
 * @param increment The increment (may be negative)
 */
void fplus_add2value(fplus_t* f, int index, int increment){
    if(f -> values[index] <= -increment) {
        f -> values[index] = F_DONT_CARE_VALUE;
    } else {
        f->values[index] += increment;
    }
}

/**
 * Subtracts the decrement to the output of the function
 * given the input as decimal. If a value <= 0 is reached
 * the point is set as a don't care point
 * @param f The function
 * @param index The input given as a decimal
 * @param decrement The amount to decrement
 */
void fplus_sub2value(fplus_t* f, int index, int decrement){
    f -> values[index] -= decrement;
    if(f -> values[index] <= 0) {
        f->values[index] = F_DONT_CARE_VALUE;
    }
}

/**
 * Checks if the non zero points array is consistent with the f values, updates the array if not
 * @param f The fplus function
 */
void fplus_update_non_zeros(fplus_t* f){
    int index_removed = 0;
    int i = 0;
    while(i < f -> size - index_removed) {
        int index = binary2decimal(f->non_zeros[i], f->variables);
        if (f -> values[index] == 0) {
            index_removed++;
            f -> non_zeros[i] = f -> non_zeros[f -> size - index_removed];
        }else
            i++;
    }
    f -> size -= index_removed;
}

/**
 * Creates a sopp with a default size
 * @return A pointer to the sopp
 */
sopp_t* sopp_create(){
    return sopp_create_wsize(INIT_SIZE);
}

/**
 * Tries to create the sopp with a table of the suggested size.
 * Note: sopp table will have at most suggested_size / good_load constant, possibly lower
 * Creates the sopp form keeping in mind the expected number of products
 * @param expected_size The expected number of products
 * @return A pointer to the sopp
 */
sopp_t* sopp_create_wsize(size_t expected_size){
    sopp_t* sopp;
    MALLOC(sopp, sizeof(sopp_t), ;);
    int good_size = (int) (expected_size / GOOD_LOAD) + 1; //always greater than 0
    while((sopp -> table = malloc(sizeof(list_t*) * good_size)) == NULL)
        good_size /= 2;
    memset(sopp -> table, 0, sizeof(list_t*) * good_size);
    sopp -> table_size = good_size;
    sopp -> current_length = 0;
    NULL_CHECK(sopp -> array = list_create());
    return sopp;
}

/**
 * Frees the memory used by the sopp form
 * @param sopp The sopp form
 */
void sopp_destroy(sopp_t* sopp){
    size_t size;
    productp_t** a = list_as_array(sopp -> array, &size);
    for(size_t i = 0; i < size; i++){
        productp_destroy(a[i]);
    }

    list_destroy(sopp -> array);
    for(size_t i = 0; i < sopp -> table_size; i++){
        if(sopp -> table[i] != NULL)
            list_destroy(sopp -> table[i]);
    }
    free(sopp -> table);
    free(sopp);
}

/**
 * Adds a product plus to the sopp form, if the product is already in the form,
 * its coefficient is updated. The product is always copied before storage
 * @param sopp The sopp form
 * @param p The product plus to add
 * @return The outcome of the operation
 */
bool sopp_add(sopp_t* sopp, productp_t* p){
    unsigned long hashcode = product_hashcode(p) % sopp -> table_size;

    if((double) (sopp -> current_length + 1) / sopp -> table_size > GOOD_LOAD)
        fprintf(stdout, "Warning: sopp table is exceeding good load, current size is %ld while max size is %ld\n", sopp -> current_length, sopp -> table_size);

    productp_t* product = productp_copy(p);

    //if list is empty
    if(sopp -> table[hashcode] == NULL){
        NULL_CHECK(sopp -> table[hashcode] = list_create());
        list_add(sopp -> table[hashcode], product, sizeof(productp_t*));
        list_add(sopp -> array, product, sizeof(productp_t*));
        sopp -> current_length++;
        return true;
    }

    //list not empty, check existence
    size_t size;
    productp_t** array = list_as_array(sopp -> table[hashcode], &size);
    size_t i = 0;
    while(i < size && !bvector_equals(array[i] -> product -> product, product -> product -> product, product -> product -> variables))
        i++;
    if(i < size) {
        //element was found, update value
        array[i]->coeff += product->coeff;
        productp_destroy(product);
        return true;
    }
    //element was not found
    list_add(sopp -> table[hashcode], product, sizeof(productp_t*));
    list_add(sopp -> array, product, sizeof(productp_t*));
    sopp -> current_length++;
    return true;
}

/**
 * Given a product plus, it returns an hashcode based on its values
 * semi stolen from java, may be improved
 * @param p The product plus
 * @return The hash code
 */
long product_hashcode(productp_t *p) {
    long hashcode = 0;
    int n = p -> product -> variables;
    for(int i = 0; i < n; i++){
        hashcode += (long) pow(p -> product -> product[i] * 31, n - i);
    }

    return hashcode;
}

/**
 * Checks if the sop plus for the given input equals the value
 * @return true if the output matches
 */
int sopp_value_of(sopp_t* sop, bool input[]){
    int sopp_value = 0;
    size_t size = 0;
    productp_t** list = list_as_array(sop -> array, &size);
    for(size_t i = 0; i < size; i++){
        productp_t* current_p = list[i];
        sopp_value += current_p -> coeff * product_of(current_p -> product, input);
    }
    return sopp_value;
}


/**
 * Checks if sopp_t is a correct sop plus form of the function fun
 * time: exponential in number of variables
 * @return true it is valid
 */
bool sopp_form_of(sopp_t* sopp, fplus_t* fun){
    int length = fun -> variables;
    bool value[length];
    memset(value, 0, sizeof(bool) * length);
    bool result = true;
    sopp_binaries(value, length, sopp, fun, &result);
    return result;
}

/*
 * rec-fun called by sopp_form_of
 *
 * generates all the possible combinations of the variables
 * and checks if the non zero values of the fun matches the non zero values
 * of the sop plus form
 */
void sopp_binaries(bool *value, int i, sopp_t* sop, fplus_t* fun, bool* result) {
    if(*result) {
        if (i == 0) {
            int fvalue = fplus_value_of(fun, value);
            *result = *result && (fvalue == 0 || sopp_value_of(sop, value) >= fvalue);
        } else {
            value[i - 1] = 0;
            sopp_binaries(value, i - 1, sop, fun, result);
            value[i - 1] = 1;
            sopp_binaries(value, i - 1, sop, fun, result);
        }
    }
}

/**
 * Checks if dsopp_t is a correct disjoint sop plus form of the function fun
 * time: exponential in number of variables
 * @return true it is valid
 */
bool dsopp_form_of(dsopp_t* sopp, fplus_t* fun){
    int length = fun -> variables;
    bool value[length];
    memset(value, 0, sizeof(bool) * length);
    bool result = true;
    dsopp_binaries(value, length, sopp, fun, &result);
    return result;
}

/*
 * rec-fun called by dsopp_form_of
 *
 * generates all the possible combinations of the variables
 * and checks if the values of the fun matches the values
 * of the disjoint sop plus form
 */
void dsopp_binaries(bool *value, int i, dsopp_t* sop, fplus_t* fun, bool* result) {
    if(*result) {
        if (i == 0) {
            int fvalue = fplus_value_of(fun, value);
            *result = *result && sopp_value_of(sop, value) == fvalue;
        } else {
            value[i - 1] = 0;
            sopp_binaries(value, i - 1, sop, fun, result);
            value[i - 1] = 1;
            sopp_binaries(value, i - 1, sop, fun, result);
        }
    }
}

/**
 * Prints the values in the sopp form
 * @param s The sopp form
 */
void sopp_print(sopp_t* s){
    size_t size;
    productp_t** array = list_as_array(s -> array, &size);
    printf("Sopp form is: \n");
    for(size_t i = 0; i < size; i++){
        printf("Product has coeff: %d and is: ", (array[i]) -> coeff);
        for(int j = 0; j < array[0] -> product -> variables; j++){
            if(array[i] -> product -> product[j] > 1)
                printf("- ");
            else
                printf("%d ", array[i] -> product -> product[j]);
        }
        printf("\n");
    }
}

/**
 * Prints the values in the dsopp form
 * @param d The dsopp form
 */
void dsopp_print(sopp_t* d){
    size_t size;
    productp_t** array = list_as_array(d -> array, &size);
    printf("Dsopp form is: \n");
    for(size_t i = 0; i < size; i++){
        printf("Product has coeff: %d and is: ", (array[i]) -> coeff);
        for(int j = 0; j < array[0] -> product -> variables; j++){
            if(array[i] -> product -> product[j] > 1)
                printf("- ");
            else
                printf("%d ", array[i] -> product -> product[j]);
        }
        printf("\n");
    }
}

/**
 * Calculates a minimal sopp form for the given function
 * sopp form is minimal <=> the sum of its coefficients is minimal
 * @param f A fplus function
 * @return The minimal sopp form
 */
sopp_t* sopp_synthesis(fplus_t* f){
    sopp_t* sopp; //will store the minimal sopp form
    implicantp_t* implicants; //will store prime implicants
    bool go_on;

    NULL_CHECK(sopp = sopp_create_wsize(f -> size));
    NULL_CHECK(implicants = prime_implicants(f));

    fplus_t* f_copy = fplus_copy(f);
    implicantp_t* i_copy = implicants_copy(implicants);

    essentialsp_t* e;
    do {
        if((e = essential_implicants(f_copy, i_copy)) == NULL)
            break;
        for (int i = 0; i < e -> impl_size; i++) {
            int max = 0;
            for (int j = 0; j < e -> points_size; j++) {
                if (product_of(e -> implicants[i]->product, e->points[j])) {
                    int cur_value = fplus_value_of(f_copy , e->points[j]);
                    if (cur_value > max)
                        max = cur_value;
                }
            }
            int old_coeff = e -> implicants[i] -> coeff;
            e -> implicants[i] -> coeff = max;
            sopp_add(sopp, e -> implicants[i]);
            e -> implicants[i] -> coeff = old_coeff;
        }

        //for each implicant chosen update f values
        productp_t** impls = list_as_array(sopp -> array, NULL);
        for(size_t i = 0; i < sopp -> current_length; i++) {
            int size;
            int *indexes = binary2decimals(impls[i] -> product -> product, impls[i] -> product -> variables, &size);
            for (int j = 0; j < size; j++) {
                fplus_sub2value(f_copy, indexes[j], impls[i]->coeff);
            }
            free(indexes);
        }

        fplus_update_non_zeros(f_copy);
        implicantp_t *new_implicants = prime_implicants(f_copy);
        go_on = remove_implicant_duplicates_old(i_copy, new_implicants, f_copy) && i_copy -> size > 0;
        implicants_soft_destroy(new_implicants);

        essentials_destroy(e);
    }while(go_on);

    while(i_copy -> size > 0) {
        int min = INT_MAX;
        int implicant_chosen = -1;

        //select the implicant to add
        for (int i = 0; i < i_copy->size; i++) {
            int max = 0;
            int size;
            int *indexes = binary2decimals(i_copy->implicants[i], i_copy->variables, &size);
            for (int j = 0; j < size; j++) {
                int c_value = fplus_value_at(f_copy, indexes[j]);
                if (c_value > 0 && c_value > max) {
                    max = c_value;
                }
            }
            if (max > 0 && max < min) {
                min = max;
                implicant_chosen = i;
            }
            free(indexes);
        }
        if(implicant_chosen == -1)
            break;
        productp_t* p = productp_create(i_copy -> implicants[implicant_chosen], i_copy -> variables, min);

        //change the dashed into not_present
        for(int k = 0; k < f -> variables; k++)
            if(p -> product -> product[k] == dash)
                p -> product -> product[k] = not_present;
        sopp_add(sopp, p);
        productp_destroy(p);

        int size;
        int *indexes = binary2decimals(i_copy -> implicants[implicant_chosen], i_copy -> variables, &size);
        for (int j = 0; j < size; j++)
            fplus_sub2value(f_copy, indexes[j], min);
        free(indexes);

        //NEW TODO: OK?
        int removed = 0;
        int i = 0;
        while(i < i_copy->size - removed){
            indexes = binary2decimals(i_copy->implicants[i], f->variables, &size);
            bool removable = true;
            int j = 0;
            while(removable && j < size){
                removable = fplus_value_at(f, indexes[j]) == F_DONT_CARE_VALUE;
                j++;
            }
            if(removable){
                removed++;
                free(i_copy->implicants[i]);
                i_copy->implicants[i] = i_copy->implicants[i_copy->size - removed];
            } else
                i++;
            free(indexes);
        }

        i_copy->size -= removed;
//        implicantp_t *new_implicants = prime_implicants(f_copy);
//        remove_implicant_duplicates(i_copy, new_implicants, f_copy);
//        implicants_soft_destroy(new_implicants);
    }

    //clean up
    implicants_destroy(implicants);
    implicants_destroy(i_copy);
    fplus_copy_destroy(f_copy);

    return sopp;
}

/**
 * Standard partition function for quicksort adapted to a bvector
 * If norms != NULL, the norm of the vectors are stored at the corresponding
 * index of the vector
 */
int partition (bvector* arr, int low, int high, int variables, int* norms){
    // pivot (Element to be placed at right position)
    bvector pivot = arr[high];
    int norm_pivot = norm1(pivot, variables); //get norm1 of vector

    int i = (low - 1);  // Index of smaller element

    for (int j = low; j < high; j++) {
        int norm_other = norm1(arr[j], variables);

        // If current element is smaller than the pivot
        if(norm_other < norm_pivot){
            i++;    // increment index of smaller element
            bvector tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            if(norms)
                norms[i] = norm_other;
        } else if(norms)
            norms[j] = norm_other;
    }

    bvector tmp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = tmp;
    if(norms) {
        norms[high] = norm1(arr[i + 1], variables);
        norms[i + 1] = norm_pivot;
    }
    return i + 1;
}

/**
 * custom quicksort, compares a array of bool vectors using their norm 1.
 * For each element e, it stores in norms the sum(e(i)) for later use
 * @param arr The bvector array to sort
 * @param low The lowest index (0)
 * @param high The highest index (size - 1)
 * @param variables The number of variables of each bvector
 * @param norms If != NULL will be an array storing the norms of the vectors
 */
void my_quicksort(bvector* arr, int low, int high, int variables, int* norms) {
    if (low < high) {
        int pi = partition(arr, low, high, variables, norms);
        my_quicksort(arr, low, pi - 1, variables, norms);  // Before pi
        my_quicksort(arr, pi + 1, high, variables, norms); // After pi
    }
}

/**
 * Frees a given heap pointer
 * Used to call list_for_each
 * @param p The pointer
 * @param s The size of the memory, will be set to 0 after free
 * @return always true
 */
int free_f(void* p, size_t* s){
    free(p);
    *s = 0;
    return 1;
}

/**
 * Quine–McCluskey algorithm for finding prime implicants
 * //TODO: leaves room for improvement
 * @param f
 * @return
 */
implicantp_t* prime_implicants(fplus_t* f) {
    size_t non_zeros_size = f->size;
    bvector *result = NULL; //will store prime implicants
    size_t result_size = 0; //will store number of prime implicants
    list_t *old_list_2free = NULL;
    bvector *non_zeros;
    MALLOC(non_zeros, sizeof(bvector) * non_zeros_size, ;);
    memcpy(non_zeros, f->non_zeros, sizeof(bvector) * non_zeros_size);

    if(non_zeros_size > 0) {
        while (true) {
            int norms[non_zeros_size]; //will contain norm of each vector
            //sort non zero values
            my_quicksort(non_zeros, 0, (int) non_zeros_size - 1, f->variables, norms);

            bool taken[non_zeros_size]; // stores if the corresponding element inside non_zeros has been joined at least one time with another
            memset(taken, 0, sizeof(bool) * non_zeros_size);

            list_t *impl_found = list_create();

            //join matching vectors
            for (size_t i = 0; i < non_zeros_size; i++) {
                size_t j = i + 1;
                int current_elem_class = norms[i];
                int class_has_changed = false;
                while (!class_has_changed && j < non_zeros_size) {
                    if (norms[j] == current_elem_class)
                        j++;
                    else if (norms[j] > current_elem_class + 1)
                        class_has_changed = true;
                    else {
                        bvector elem = joinable_vectors(non_zeros[i], non_zeros[j], f->variables);
                        if (elem) {
                            taken[i] = true;
                            taken[j] = true;
                            list_add(impl_found, elem, sizeof(bvector));
                        }
                        j++;
                    }
                }
            }

            //case last cycle
            if (list_length(impl_found) == 0) {
                if (!old_list_2free) {
                    //only one cycle was made
                    MALLOC(result, sizeof(bvector) * non_zeros_size, ;);
                    for (size_t i = 0; i < non_zeros_size; i++) {
                        MALLOC(result[i], sizeof(bool) * f->variables, free(result);
                                free(non_zeros));
                        memcpy(result[i], non_zeros[i], sizeof(bool) * f->variables);
                    }
                    free(non_zeros);
                } else {
                    MALLOC(result, sizeof(bvector) * non_zeros_size, ;);
                    memcpy(result, non_zeros, sizeof(bvector) * non_zeros_size);
                    list_destroy(old_list_2free);
                }
                result_size = non_zeros_size;
                list_destroy(impl_found);
                break;
            }

            //add implicants that have not been joined
            for (size_t i = 0; i < non_zeros_size; i++) {
                if (!taken[i]) {
                    bvector b;
                    MALLOC(b, sizeof(bool) * f->variables, ;); //TODO: add more clean up
                    memcpy(b, non_zeros[i], sizeof(bool) * f->variables);
                    list_add(impl_found, b, sizeof(bvector));
                }
            }

            //free list used in last cycle
            if (old_list_2free) {
                list_for_each(old_list_2free, free_f);
                list_destroy(old_list_2free);
            } else
                //if first cycle
                free(non_zeros);

            old_list_2free = impl_found;

            //set as new array the joint implicants found
            non_zeros = list_as_array(impl_found, &non_zeros_size);

            //delete duplicates
            int duplicates_found = 0;
            for (size_t i = 0; i < non_zeros_size - 1 - duplicates_found; i++) {
                for (size_t j = i + 1; j < non_zeros_size - duplicates_found; j++) {
                    if (bvector_equals(non_zeros[i], non_zeros[j], f->variables)) {
                        duplicates_found++;
                        free(non_zeros[j]);
                        non_zeros[j] = non_zeros[non_zeros_size - duplicates_found];
                        non_zeros[non_zeros_size - duplicates_found] = NULL;
                        j--; //recheck current element
                    }
                }
            }
            non_zeros_size -= duplicates_found;


        }

        //delete duplicates
        int duplicates_found = 0;
        for (int i = 0; i < result_size - 1 - duplicates_found; i++) {
            for (int j = i + 1; j < result_size - duplicates_found; j++) {
                if (bvector_equals(result[i], result[j], f->variables)) {
                    duplicates_found++;
                    free(result[j]);
                    result[j] = result[result_size - duplicates_found];
                    result[result_size - duplicates_found] = NULL;
                    j--;
                }
            }
        }
        result_size -= duplicates_found;
    }

    //create implicants object
    REALLOC(result, sizeof(bvector) * result_size, ;);
    implicantp_t* implicants;
    MALLOC(implicants, sizeof(implicantp_t), free(result)); //TODO: should clean more stuff
    implicants -> implicants = result;
    implicants -> size = result_size;
    implicants -> variables = f -> variables;

    return implicants;
}

/**
 * Checks if the two vectors are joinable
 * Two vectors v1, v2 are joinable <=> Exists j / v1(j) = !v2(j) and for each i != j v1(i) = v2(i)
 * The joined vector will be equal to v1(or v2) for each i != j and will a dash value for i = j
 * @param size The size of the vectors
 * @return The joined vector (in heap) if possible, NULL otherwise
 */
bvector joinable_vectors(const bool* v1, const bool* v2, int size){
    bool* join;
    MALLOC(join, sizeof(bool) * size, ;);

    int counter = 0; //counts the complimentary bits found
    for(int i = 0; i < size; i++){
        if(v1[i] == v2[i]){
            join[i] = v1[i];
        }else if(v1[i] == dash || v2[i] == dash) {
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
 * Frees the memory used by implicants plus
 * @param impl The implicants to free
 */
void implicants_destroy(implicantp_t* impl){
    for(int i = 0; i < impl -> size; i++){
        free(impl -> implicants[i]);
    }
    free(impl -> implicants);
    free(impl);
}

/**
 * Frees the memory used by an implicant leaving the memory
 * used by each element of impl->implicants
 */
void implicants_soft_destroy(implicantp_t* impl){
    free(impl -> implicants);
    free(impl);
}

/**
 * Prints the implicants
 */
void implicants_print(implicantp_t* impl){
    printf("Final implicants: \n");
    for(int i = 0; i < impl -> size; i++){
        for(int j = 0; j < impl -> variables; j++){
            if(impl -> implicants[i][j] > 1)
                printf("-\t");
            else
                printf("%d\t", impl -> implicants[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Finds the essential implicants of the function from the prime implicants
 * !!!Exponential in space!!!
 * @return a pointer to a struct containing the essential points and the essential prime implicants
 */
essentialsp_t* essential_implicants(fplus_t* f, implicantp_t* implicants){
    long f_size = (long) exp2l(f -> variables); //exp size unnecessary as only non_zero points are needed
    list_t* points[f_size]; //each index represent a point of f, the list will contain the implicants covering that point
    bvector essential_implicants[f_size]; //stores the essential prime implicants
    int e_index = 0; //index of above and below array;
    bvector* essential_points; //stores the essential points
    MALLOC(essential_points, sizeof(bvector) * f_size, ;);

    //init the array of points
    for(size_t i = 0; i < f_size; i++){
        points[i] = list_create();
    }

    //count occurrences of various points
    for(int i = 0; i < implicants -> size; i++){
        int size = 0;
        int* indexes = binary2decimals(implicants -> implicants[i], f -> variables, &size);
        for(int j = 0; j < size; j++){
            list_add(points[indexes[j]], implicants -> implicants[i], sizeof(bvector));
        }
        free(indexes);
    }

    //store points and implicants in array
    for(int i = 0; i < f -> size; i++){
        int index = binary2decimal(f -> non_zeros[i], f -> variables);
        int l_size = list_length(points[index]);
        if(l_size == 0){
            fprintf(stderr, "Some error occurred, an essential point has not been covered\n");
        }else if(l_size == 1){
            essential_implicants[e_index] = list_get(points[index], 0, 0);
            essential_points[e_index++] = f -> non_zeros[i];
        }
    }

    //if no essential points are found
    if(e_index == 0) {
        free(essential_points);
        essential_points = NULL;
    } else
        REALLOC(essential_points, sizeof(bvector) * e_index, ;);

    essentialsp_t* e;
    MALLOC(e, sizeof(essentialsp_t), free(essential_points););
    e -> implicants = implicants2sop(essential_implicants, e_index, f -> variables, &e -> impl_size);
    e -> points = essential_points;
    e -> points_size = e_index;

    for(size_t i = 0; i < f_size; i++) {
        list_destroy(points[i]);
    }
    return e;
}

/**
 * Converts a boolean vector array to a product array
 * Also removes duplicates if found (O(n^2), may be improved)
 * @param final_size Sets in this variable the size without the duplicates
 * @return The newly created array
 */
productp_t** implicants2sop(bvector* implicants, int size, int variables, int* final_size){
    int duplicates[size];
    int duplicates_found = 0;
    memset(duplicates, 0, sizeof(int) * size);

    //tag duplicates
    for(int i = 0; i < size - 1; i++){
        if(!duplicates[i])
            for(int j = i + 1; j < size; j++) {
                if (bvector_equals(implicants[i], implicants[j], variables)) {
                    if (!duplicates[j]) {
                        duplicates[j] = true;
                        duplicates_found++;
                    }
                }
            }
    }

    productp_t** product_array;
    MALLOC(product_array, sizeof(productp_t*) * (size - duplicates_found), ;);
    int array_index = 0;

    //copy only non duplicates
    for(int i = 0; i < size; i++){
        if(!duplicates[i]) {
            for (int j = 0; j < variables; j++) {
                if (implicants[i][j] == dash) //write in new variable
                    implicants[i][j] = not_present;
            }
            product_array[array_index] = productp_create(implicants[i], variables, 1);
            array_index++;
            for (int j = 0; j < variables; j++) {
                if (implicants[i][j] == not_present) //write in new variable
                    implicants[i][j] = dash;
            }
        }
    }

    *final_size = size - duplicates_found;

    return product_array;
}

/**
 * Frees the memory used by the essentials form
 * @param e The essential form
 */
void essentials_destroy(essentialsp_t* e){
    for(int i = 0; i < e -> impl_size; i++)
        productp_destroy(e -> implicants[i]);
    free(e -> implicants);
    free(e -> points);
    free(e);
}

/**
 * Prints the essential implicants and points
 * @param e The essentials
 * @param variables The number of variables
 */
void essentials_print(essentialsp_t* e, int variables){
    printf("Essential implicants: \n");
    for(int i = 0; i < e -> impl_size; i++){
        for(int j = 0; j < variables; j++){
            printf("%d\t", e -> implicants[i] -> product -> product[j]);
        }
        printf("\n");
    }
    printf("\n");

    printf("Essential points: \n");
    for(int i = 0; i < e -> points_size; i++){
        for(int j = 0; j < variables; j++){
            printf("%d\t", e -> points[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Frees the memory used by the product plus
 * @param p The product plus
 */
void productp_destroy(productp_t *p) {
    free(p -> product -> product);
    free(p -> product);
    free(p);
}

/**
 * Creates a product plus
 * @param b A vector with the variables in the product (b[i] = true <=> variable is in the product)
 * @param variables Number of total variables (also size of above vector)
 * @param coeff coefficient of the product
 * @return A pointer to the product
 */
productp_t* productp_create(bool* b, int variables, int coeff){
    productp_t* p;
    MALLOC(p, sizeof(productp_t), ;);
    p -> product = product_create(b, variables);
    p -> coeff = coeff;
    return p;
}

/**
 * @return Given a product p, returns an exact copy
 */
productp_t* productp_copy(productp_t* p){
    productp_t* p_copy;
    MALLOC(p_copy, sizeof(productp_t), ;);
    p_copy -> product = product_create(p -> product -> product, p -> product -> variables);
    p_copy -> coeff = p -> coeff;
    return p_copy;
}


/**
 * Creates a copy of the given implicant.
 * @return copy of the implicants passed as parameters
 */
implicantp_t* implicants_copy(implicantp_t* impl){
    implicantp_t* i_copy;
    MALLOC(i_copy, sizeof(implicantp_t),;);
    i_copy -> size = impl -> size;
    i_copy -> variables = impl -> variables;
    MALLOC(i_copy -> implicants, sizeof(bool*) * impl -> size, free(i_copy));
    for(int i = 0; i < impl -> size; i++){
        MALLOC(i_copy -> implicants[i], sizeof(bool) * impl -> variables, ;);//TODO: improve clean
        memcpy(i_copy -> implicants[i], impl -> implicants[i], sizeof(bool) * impl -> variables);
    }
    return i_copy;
}

/*
 * An attempt to optimize below function.
 * Although not very successful as new function doesn't behave
 * the same.
 */
bool remove_implicant_duplicates(implicantp_t* source, implicantp_t* to_remove, fplus_t* f){
    //join without duplicates

    //not the proper sort but good enough
    my_quicksort(source->implicants, 0, source->size - 1, f->variables, NULL);
    my_quicksort(to_remove->implicants, 0, to_remove->size - 1, f->variables, NULL);
    bvector* joint_array;
    MALLOC(joint_array, sizeof(bvector*) * (source->size + to_remove->size), ;);
    int joint_index = 0;
    int to_remove_index = 0;
    int source_index = 0;
    while(joint_index < source->size + to_remove->size && (to_remove_index < to_remove->size || source_index < source->size)){
        int v1 = INT_MAX;
        int v2 = INT_MAX;
        if(source_index < source->size)
            v1 = norm1(source->implicants[source_index], f->variables);
        if(to_remove_index < to_remove->size)
            v2 = norm1(to_remove->implicants[to_remove_index], f->variables);
        if(v1 < v2){
            joint_array[joint_index] = source->implicants[source_index];
            source_index++;
        }else if(v1 == v2 && bvector_equals(source->implicants[source_index], to_remove->implicants[to_remove_index], f->variables)) {
            free(source->implicants[source_index]);
            source_index++;
            joint_array[joint_index] = to_remove->implicants[to_remove_index];
            to_remove_index++;
        }else{
            //includes when v2 < v1 and when v2 = v1 but the vectors are different
            joint_array[joint_index] = to_remove->implicants[to_remove_index];
            to_remove_index++;
        }
        joint_index++;
    }
    REALLOC(joint_array, sizeof(bvector) * joint_index, ;);

    int removed = 0;
    int i = 0;
    while(i < joint_index - removed){
        int size;
        int* indexes = binary2decimals(joint_array[i], f->variables, &size);
        bool removable = true;
        int j = 0;
        while(removable && j < size){
            removable = fplus_value_at(f, indexes[j]) == F_DONT_CARE_VALUE;
            j++;
        }
        if(removable){
            removed++;
            free(joint_array[i]);
            joint_array[i] = joint_array[joint_index - removed];
        } else
            i++;
        free(indexes);
    }

    bool return_value = removed > 0 || joint_index < source->size + to_remove->size;

    source->size = joint_index - removed;
    source->implicants = joint_array;

    return return_value;
}

/**
 * Joins source and to_remove while removing the implicants covering the same non zero points
 * @param source The original implicants. May have some implicants covering 0 or don't care point
 * @param to_remove New implicants, covering only non zero points
 * @param f The boolean plus function
 * @return true if at least an implicant in source has been removed
 */
bool remove_implicant_duplicates_old(implicantp_t* source, implicantp_t* to_remove, fplus_t* f){
    int removed = 0; //stores the number of removed implicants from source
    int non_zero_values = 0; //stores the number of non zero values encountered. if = 0 => no need to go on

    /*
     * for each implicant in to_remove checks if it covers all the non_zero points
     * of a implicant in source. In that case the implicant in source is removed     *
     */
    for(int i = 0; i < to_remove -> size; i++){
        //get all the points covered by the implicant in to_remove
        int r_size; //remove size
        int* r_indexes = binary2decimals(to_remove -> implicants[i], to_remove -> variables, &r_size);
        int j = 0;
        while(j < source -> size - removed){
            //get all the points covered by the implicant in source
            int s_size; //source zie
            int* s_indexes = binary2decimals(source -> implicants[j], source -> variables, &s_size);

            //if all non_zero points of s are covered by r
            if(r_cover_s(r_indexes, r_size, s_indexes, s_size, f, &non_zero_values)){
                removed++;
                free(source->implicants[j]);
                source -> implicants[j] = source -> implicants[source->size - removed];
            }else
                j++;
            free(s_indexes);
        }
        free(r_indexes);
    }
    source -> size -= removed;

    //add all implicants in to_remove to source
    REALLOC(source -> implicants, sizeof(bvector) * (source -> size + to_remove -> size), ;);
    for(int i = source -> size; i < source -> size + to_remove -> size; i++){
        source -> implicants[i + source -> size] = to_remove -> implicants[i];
    }
    source -> size += to_remove -> size;
    if(source -> size == to_remove -> size) {
        return false;
    }
    return non_zero_values > 0;
}

/**
 * Returns true if all non_zero points of s are covered by r
 * @param r_indexes The points of r as decimal
 * @param r_size The number of points of r
 * @param s_indexes The points of s as decimal
 * @param s_size The number of points of s
 * @param f The function which output to check
 * @param non_zero_values Will store the number of non-zero values found
 * @return
 */
bool r_cover_s(int *r_indexes, int r_size, int *s_indexes, int s_size, fplus_t *f, int* non_zero_values) {
    int i = 0; //index of r_indexes
    for(int k = 0; k < s_size; k++){
        if(fplus_value_at(f, s_indexes[k]) > 0){
            (*non_zero_values)++;
            while(i < r_size && r_indexes[i] > s_indexes[k])
                i++;
            if(i == r_size || r_indexes[i] != s_indexes[k]) {
                return false;
            }
        }
    }
}


/**
 * Calculates a minimal dsopp form for the given function
 * dsopp form is minimal <=> the sum of its coefficients is minimal
 * @param f A fplus function
 * @return The minimal dsopp form
 */
dsopp_t* dsopp_synthesis(fplus_t* f){
    dsopp_t* dsopp = sopp_create_wsize(f -> size);
    NULL_CHECK(dsopp);
    sopp_t* sopp = sopp_synthesis(f);
    NULL_CHECK(sopp);
    heap_t* heap = heap_create_wsize(sopp->current_length);
    NULL_CHECK(heap);
    fplus_t* f_copy = fplus_copy(f);

    while(f_copy->size > 0) {
        productp_t **products = list_as_array(sopp->array, NULL);
        for (int i = 0; i < sopp->current_length; i++) {
            int size;
            //TODO: cache indexes
            int *indexes = binary2decimals(products[i]->product->product, f->variables, &size);
            int min = INT_MAX;
            for (int j = 0; j < size; j++) {
                int fvalue = fplus_value_at(f_copy, indexes[j]);
                if (fvalue < min && fvalue != F_DONT_CARE_VALUE)
                    min = fvalue;
            }
            if(min != INT_MAX)
                heap_insert(heap, products[i], min);
            free(indexes);
        }
        if(heap_size(heap) == 0)
            break;
        int max_value;
        productp_t *p;
        while ((p = heap_extract_max(heap, &max_value)) != NULL) {
            int size;
            int *indexes = binary2decimals(p->product->product, f->variables, &size);
            for (int i = 0; i < size; i++)
                fplus_sub2value(f_copy, indexes[i], max_value);
            free(indexes);
            int old_coeff = p->coeff;
            p->coeff = max_value;
            sopp_add(dsopp, p);
            p->coeff = old_coeff;
            heap_delete_useless(heap, f_copy);
        }
        fplus_update_non_zeros(f_copy);
        sopp_destroy(sopp);
        sopp = sopp_synthesis(f_copy);
    }

    heap_destroy(heap);
    fplus_copy_destroy(f_copy);
    sopp_destroy(sopp);
    return dsopp;
}


/**
 * Creates heap with default size
 * @return A pointer to the heap
 */
heap_t* heap_create(){
    return heap_create_wsize(INIT_SIZE);
}

/**
 * Tries to create the heap with the suggested size.
 * Note: Heap will have at most suggested_size, possibily lower
 * @return A pointer to the heap
 */
heap_t* heap_create_wsize(size_t suggested_size){
    heap_t* h;
    int size = suggested_size;
    MALLOC(h, sizeof(heap_t), ;);
    //try to alloc suggested_size
    while((h->products = malloc(sizeof(productp_t*) * size)) == NULL)
        size /= 2; //suggested_size is too big, try lower
    MALLOC(h->minimal_points, sizeof(int) * size, free(h->products); free(h));
    h->current_size = 0;
    h->max_size = size;
    return h;
}

/**
 * Inserts a key into the heap
 * @param h The heap
 * @param k The key
 * @param p The product associated with the key
 * @return true if the insertion was successful,
 *      false in case heap needs to be extended but memory is full
 */
bool heap_insert(heap_t* h, productp_t* p, int k){
    if(h->current_size == h->max_size){
        REALLOC(h->products, sizeof(productp_t*) * h->max_size * 2, ;);
        REALLOC(h->minimal_points, sizeof(int) * h->max_size * 2, ;);
        h->max_size *= 2;
    }
    h->minimal_points[h->current_size] = INT_MIN;
    heap_increase_key(h, p, k, h->current_size);
    h->current_size++;

    return true;
}

/**
 * @return true if p1 has lower literals than p2
 */
bool lower_literals(productp_t* p1, productp_t* p2){
    int sum1 = 0;
    int sum2 = 0;
    for(int i = 0; i < p1->product->variables; i++){
        sum1 += p1->product->product[i] < 2;
        sum2 += p2->product->product[i] < 2;
    }
    return sum1 < sum2;
}

/**
 * Key at index has to be inserted. It is compared to the other and put
 * to the right place. If key is equal to the compared value the greater
 * is the one with the product associated with the least amount of literals
 * @param h The heap
 * @param product The product associated with the key
 * @param value The key
 * @param index The index where the key has to be initially placed
 * @return
 */
bool heap_increase_key(heap_t* h, productp_t* product, int value, int index){
    if(h->minimal_points[index] >= value)
        return false; //error
    h->minimal_points[index] = value;
    h->products[index] = product;
    int parent = HEAP_PARENT(index);

    //while value is greater than parent
    while(index > 0 && (h->minimal_points[parent] < h->minimal_points[index] ||
            (h->minimal_points[parent] == h->minimal_points[index] && lower_literals(h->products[index], h->products[parent])))){
        int tmp = h->minimal_points[index];
        void* tmp_p = h->products[index];
        h->minimal_points[index] = h->minimal_points[parent];
        h->products[index] = h->products[parent];
        h->minimal_points[parent] = tmp;
        h->products[parent] = tmp_p;

        index = parent;
        parent = HEAP_PARENT(index);
    }
}

/**
 * Gets and removes the max from the heap
 * @param h The heap
 * @param value A pointer to integer. Will contain the key
 * @return The product associated with the key
 */
productp_t* heap_extract_max(heap_t* h, int* value) {
    NULL_CHECK(h);
    if(h->current_size == 0)
        return NULL;

    *value = h->minimal_points[0];
    void* return_value = h->products[0];
    h->current_size--;
    h->minimal_points[0] = h->minimal_points[h->current_size];
    h->products[0] = h->products[h->current_size];
    max_heapify(h, 0);

    return return_value;
}

/**
 * Given the index of a misplaced key, it re-balances the heap
 * @param h The heap
 * @param index The index to re-balance
 */
void max_heapify(heap_t* h, int index){
    int l = HEAP_LEFT(index);
    int r = HEAP_RIGHT(index);
    int max = index;
    if(l < h->current_size && h->minimal_points[l] > h->minimal_points[index])
        max = l;
    if(r < h->current_size && h->minimal_points[r] > h->minimal_points[index])
        max = r;
    if(max != index){
        int tmp = h->minimal_points[index];
        void* tmp_p = h->products[index];
        h->minimal_points[index] = h->minimal_points[max];
        h->products[index] = h->products[max];
        h->minimal_points[max] = tmp;
        h->products[max] = tmp_p;
        max_heapify(h, max);
    }
}

/**
 * Free the memory used by the heap
 * @param h The heap
 */
void heap_destroy(heap_t* h){
    if(!h)
        return;
    free(h->products);
    free(h->minimal_points);
    free(h);
}

/**
 * Checks all the elements of the heap deleting
 * the implicants that cover at least one 0 point
 * of the function
 * @param h The heap
 * @param f The function used to check the 0 points
 */
void heap_delete_useless(heap_t* h, fplus_t* f){
    int i = 0;
    while(i < h->current_size){
        int size;
        int* indexes = binary2decimals(h->products[i]->product->product, f->variables, &size);
        bool deletable = false;
        int j = 0;
        while(j < size && !deletable){
            if(fplus_value_at(f, indexes[j]) < 1)
                deletable = true;
            j++;
        }
        free(indexes);
        if(deletable){
            //TODO: free?
            h->current_size--;
            h->minimal_points[i] = h->minimal_points[h->current_size];
            h->products[i] = h->products[h->current_size];
            max_heapify(h, i);
        } else
            i++;
    }
}


/**
 * @param h The heap
 * @return The elements in h
 */
int heap_size(heap_t* h){
    return h->current_size;
}