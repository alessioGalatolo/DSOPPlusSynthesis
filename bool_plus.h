/*
 * Library containing all the useful definitions of the algebraic plus forms:
 *      Function plus -> from vector of bool to a natural number
 *      Sop plus form -> sum of product where each product has a natural number as coefficient
 *      Product plus -> a product with a coefficient
 * It also provides basic operation extended to these forms
 */

#ifndef SOP_SYNTHESIS_BOOL_PLUS_H
#define SOP_SYNTHESIS_BOOL_PLUS_H

#define F_DONT_CARE_VALUE -1

#include "list.h"

//a boolean product with a coefficient
typedef struct{
    bool_product* product;
    int coeff;
}productp_t;

//kinda hashtable used to store sopp
//more like a set
typedef struct{
    list_t** table; //the hashtable
    size_t table_size; //number of buckets
    size_t current_length; //number of actual elements
    list_t* array; //list with all the elements
}sopp_t; //sop plus form

//a function from a vector of bool to natural numbers
typedef struct {
    int* values; //stores each combination of the input
    int variables; //number of variables taken as input, 2^variables = size of above array
    bool** non_zeros; //array with the index of non-zero values written as binary numbers
    int size; //size of above array
}fplus_t;

//stores a list of implicants
typedef struct {
    bool** implicants;
    int size;
    int variables;
}implicantp_t;

//stores a list of essential prime implicants and their essential points
typedef struct {
    productp_t** implicants;
    int impl_size;
    bool** points;
    int points_size;
}essentialsp_t;

void productp_destroy(productp_t *p); //frees the memory of a product plus
productp_t* productp_create(bool*, int variables, int coeff);
productp_t* productp_copy(productp_t*);

/*
 * fplus related functions
 */
fplus_t* fplus_create(int* values, bool** non_zeros, int variables, int size); //creates a boolean plus function with the given parameters
fplus_t* fplus_create_random(int variables, int max_value); //creates a boolean plus function with random outputs
int fplus_value_of(fplus_t*, bool*); //returns the output of the function with the given input
int fplus_value_at(fplus_t*, int); //returns the output of the function at the given index
void fplus_print(fplus_t*); //prints the function as Karnaugh map <=> n_variables = 4
void fplus_destroy(fplus_t*); //frees the heap taken by the function
void fplus_add2value(fplus_t*, int index, int increment); //add given value to the output of the function in the given input
fplus_t* fplus_copy(fplus_t*); //returns a copy of f
void fplus_copy_destroy(fplus_t*); //destroys a function created with fplus_copy
void fplus_update_non_zeros(fplus_t*); //re-calculates the non_zeros array

/*
 * sopp related functions
 */
sopp_t* sopp_create(); //creates a sopp with default size
sopp_t* sopp_create_wsize(int expected_size); //creates a sopp with a suggested size
void sopp_destroy(sopp_t*); //frees the memory of a sopp form
bool sopp_add(sopp_t*, productp_t*); //adds a product to a sopp
int sopp_value_of(sopp_t*, bool*); //returns the output of the sopp with the given variables values
bool sopp_form_of(sopp_t*, fplus_t*); //returns true if the given sopp form is valid for the given function
void sopp_print(sopp_t*); //prints the sopp
sopp_t* sopp_synthesis(fplus_t*); //return a sopp minimal sopp form for the given function

/*
 * implicants related functions
 */
implicantp_t* prime_implicants(fplus_t*); //returns the prime implicants of the given bool plus function
void implicants_destroy(implicantp_t*); //frees the heap taken by the above function
void implicants_print(implicantp_t*); //prints the list of implicants
implicantp_t* implicants_copy(implicantp_t*); //returns a copy of the given implicants
void implicants_copy_destroy(implicantp_t* impl); //destroy implicants created with implicants_copy ???
bool implicant_of(bvector, bvector, int variables); /*deprecated*/

/*
 * essential related functions
 */
essentialsp_t* essential_implicants(fplus_t*, implicantp_t*); //returns the essential prime implicants
void essentials_destroy(essentialsp_t*); //frees the heap taken by the above function
void essentials_print(essentialsp_t*, int); //prints the implicants and the points
bool remove_implicant_duplicates(implicantp_t*, implicantp_t*, fplus_t*);



#endif //SOP_SYNTHESIS_BOOL_PLUS_H
