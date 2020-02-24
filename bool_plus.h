/*
 * Library containing all the useful definitions of the algebraic plus forms:
 *      Function plus -> from vector of bool to a natural number
 *      Sop plus form -> sum of product where each product has a natural number as coefficient
 *      Product plus -> a product with a coefficient
 * It also provides basic operation extended to these forms
 */

#ifndef SOP_SYNTHESIS_BOOL_PLUS_H
#define SOP_SYNTHESIS_BOOL_PLUS_H

#include "list.h"

typedef struct{
    bool_product* product;
    int coeff;
}product_plus;

typedef struct{
    list_t* products; //list of products of product_plus
    int size;
}sopp; //sop plus form

typedef struct {
    int* values;
    int variables; //number of variables taken as input, 2^variables = size of above array
    bool** non_zeros; //array with the index of non-zero values written as binary numbers
    int size; //size of above array
}fplus;

typedef struct {
    bool** implicants;
    int size;
}implicant_plus;

typedef struct {
    product_plus* implicants;
    bool** points;
    int size;
}essentials;

fplus* fplus_create(int* values, bool** non_zeros, int variables, int size); //creates a boolean plus function with the given parameters
fplus* fplus_create_random(int variables, int max_value); //creates a boolean plus function with random outputs
int fplus_value_of(fplus*, bool*); //returns the output of the function with the given input
void fplus_print(fplus*);

sopp* sopp_create();
int sopp_add(sopp*, product_plus*);
int sopp_value_of(sopp*, bool*);
bool is_sopp_of(sopp*, fplus);
implicant_plus* prime_implicants(fplus*);
essentials* essential_implicants(fplus*, implicant_plus*);



#endif //SOP_SYNTHESIS_BOOL_PLUS_H
