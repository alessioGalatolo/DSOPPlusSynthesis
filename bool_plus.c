#include "bool_plus.h"
#include "utils.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <math.h>


#define PROBABILITY_NON_ZERO_VALUE 40

//internal functions
void binaries(bool value[], int i, sopp_t *sop, fplus_t* fun, bool *result);
bvector joinable_vectors(const bool*, const bool*, int size);
productp_t* implicants2sop(bvector*, int size, int variables, int* final_size);

void productp_destroy(productp_t *p);

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
    srandom(time(NULL));
    fplus_t* function;
    int f_size = (int) exp2(variables);
    int non_zeros_index = 0;

    MALLOC(function, sizeof(fplus_t), ;);
    MALLOC(function -> values, sizeof(int) * f_size, free(function));
    MALLOC(function -> non_zeros, sizeof(bvector) * f_size, free(function -> values); free(function));

    for(int i = 0; i < f_size; i++){
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
 * returns the output of the function with the given input
 * @param f A pointer to the boolean plus function
 * @param input The input to the function given as a vector of bool
 * @return The output of the function
 */
int fplus_value_of(fplus_t* f, bool input[]){
    return f -> values[binary2decimal(input, f -> variables)];
}

/**
 * Prints function as Karnaugh map
 * Will work only for n = 4
 */
void fplus_print(fplus_t* f){
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

    printf("Function table: \n");
    for(int i = 0; i < f -> variables; i++) {
        for (int j = 0; j < f -> variables; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
}

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

void fplus_add2value(fplus_t* f, int index, int increment){
    if(f -> values[index] <= -increment)
        f -> values[index] = 0;
    else
        f -> values[index] += increment;
}

/**
 * Creates and initializes a sop plus form
 * @return The sopp_t form
 */
sopp_t* sopp_create(){
    sopp_t* sopp;
    MALLOC(sopp, sizeof(sopp_t), ;);
    sopp -> products = list_create();
    sopp -> size = 0;
    return sopp;
}

void sopp_destroy(sopp_t* sopp){
    list_destroy(sopp -> products);
    free(sopp);
}

/**
 * Adds a product to the sop plus form
 * @param sopp The sop plus form
 * @param sop1 The product to add
 * @return true if the operation was successful
 */
int sopp_add(sopp_t* sopp, productp_t* sop1){
    list_add(sopp -> products, sop1, sizeof(sop1)); //TODO: error?
    (sopp -> size)++;
    return sopp -> products == NULL ? 0: 1;
}

/**
 * Checks if the sop plus for the given input equals the value
 * @return true if the output matches
 */
int sopp_value_equals(sopp_t* sop, bool input[], int value){
    int sopp_value = 0;
    size_t size = 0;
    void** list = list_as_array(sop -> products, &size); //sop -> products;
    for(size_t i = 0; i < size; i++){
        productp_t* current_p = list[i];
        sopp_value += current_p -> coeff * product_of(current_p -> product, input);
    }
    return sopp_value == value;
}


/**
 * Checks if sopp_t is a correct sop plus form of the function fun
 * time: exponential in number of variables
 * @return true it is valid
 */
bool sopp_from_of(sopp_t* sopp, fplus_t* fun){
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
void binaries(bool value[], int i, sopp_t* sop, fplus_t* fun, bvector result) {
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


//standard partition function for quicksort
int partition (bvector* arr, int low, int high, int variables, int* norms){
    // pivot (Element to be placed at right position)
    bvector pivot = arr[high];
    int norm_pivot = norm1(pivot, variables);

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
        }
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

/*
 * custom quicksort, compares a array of bool vectors.
 * For each element e, it stores in norms the sum(e(i)) for later use
 */
void quickSort(bvector* arr, int low, int high, int variables, int* norms) {
    if (low < high) {
        /* pi is partitioning index, arr[pi] is now
           at right place */
        int pi = partition(arr, low, high, variables, norms);
        if(norms)
            norms[pi] = norm1(arr[pi], variables);
        quickSort(arr, low, pi - 1, variables, norms);  // Before pi
        quickSort(arr, pi + 1, high, variables, norms); // After pi
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
implicantp_t* prime_implicants(fplus_t* f){
    size_t non_zeros_size = f -> size;
    bvector* result = NULL; //will store prime implicants
    int result_size = 0; //will store number of prime implicants
    list_t* old_list_2free = NULL;
    bvector* non_zeros;
    MALLOC(non_zeros, sizeof(bvector) * non_zeros_size, ;);
    memcpy(non_zeros, f -> non_zeros, sizeof(bvector) * non_zeros_size);


    while(true) {
        int norms[non_zeros_size]; //will contain norm of each vector
        //sort non zero values
        quickSort(non_zeros, 0, (int) non_zeros_size - 1, f -> variables, norms);

        for(size_t i = 0; i < non_zeros_size; i++)
//            assert(norm1(non_zeros[i], f -> variables) == norms[i]);
            norms[i] = norm1(non_zeros[i], f -> variables);
        //TODO: fix norm calculation in quick sort



        bool taken[non_zeros_size]; // stores if the corresponding element inside non_zeros has been joined at least one time with another
        memset(taken, 0, sizeof(bool) * non_zeros_size);

        list_t* impl_found = list_create();

        //assert(norms contains matching norms of non_zeros elements);

        //join matching vectors
        for(size_t i = 0; i < non_zeros_size; i++){
            size_t j = i + 1;
            int current_elem_class = norms[i];
            int class_has_changed = false;
            while(!class_has_changed && j < non_zeros_size){
                if(norms[j] == current_elem_class)
                    j++;
                else if(norms[j] > current_elem_class + 1)
                    class_has_changed = true;
                else{
                    bvector elem = joinable_vectors(non_zeros[i], non_zeros[j], f -> variables);
                    if(elem){
                        taken[i] = true;
                        taken[j] = true;
                        list_add(impl_found, elem, sizeof(bvector));
                    }
                    j++;
                }
            }
        }

        if(list_length(impl_found) == 0){//end of cycle
            if(!old_list_2free){
                //only one cycle was made
                MALLOC(result, sizeof(bvector) * non_zeros_size, ;);
                for(size_t i = 0; i < non_zeros_size; i++) {
                    MALLOC(result[i], sizeof(bool) * f -> variables, free(result); free(non_zeros));
                    memcpy(result[i], non_zeros[i], sizeof(bool) * f -> variables);
                }
                free(non_zeros);
            }else {
                MALLOC(result, sizeof(bvector) * non_zeros_size, ;);
                memcpy(result, non_zeros, sizeof(bvector) * non_zeros_size);
                list_destroy(old_list_2free);
            }
            result_size = non_zeros_size;
            list_destroy(impl_found);
            break;
        }

        //add implicants that have not been joined
        for(size_t i = 0; i < non_zeros_size; i++){
            if(!taken[i]) {
                bvector b;
                MALLOC(b, sizeof(bool) * f -> variables, ;); //TODO: add more clean up
                memcpy(b, non_zeros[i], sizeof(bool) * f -> variables);
                list_add(impl_found, b, sizeof(bvector));
            }
        }

        //free list used in last cycle
        if(old_list_2free) {
            list_for_each(old_list_2free, free_f);
            list_destroy(old_list_2free);
        }else
            //if first cycle
            free(non_zeros);

        old_list_2free = impl_found;

        //set as new array the joint implicants found
        non_zeros = list_as_array(impl_found, &non_zeros_size);
        //delete duplicates
        int duplicates_found = 0;
        for(size_t i = 0; i < non_zeros_size - 1 - duplicates_found; i++){
            for(size_t j = i + 1; j < non_zeros_size - duplicates_found; j++) {
                if (bvector_equals(non_zeros[i], non_zeros[j], f -> variables)) {
                    duplicates_found++;
//                    free(non_zeros[j]); //THIS WILL LEAK MEMORY
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
    for(int i = 0; i < result_size - 1 - duplicates_found; i++){
        for(int j = i + 1; j < result_size - duplicates_found; j++) {
            if (bvector_equals(result[i], result[j], f -> variables)) {
                duplicates_found++;
                free(result[j]);
                result[j] = result[result_size - duplicates_found];
                result[result_size - duplicates_found] = NULL;
                j--;
            }
        }
    }
    result_size -= duplicates_found;

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

void implicants_destroy(implicantp_t* impl){
    for(int i = 0; i < impl -> size; i++){
        free(impl -> implicants[i]);
    }
    free(impl -> implicants);
    free(impl);
}

void implicants_print(implicantp_t* impl, int variables){
    printf("Final implicants: \n");
    for(int i = 0; i < impl -> size; i++){
        for(int j = 0; j < variables; j++){
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
    int f_size = (int) exp2(f -> variables); //exp size unnecessary as only non_zero points are needed
    list_t* points[f_size]; //each index represent a point of f, the list will contain the implicants covering that point
    bvector essential_implicants[implicants -> size]; //stores the essential points
    int e_index = 0; //index of above and below array;
    bvector* essential_points;
    MALLOC(essential_points, sizeof(bvector) * f_size, ;);

    //init the array of points
    for(int i = 0; i < f_size; i++){
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
            fprintf(stderr, "Some error occurred, an essential point has not been covered");
            return NULL;
        }else if(l_size == 1){
            essential_implicants[e_index] = list_get(points[index], 0, 0);
            essential_points[e_index++] = f -> non_zeros[i];
        }
    }

    if(e_index == 0)
        free(essential_points);
    else
        REALLOC(essential_points, sizeof(bvector) * e_index, ;);

    essentialsp_t* e;
    MALLOC(e, sizeof(essentialsp_t), ;);
    e -> implicants = implicants2sop(essential_implicants, e_index, f -> variables, &e -> impl_size);
    e -> points = essential_points;
    e -> points_size = e_index;

//    for(int i = 0; i < f_size; i++) {
//        list_destroy(points[i]);
//    }
    return e;
}

/**
 * Converts a boolean vector array to a product array
 * Also removes duplicates if found (O(n^2)), may be improved
 * @param final_size Sets in this variable the size without the duplicates
 * @return The newly created array
 */
productp_t* implicants2sop(bvector* implicants, int size, int variables, int* final_size){
    int duplicates[size];
    int duplicates_found = 0;
    memset(duplicates, 0, sizeof(int) * size);

    //tag duplicates
    for(int i = 0; i < size - 1; i++){
        for(int j = i + 1; j < size; j++) {
            if (bvector_equals(implicants[i], implicants[j], variables)) {
                if (!duplicates[j]) {
                    duplicates[j] = true;
                    duplicates_found++;
                }
            }
        }
    }

    productp_t* product_array;
    MALLOC(product_array, sizeof(productp_t) * (size - duplicates_found), ;);
    int array_index = 0;

    //copy only non duplicates
    for(int i = 0; i < size; i++){
        if(!duplicates[i]) {
            for (int j = 0; j < variables; j++) {
                if (implicants[i][j] == dash)
                    implicants[i][j] = not_present;
            }
            (product_array + array_index) -> product = product_create(implicants[i], variables);
            (product_array + array_index) -> coeff = 1;
            array_index++;
        }
    }

    *final_size = size - duplicates_found;

    return product_array;
}

void essentials_destroy(essentialsp_t* e){
    for(int i = 0; i < e -> impl_size; i++)
        productp_destroy(e -> implicants + i);
    free(e -> implicants);
    free(e -> points);
    free(e);
}

void essentials_print(essentialsp_t* e, int variables){
    printf("Essential implicants: \n");
    for(int i = 0; i < e -> impl_size; i++){
        for(int j = 0; j < variables; j++){
            printf("%d\t", (e -> implicants + i) -> product -> product[j]);
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

void productp_destroy(productp_t *p) {
    free(p -> product -> product);
    free(p -> product);
}

implicantp_t* implicants_copy(implicantp_t* impl){
    implicantp_t* i_copy;
    MALLOC(i_copy, sizeof(implicantp_t),;);
    i_copy -> size = impl -> size;
    i_copy -> variables = impl -> variables;
    MALLOC(i_copy -> implicants, sizeof(bool*) * impl -> size, free(i_copy));
    memcpy(i_copy -> implicants, impl -> implicants, sizeof(bool*) * impl -> size);
    return i_copy;
}

void remove_implicant_duplicates(implicantp_t* source, implicantp_t* to_remove, fplus_t* f){
    int removed = 0;
    for(int i = 0; i < source -> size - removed; i++){
        for(int j = 0; j < to_remove -> size; j++){
            //remove all prime implicants covering non-zero points all covered by another implicants in to_remove
//            if(bvector_equals(source -> implicants[i], to_remove -> implicants[j], source -> variables)){
//                removed++;
//                free(source -> implicants[i]);
//                source -> implicants[i] = source -> implicants[source -> size - removed];
//            }
        }
    }
    source -> size -= removed;
}