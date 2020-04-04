#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include "bool_plus.h"
#include "utils.h"
#include "linkedlist.h"

//internal functions
void sopp_binaries(bool *value, unsigned i, sopp_t *sop, fplus_t* fun, bool *result);
void dsopp_binaries(bool *value, unsigned i, dsopp_t *sop, fplus_t* fun, bool *result);
bvector joinable_vectors(const bool*, const bool*, unsigned variables);
productp_t** implicants2sop(bvector*, int size, unsigned variables, int* final_size);
long product_hashcode(productp_t *p);
bool r_cover_s(const int *r_indexes, int r_size, int *s_indexes, int s_size, fplus_t *f, int* non_zero_values);
void my_quicksort(bvector* arr, int low, int high, unsigned variables, int* norms);
int free_f(void* p, size_t* s);

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
    while((sopp -> table = malloc(sizeof(alist_t*) * good_size)) == NULL)
        good_size /= 2;
    memset(sopp -> table, 0, sizeof(alist_t*) * good_size);
    sopp -> table_size = good_size;
    sopp -> current_length = 0;
    NULL_CHECK(sopp -> arraylist = alist_create());
    return sopp;
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
        NULL_CHECK(sopp -> table[hashcode] = alist_create());
        alist_add(sopp->table[hashcode], product, sizeof(productp_t *));
        alist_add(sopp->arraylist, product, sizeof(productp_t *));
        sopp -> current_length++;
        return true;
    }

    //list not empty, check existence
    size_t size;
    productp_t** array = alist_as_array(sopp->table[hashcode], &size);
    size_t i = 0;
    while(i < size && !bvector_equals(array[i] -> b_product -> product, product -> b_product -> product, product -> b_product -> variables))
        i++;
    if(i < size) {
        //element was found, update value
        array[i]->coeff += product->coeff;
        productp_destroy(product);
        return true;
    }
    //element was not found
    alist_add(sopp->table[hashcode], product, sizeof(productp_t *));
    alist_add(sopp->arraylist, product, sizeof(productp_t *));
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
    unsigned n = p -> b_product -> variables;
    for(int i = 0; i < n; i++){
        hashcode += (long) pow(p -> b_product -> product[i] * 31, n - i);
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
    productp_t** list = alist_as_array(sop->arraylist, &size);
    for(size_t i = 0; i < size; i++){
        productp_t* current_p = list[i];
        sopp_value += current_p->coeff * product_of(current_p -> b_product, input);
    }
    return sopp_value;
}

/**
 * Checks if sopp_t is a correct sop plus form of the function fun
 * time: exponential in number of variables
 * @return true it is valid
 */
bool sopp_form_of(sopp_t* sopp, fplus_t* fun){
    unsigned length = fun -> variables;
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
void sopp_binaries(bool *value, unsigned i, sopp_t* sop, fplus_t* fun, bool* result) {
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
 * Prints the values in the sopp form
 * @param s The sopp form
 */
void sopp_print(sopp_t* s){
    size_t size;
    productp_t** array = alist_as_array(s->arraylist, &size);
    printf("Sopp form is: \n");
    for(size_t i = 0; i < size; i++){
        printf("Product has coeff: %d and is: ", (array[i]) -> coeff);
        for(int j = 0; j < array[0] -> b_product -> variables; j++){
            if(array[i] -> b_product -> product[j] > 1)
                printf("- ");
            else
                printf("%d ", array[i] -> b_product -> product[j]);
        }
        printf("\n");
    }
}

/**
 * Frees the memory used by the sopp form
 * @param sopp The sopp form
 */
void sopp_destroy(sopp_t* sopp){
    if(sopp) {
        size_t size;
        productp_t **a = alist_as_array(sopp->arraylist, &size);
        for (size_t i = 0; i < size; i++) {
            productp_destroy(a[i]);
        }

        alist_destroy(sopp->arraylist);
        for (size_t i = 0; i < sopp->table_size; i++) {
            if (sopp->table[i] != NULL)
                alist_destroy(sopp->table[i]);
        }
        FREE(sopp->table);
        FREE(sopp);
    }
}

/**
 * Calculates a (reasonably) minimal sopp form for the given function
 * sopp form is minimal <=> the sum of its coefficients is minimal
 * @param f A fplus function
 * @return The minimal sopp form
 */
sopp_t* sopp_synthesis(fplus_t* f){
    sopp_t* sopp; //will store the minimal sopp form
    implicantp_t* implicants; //will store prime implicants
    bool go_on;

    NULL_CHECK(sopp = sopp_create_wsize(f -> nz_size));
    NULL_CHECK(implicants = prime_implicants(f));

    fplus_t* f_copy = fplus_copy(f);
    implicantp_t* i_copy = implicants_copy(implicants);

    essentialsp_t* e;
    do {
        if((e = essential_implicants(f_copy, i_copy)) == NULL)
            break;
        if(e->points_size == 0){
            essentials_destroy(e);
        }
        for (int i = 0; i < e -> impl_size; i++) {
            int max = 0;
            for (int j = 0; j < e -> points_size; j++) {
                if (product_of(e -> implicants[i]->b_product, e->points[j])) {
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
        productp_t** impls = alist_as_array(sopp->arraylist, NULL);
        for(size_t i = 0; i < sopp -> current_length; i++) {
            int size;
            int *indexes = binary2decimals(impls[i] ->b_product->product, impls[i] -> b_product -> variables, &size);
            for (int j = 0; j < size; j++) {
                fplus_sub2value_sopp(f_copy, indexes[j], impls[i]->coeff);
            }
            FREE(indexes);
        }

        fplus_update_non_zeros(f_copy);
        implicantp_t *new_implicants = prime_implicants(f_copy);
        go_on = remove_implicant_duplicates(i_copy, new_implicants, f_copy) && i_copy -> size > 0;
        //TODO: better exit only if no essential implicants
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
            int *indexes = binary2decimals(i_copy->bvectors[i], i_copy->variables, &size);
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
            FREE(indexes);
        }
        if(implicant_chosen == -1)
            break;
        productp_t* p = productp_create(i_copy -> bvectors[implicant_chosen], i_copy -> variables, min);

        //change the dashed into not_present
        for(int k = 0; k < f -> variables; k++)
            if(p -> b_product -> product[k] == dash)
                p -> b_product -> product[k] = not_present;
        sopp_add(sopp, p);
        productp_destroy(p);

        int size;
        int *indexes = binary2decimals(i_copy -> bvectors[implicant_chosen], i_copy -> variables, &size);
        for (int j = 0; j < size; j++)
            fplus_sub2value_sopp(f_copy, indexes[j], min);
        FREE(indexes);

        implicantp_t *new_implicants = prime_implicants(f_copy);
        remove_implicant_duplicates(i_copy, new_implicants, f_copy);
        implicants_soft_destroy(new_implicants);
    }

    //clean up
    implicants_destroy(implicants);
    implicants_destroy(i_copy);
    fplus_copy_destroy(f_copy);

    return sopp;
}

/**
 * Calculates a (reasonably) minimal sopp form for the given function
 * Similar to the standard procedure calculates an approximation to gain
 * in time complexity
 * @param f A fplus function
 * @return The minimal sopp form
 */
sopp_t* sopp_synthesis_experimental(fplus_t* f){
    sopp_t* sopp; //will store the minimal sopp form
    implicantp_t* implicants; //will store prime implicants
    bool go_on;

    NULL_CHECK(sopp = sopp_create_wsize(f -> nz_size));
    NULL_CHECK(implicants = prime_implicants(f));

    fplus_t* f_copy = fplus_copy(f);
    implicantp_t* i_copy = implicants_copy(implicants);

    essentialsp_t* e;
    do {
        if((e = essential_implicants(f_copy, i_copy)) == NULL)
            break;
        if(e->points_size == 0){
            essentials_destroy(e);
        }
        for (int i = 0; i < e -> impl_size; i++) {
            int max = 0;
            for (int j = 0; j < e -> points_size; j++) {
                if (product_of(e -> implicants[i]->b_product, e->points[j])) {
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
        productp_t** impls = alist_as_array(sopp->arraylist, NULL);
        for(size_t i = 0; i < sopp -> current_length; i++) {
            int size;
            int *indexes = binary2decimals(impls[i] ->b_product->product, impls[i] -> b_product -> variables, &size);
            for (int j = 0; j < size; j++) {
                fplus_sub2value_sopp(f_copy, indexes[j], impls[i]->coeff);
            }
            FREE(indexes);
        }

        fplus_update_non_zeros(f_copy);
        implicantp_t *new_implicants = prime_implicants(f_copy);
        go_on = remove_implicant_duplicates(i_copy, new_implicants, f_copy) && i_copy -> size > 0;
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
            int *indexes = binary2decimals(i_copy->bvectors[i], i_copy->variables, &size);
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
            FREE(indexes);
        }
        if(implicant_chosen == -1)
            break;
        productp_t* p = productp_create(i_copy -> bvectors[implicant_chosen], i_copy -> variables, min);

        //change the dashed into not_present
        for(int k = 0; k < f -> variables; k++)
            if(p -> b_product -> product[k] == dash)
                p -> b_product -> product[k] = not_present;
        sopp_add(sopp, p);
        productp_destroy(p);

        int size;
        int *indexes = binary2decimals(i_copy -> bvectors[implicant_chosen], i_copy -> variables, &size);
        for (int j = 0; j < size; j++)
            fplus_sub2value_sopp(f_copy, indexes[j], min);
        FREE(indexes);

        //remove implicants covering only don't care points
        int removed = 0;
        int i = 0;
        while(i < i_copy->size - removed){
            indexes = binary2decimals(i_copy->bvectors[i], f->variables, &size);
            bool removable = true;
            int j = 0;
            while(removable && j < size){
                removable = fplus_value_at(f, indexes[j]) == F_DONT_CARE_VALUE;
                j++;
            }
            if(removable){
                removed++;
                FREE(i_copy->bvectors[i]);
                i_copy->bvectors[i] = i_copy->bvectors[i_copy->size - removed];
            } else
                i++;
            FREE(indexes);
        }

        //update implicants count
        i_copy->size -= removed;
    }

    //clean up
    implicants_destroy(implicants);
    implicants_destroy(i_copy);
    fplus_copy_destroy(f_copy);

    return sopp;
}

/**
 * @param sopp A sopp or dsopp form
 * @return The sum of the weights of all the products
 */
long sopp_weights_sum(sopp_t* sopp){
    NULL_CHECK(sopp);
    long sum = 0;
    productp_t** a = alist_as_array(sopp->arraylist, NULL);
    for(int i = 0; i < sopp->current_length; i++){
        sum += a[i]->coeff;
    }
    return sum;
}

/**
 * Checks if dsopp_t is a correct disjoint sop plus form of the function fun
 * time: exponential in number of variables
 * @return true it is valid
 */
bool dsopp_form_of(dsopp_t* sopp, fplus_t* fun){
    unsigned length = fun -> variables;
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
void dsopp_binaries(bool *value, unsigned i, dsopp_t* sop, fplus_t* fun, bool* result) {
    if(*result) {
        if (i == 0) {
            int fvalue = fplus_value_of(fun, value);
            *result = *result && sopp_value_of(sop, value) == fvalue;
        } else {
            value[i - 1] = 0;
            dsopp_binaries(value, i - 1, sop, fun, result);
            value[i - 1] = 1;
            dsopp_binaries(value, i - 1, sop, fun, result);
        }
    }
}

/**
 * Prints the values in the dsopp form
 * @param d The dsopp form
 */
void dsopp_print(sopp_t* d){
    size_t size;
    productp_t** array = alist_as_array(d->arraylist, &size);
    printf("Dsopp form is: \n");
    for(size_t i = 0; i < size; i++){
        printf("Product has coeff: %d and is: ", (array[i]) -> coeff);
        for(int j = 0; j < array[0] -> b_product -> variables; j++){
            if(array[i] -> b_product -> product[j] > 1)
                printf("- ");
            else
                printf("%d ", array[i] -> b_product -> product[j]);
        }
        printf("\n");
    }
}

/**
 * Calculates a minimal dsopp form for the given function
 * dsopp form is minimal <=> the sum of its coefficients is minimal
 * @param f A fplus function
 * @return The minimal dsopp form
 */
dsopp_t* dsopp_synthesis(fplus_t* f){
    dsopp_t* dsopp = sopp_create_wsize(f->nz_size);
    NULL_CHECK(dsopp);
    sopp_t* sopp = sopp_synthesis(f);
    NULL_CHECK(sopp);
    llist_t* product_list = llist_create(); //array of int pointer (not array of arrays)
    NULL_CHECK(product_list);
    fplus_t* f_copy = fplus_copy(f);

    while(f_copy->nz_size > 0) {
        productp_t **products = alist_as_array(sopp->arraylist, NULL);
        for (int i = 0; i < sopp->current_length; i++) {
            llist_add(product_list, products[i]);
        }
        int k;
        productp_t* p;
        while(llist_length(product_list) > 0 && (p = llist_max_product(product_list, &k, f_copy)) != NULL){
            int old_coeff = p->coeff;
            p->coeff = k;
            sopp_add(dsopp, p);
            p->coeff = old_coeff;
        }
        fplus_update_non_zeros(f_copy);
        sopp_destroy(sopp);
        sopp = sopp_synthesis(f_copy);
    }

    llist_destroy(product_list);
    fplus_copy_destroy(f_copy);
    sopp_destroy(sopp);
    return dsopp;
}

/**
 * Similar to dsopp_synthesis but it uses experimental sopp synthesis instead of regular
 * @param f A fplus function
 * @return The minimal dsopp form
 */
dsopp_t* dsopp_synthesis_wexperimental(fplus_t* f){
    dsopp_t* dsopp = sopp_create_wsize(f->nz_size);
    NULL_CHECK(dsopp);
    sopp_t* sopp = sopp_synthesis_experimental(f);
    NULL_CHECK(sopp);
    llist_t* product_list = llist_create(); //array of int pointer (not array of arrays)
    NULL_CHECK(product_list);
    fplus_t* f_copy = fplus_copy(f);

    while(f_copy->nz_size > 0) {
        productp_t **products = alist_as_array(sopp->arraylist, NULL);
        for (int i = 0; i < sopp->current_length; i++) {
            llist_add(product_list, products[i]);
        }
        int k;
        productp_t* p;
        while(llist_length(product_list) > 0 && (p = llist_max_product(product_list, &k, f_copy)) != NULL){
            int old_coeff = p->coeff;
            p->coeff = k;
            sopp_add(dsopp, p);
            p->coeff = old_coeff;
        }
        fplus_update_non_zeros(f_copy);
        sopp_destroy(sopp);
        sopp = sopp_synthesis_experimental(f_copy);
    }

    llist_destroy(product_list);
    fplus_copy_destroy(f_copy);
    sopp_destroy(sopp);
    return dsopp;
}


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
    function -> nz_size = size;
    return function;
}

/**
 * Creates a boolean plus function with random outputs.
 * Each non zero value has a random output between 1 and max_value
 * @param variables Number of variables taken by the function
 * @param max_value Max value for the output of the function
 * @param non_zero_chance The probability (as percentage) of having a non zero value as output
 * @return A pointer to the function
 */
fplus_t* fplus_create_random(unsigned variables, int max_value, unsigned non_zero_chance){
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    srandom(spec.tv_nsec);
    fplus_t* function;
    unsigned long f_size = 1;
    f_size = f_size << variables;
    int non_zeros_index = 0;

    MALLOC(function, sizeof(fplus_t), ;);
    MALLOC(function -> values, sizeof(int) * f_size, FREE(function));
    MALLOC(function -> non_zeros, sizeof(bvector) * f_size, FREE(function -> values); FREE(function));

    for(size_t i = 0; i < f_size; i++){
        bool is_non_zero = random() % 100;
        if(is_non_zero < non_zero_chance) {
            function -> values[i] = (int) ((random()) % max_value) + 1;
            function -> non_zeros[non_zeros_index++] = decimal2binary(i, variables); //end of array
        }else
            function -> values[i] = 0;
    }
    function -> variables = variables;
    function -> nz_size = non_zeros_index; //end of array
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
    int index = binary2decimal(input, f -> variables);
    if(index == -1)
        return 0;
    return f -> values[index];
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
void fplus_sub2value_sopp(fplus_t* f, int index, int decrement){
    f -> values[index] -= decrement;
    if(f -> values[index] <= 0) {
        f->values[index] = F_DONT_CARE_VALUE;
    }
}

void fplus_sub2value_dsopp(fplus_t* f, int index, int decrement){
    f -> values[index] -= decrement;
    if(f -> values[index] < 0) {
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
    while(i < f -> nz_size - index_removed) {
        int index = binary2decimal(f->non_zeros[i], f->variables);
        if (f -> values[index] == 0) {
            index_removed++;
            f -> non_zeros[i] = f -> non_zeros[f -> nz_size - index_removed];
        }else
            i++;
    }
    f -> nz_size -= index_removed;
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
    f_copy -> nz_size = f -> nz_size;
    unsigned long values_size = 1;
    values_size = values_size << f -> variables;
    MALLOC(f_copy -> values, sizeof(int) * values_size, FREE(f_copy););
    if(f->nz_size == 0)
        f_copy->non_zeros = NULL;
    else {
        MALLOC(f_copy->non_zeros, sizeof(bool *) * f->nz_size, FREE(f_copy->values);
                FREE(f_copy););
        memcpy(f_copy -> non_zeros, f -> non_zeros, sizeof(bool*) * f -> nz_size);
    }
    memcpy(f_copy->values, f->values, sizeof(int) * values_size);
    return f_copy;
}

/**
 * Frees the memory used by the function
 * @param f A function created with the method fplus_copy
 */
void fplus_copy_destroy(fplus_t* f){
    FREE(f -> non_zeros);
    FREE(f -> values);
    FREE(f);
}

/**
 * Prints function as Karnaugh map if n of variables = 4
 * if n != 4 will print the function as a matrix
 */
void fplus_print(fplus_t* f){
    if(f -> variables != 4){
        unsigned long size = 1;
        size = size << ((f->variables) >> size); //2^(f->variables)
        printf("Function table: \n");
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                printf("%d\t", f -> values[(i * size) + j]);
            }
            printf("\n");
        }
        return;
    }

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
    for(int i = 0; i < f -> nz_size; i++){
        FREE(f -> non_zeros[i]);
    }
    FREE(f -> non_zeros);
    FREE(f -> values);
    FREE(f);
}


/**
 * Creates a product plus
 * @param b A vector with the variables in the product (b[i] = true <=> variable is in the product)
 * @param variables Number of total variables (also size of above vector)
 * @param coeff coefficient of the product
 * @return A pointer to the product
 */
productp_t* productp_create(bool* b, unsigned variables, int coeff){
    productp_t* p;
    MALLOC(p, sizeof(productp_t), ;);
    p -> b_product = product_create(b, variables);
    p -> coeff = coeff;
    return p;
}

/**
 * @return Given a product p, returns an exact copy
 */
productp_t* productp_copy(productp_t* p){
    productp_t* p_copy;
    MALLOC(p_copy, sizeof(productp_t), ;);
    p_copy -> b_product = product_create(p -> b_product -> product, p -> b_product -> variables);
    p_copy -> coeff = p -> coeff;
    return p_copy;
}

/**
 * Frees the memory used by the product plus
 * @param p The product plus
 */
void productp_destroy(productp_t *p) {
    FREE(p -> b_product -> product);
    FREE(p -> b_product);
    FREE(p);
}


/**
 * Quine–McCluskey algorithm for finding prime implicants
 * @param f
 * @return
 */
implicantp_t* prime_implicants(fplus_t* f) {
    size_t non_zeros_size = f->nz_size;
    bvector *result = NULL; //will store prime implicants
    size_t result_size = 0; //will store number of prime implicants
    alist_t *old_list_2free = NULL;

    if(non_zeros_size > 0){
        bvector *non_zeros;
        MALLOC(non_zeros, sizeof(bvector) * non_zeros_size, ;);
        memcpy(non_zeros, f->non_zeros, sizeof(bvector) * non_zeros_size);

        while (true) {
            int norms[non_zeros_size]; //will contain norm of each vector
            //sort non zero values
            my_quicksort(non_zeros, 0, (int) non_zeros_size - 1, f->variables, norms);

            bool taken[non_zeros_size]; // stores if the corresponding element inside non_zeros has been joined at least one time with another
            memset(taken, 0, sizeof(bool) * non_zeros_size);

            alist_t *impl_found = alist_create(); //current list of implicants

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
                        if(!old_list_2free && fplus_value_of(f, non_zeros[i]) == F_DONT_CARE_VALUE && fplus_value_of(f, non_zeros[j]) == F_DONT_CARE_VALUE){
                            //if they both cover don't care points
                            taken[i] = true;
                            taken[j] = true;
                        }else {
                            bvector elem = joinable_vectors(non_zeros[i], non_zeros[j], f->variables);
                            if (elem) {
                                taken[i] = true;
                                taken[j] = true;
                                alist_add(impl_found, elem, sizeof(bvector));
                            }
                        }
                        j++;
                    }
                }
            }

            //case last cycle
            if (alist_length(impl_found) == 0) {
                if (!old_list_2free) {
                    //only one cycle was made
                    MALLOC(result, sizeof(bvector) * non_zeros_size, ;);
                    for (size_t i = 0; i < non_zeros_size; i++) {
                        MALLOC(result[i], sizeof(bool) * f->variables, FREE(result);
                                FREE(non_zeros));
                        memcpy(result[i], non_zeros[i], sizeof(bool) * f->variables);
                    }
                    FREE(non_zeros);
                } else {
                    MALLOC(result, sizeof(bvector) * non_zeros_size, ;);
                    memcpy(result, non_zeros, sizeof(bvector) * non_zeros_size);
                    alist_destroy(old_list_2free);
                }
                result_size = non_zeros_size;
                alist_destroy(impl_found);
                break;
            }

            //add implicants that have not been joined
            for (size_t i = 0; i < non_zeros_size; i++) {
                if (!taken[i]) {
                    bvector b;
                    MALLOC(b, sizeof(bool) * f->variables, ;); //TODO: add more clean up
                    memcpy(b, non_zeros[i], sizeof(bool) * f->variables);
                    alist_add(impl_found, b, sizeof(bvector));
                }
            }

            //free list used in last cycle
            if (old_list_2free) {
                alist_for_each(old_list_2free, free_f);
                alist_destroy(old_list_2free);
            } else
                //if first cycle
                FREE(non_zeros);

            old_list_2free = impl_found;

            //set as new array the joint implicants found
            non_zeros = alist_as_array(impl_found, &non_zeros_size);

            //delete duplicates
            int duplicates_found = 0;
            for (size_t i = 0; i < non_zeros_size - 1 - duplicates_found; i++) {
                for (size_t j = i + 1; j < non_zeros_size - duplicates_found; j++) {
                    if (bvector_equals(non_zeros[i], non_zeros[j], f->variables)) {
                        duplicates_found++;
                        FREE(non_zeros[j]);
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
                    FREE(result[j]);
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
    MALLOC(implicants, sizeof(implicantp_t), FREE(result)); //TODO: should clean more stuff
    implicants -> bvectors = result;
    implicants -> size = result_size;
    implicants -> variables = f -> variables;

    return implicants;
}

/**
 * Standard partition function for quicksort adapted to a bvector
 * If norms != NULL, the norm of the vectors are stored at the corresponding
 * index of the vector
 */
int partition (bvector* arr, int low, int high, unsigned variables, int* norms){
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
void my_quicksort(bvector* arr, int low, int high, unsigned variables, int* norms) {
    if (low < high) {
        int pi = partition(arr, low, high, variables, norms);
        my_quicksort(arr, low, pi - 1, variables, norms);  // Before pi
        my_quicksort(arr, pi + 1, high, variables, norms); // After pi
    }
}

/**
 * Frees a given heap pointer
 * Used to call alist_for_each
 * @param p The pointer
 * @param s The size of the memory, will be set to 0 after free
 * @return always true
 */
int free_f(void* p, size_t* s){
    FREE(p);
    *s = 0;
    return 1;
}

/**
 * Checks if the two vectors are joinable
 * Two vectors v1, v2 are joinable <=> Exists j / v1(j) = !v2(j) and for each i != j v1(i) = v2(i)
 * The joined vector will be equal to v1(or v2) for each i != j and will a dash value for i = j
 * @param size The size of the vectors
 * @return The joined vector (in heap) if possible, NULL otherwise
 */
bvector joinable_vectors(const bool* v1, const bool* v2, unsigned variables){
    bool* join;
    MALLOC(join, sizeof(bool) * variables, ;);

    int counter = 0; //counts the complimentary bits found
    for(int i = 0; i < variables; i++){
        if(v1[i] == v2[i]){
            join[i] = v1[i];
        }else if(v1[i] == dash || v2[i] == dash) {
            FREE(join);
            return NULL;
        }else{
            counter++;
            if(counter > 1) {
                FREE(join);
                return NULL; //are not joinable
            }
            join[i] = dash;
        }
    }
    return join;
}

/**
 * Creates a copy of the given implicant.
 * @return copy of the implicants passed as parameters
 */
implicantp_t* implicants_copy(implicantp_t* impl){
    implicantp_t* i_copy;
    MALLOC(i_copy, sizeof(implicantp_t),;);
    i_copy->size = impl->size;
    i_copy->variables = impl->variables;
    if(impl->size == 0)
        i_copy->bvectors = NULL;
    else {
        MALLOC(i_copy->bvectors, sizeof(bool *) * impl->size, FREE(i_copy));
        for (int i = 0; i < impl->size; i++) {
            MALLOC(i_copy->bvectors[i], sizeof(bool) * impl->variables, ;);//TODO: improve clean
            memcpy(i_copy->bvectors[i], impl->bvectors[i], sizeof(bool) * impl->variables);
        }
    }
    return i_copy;
}

/**
 * Joins source and to_remove while removing the implicants covering the same non zero points
 * @param source The original implicants. May have some implicants covering 0 or don't care point
 * @param to_remove New implicants, covering only non zero points
 * @param f The boolean plus function
 * @return true if at least an implicant in source has been removed
 */
bool remove_implicant_duplicates(implicantp_t* source, implicantp_t* to_remove, fplus_t* f){
    int removed = 0; //stores the number of removed implicants from source
    int non_zero_values = 0; //stores the number of non zero values encountered. if = 0 => no need to go on

    /*
     * for each implicant in to_remove checks if it covers all the non_zero points
     * of a implicant in source. In that case the implicant in source is removed
     */
    for(int i = 0; i < to_remove -> size; i++){
        //get all the points covered by the implicant in to_remove
        int r_size; //remove size
        int* r_indexes = binary2decimals(to_remove -> bvectors[i], to_remove -> variables, &r_size);
        int j = 0;
        while(j < source -> size - removed){
            //get all the points covered by the implicant in source
            int s_size; //source zie
            int* s_indexes = binary2decimals(source -> bvectors[j], source -> variables, &s_size);

            //if all non_zero points of s are covered by r
            if(r_cover_s(r_indexes, r_size, s_indexes, s_size, f, &non_zero_values)){
                removed++;
                FREE(source->bvectors[j]);
                source -> bvectors[j] = source -> bvectors[source->size - removed];
            }else
                j++;
            FREE(s_indexes);
        }
        FREE(r_indexes);
    }
    source -> size -= removed;

    //add all implicants in to_remove to source
    REALLOC(source -> bvectors, sizeof(bvector) * (source -> size + to_remove -> size), ;);
    for(int i = source -> size; i < source->size + to_remove->size; i++){
        source->bvectors[i] = to_remove->bvectors[i - source->size];
    }
    source -> size += to_remove -> size;
    if(source -> size == to_remove -> size) {
        return false;
    }
    return non_zero_values > 0;
}

/**
 * @param r_indexes The points of r as decimal
 * @param r_size The number of points of r
 * @param s_indexes The points of s as decimal
 * @param s_size The number of points of s
 * @param f The function which output to check
 * @param non_zero_values Will store the number of non-zero values found
 * @return true if all non_zero points of s are covered by r
 */
bool r_cover_s(const int *r_indexes, int r_size, int *s_indexes, int s_size, fplus_t *f, int* non_zero_values) {
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
    return true;
}

/**
 * Prints the implicants
 */
void implicants_print(implicantp_t* impl){
    printf("Implicants: \n");
    for(int i = 0; i < impl -> size; i++){
        for(int j = 0; j < impl -> variables; j++){
            if(impl -> bvectors[i][j] > 1)
                printf("-\t");
            else
                printf("%d\t", impl -> bvectors[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Frees the memory used by an implicant leaving the memory
 * used by each element of impl->implicants
 */
void implicants_soft_destroy(implicantp_t* impl){
    FREE(impl -> bvectors);
    FREE(impl);
}

/**
 * Frees the memory used by implicants plus
 * @param impl The implicants to free
 */
void implicants_destroy(implicantp_t* impl){
    for(int i = 0; i < impl -> size; i++){
        FREE(impl -> bvectors[i]);
    }
    FREE(impl -> bvectors);
    FREE(impl);
}


/**
 * Finds the essential implicants of the function from the prime implicants
 * !!!Exponential in space!!!
 * @return a pointer to a struct containing the essential points and the essential prime implicants
 */
essentialsp_t* essential_implicants(fplus_t* f, implicantp_t* implicants){
    unsigned long f_size = 1;
    f_size = f_size << (f -> variables); //TODO: exp size unnecessary as only non_zero points are needed
    alist_t* points[f_size]; //each index represent a point of f, the list will contain the implicants covering that point
    bvector essential_implicants[f_size]; //stores the essential prime implicants
    int e_index = 0; //index of above and below array;
    bvector* essential_points; //stores the essential points
    MALLOC(essential_points, sizeof(bvector) * f_size, ;);

    //init the array of points
    for(size_t i = 0; i < f_size; i++){
        points[i] = alist_create();
    }

    //count occurrences of various points
    for(int i = 0; i < implicants -> size; i++){
        int size = 0;
        int* indexes = binary2decimals(implicants -> bvectors[i], f -> variables, &size);
        for(int j = 0; j < size; j++){
            alist_add(points[indexes[j]], implicants->bvectors[i], sizeof(bvector));
        }
        FREE(indexes);
    }

    //store points and implicants in array
    for(int i = 0; i < f -> nz_size; i++){
        int index = binary2decimal(f -> non_zeros[i], f -> variables);
        int l_size = alist_length(points[index]);
        if(fplus_value_of(f, f->non_zeros[i]) != F_DONT_CARE_VALUE) {
            if (l_size == 0) {
                fprintf(stderr, "Some error occurred, an essential point has not been covered\n");
            } else if (l_size == 1) {
                essential_implicants[e_index] = alist_get(points[index], 0, 0);
                essential_points[e_index++] = f->non_zeros[i];
            }
        }
    }

    essentialsp_t* e;

    productp_t** final_implicants;
    //if no essential points are found
    if(e_index == 0) {
        FREE(essential_points);
        e = NULL;
    } else {
        MALLOC(e, sizeof(essentialsp_t), FREE(essential_points););
        REALLOC(essential_points, sizeof(bvector) * e_index, ;);
        final_implicants = implicants2sop(essential_implicants, e_index, f->variables, &e->impl_size);

        e->implicants = final_implicants;
        e->points = essential_points;
        e->points_size = e_index;
    }

    for(size_t i = 0; i < f_size; i++) {
        alist_destroy(points[i]);
    }
    return e;
}

/**
 * Converts a boolean vector array to a product array
 * Also removes duplicates if found (O(n^2), may be improved)
 * @param final_size Sets in this variable the size without the duplicates
 * @return The newly created array
 */
productp_t** implicants2sop(bvector* implicants, int size, unsigned variables, int* final_size){
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
 * Prints the essential implicants and points
 * @param e The essentials
 * @param variables The number of variables
 */
void essentials_print(essentialsp_t* e, unsigned variables){
    printf("Essential implicants: \n");
    for(int i = 0; i < e -> impl_size; i++){
        for(int j = 0; j < variables; j++){
            printf("%d\t", e -> implicants[i] -> b_product -> product[j]);
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
 * Frees the memory used by the essentials form
 * @param e The essential form
 */
void essentials_destroy(essentialsp_t* e){
    for(int i = 0; i < e -> impl_size; i++)
        productp_destroy(e -> implicants[i]);
    FREE(e -> implicants);
    FREE(e -> points);
    FREE(e);
}