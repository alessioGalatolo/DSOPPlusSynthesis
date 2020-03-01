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
}productp_t;

typedef struct{
    list_t* products; //list of products of productp_t
    int size;
}sopp_t; //sop plus form

typedef struct {
    int* values;
    int variables; //number of variables taken as input, 2^variables = size of above array
    bool** non_zeros; //array with the index of non-zero values written as binary numbers
    int size; //size of above array
}fplus_t;

typedef struct {
    bool** implicants;
    int size;
}implicantp_t;

typedef struct {
    productp_t* implicants;
    int impl_size;
    bool** points;
    int points_size;
}essentialsp_t;

fplus_t* fplus_create(int* values, bool** non_zeros, int variables, int size); //creates a boolean plus function with the given parameters
fplus_t* fplus_create_random(int variables, int max_value); //creates a boolean plus function with random outputs
int fplus_value_of(fplus_t*, bool*); //returns the output of the function with the given input
void fplus_print(fplus_t*); //prints the function as Karnaugh map <=> n_variables = 4
void fplus_destroy(fplus_t*); //frees the heap taken by the function

sopp_t* sopp_create();
void sopp_destroy(sopp_t*);
int sopp_add(sopp_t*, productp_t*);
int sopp_value_of(sopp_t*, bool*);
bool is_sopp_of(sopp_t*, fplus_t);
implicantp_t* prime_implicants(fplus_t*);
essentialsp_t* essential_implicants(fplus_t*, implicantp_t*);



#endif //SOP_SYNTHESIS_BOOL_PLUS_H
