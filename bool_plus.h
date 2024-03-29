/*
 * Library containing all the useful definitions of the algebraic plus forms:
 *      Function plus -> from vector of bool to a natural number
 *      Product plus -> a product with a coefficient
 *      Sop plus form -> sum of product where each product has a natural number as coefficient
 *      Sopp form for f -> given s sopp form, f function plus. s is a valid sopp form of f if
 *          for each value x, if f(x) = 0 then s(x) = 0, else s(x) >= f(x)
 *      Disjoint sopp form of f -> given d dsopp form, f function plus: d is a valid dsopp form of f if
 *          for each value x, s(x) = f(x)
 * It also provides basic operation extended to these forms
 */

#ifndef DSOPP_SYNTHESIS_BOOL_PLUS_H
#define DSOPP_SYNTHESIS_BOOL_PLUS_H

//value for don't care point in f
#define F_DONT_CARE_VALUE (-1)

//distribution of don't care values when building a random fplus
#define PROBABILITY_UNDEFINED 50

//parameter for sopp representation in semi-hashtable
#define INIT_SIZE 100
#define GOOD_LOAD 0.5

#include "arraylist.h"
#include "bool_utils.h"
#include "bool_plus.h"

//a boolean product with a coefficient
typedef struct{
    bool_product* b_product;
    int coeff;
}productp_t;

//a function from a vector of bool to natural numbers
typedef struct {
    int* values; //stores each combination of the input
    unsigned variables; //number of variables taken as input, 2^variables = size of above array
    bool** non_zeros; //array with the index of non-zero values written as binary numbers
    size_t nz_size; //size of above array
}fplus_t;

//stores a list of essential prime implicants and their essential points
typedef struct {
    productp_t** implicants; //list of essential implicants
    int impl_size; //size of above list
    bool** points; //list of point covered by the implicants
    int points_size; //size of above list
}essentialsp_t;

//uses hashtable to store sop
//no duplicates are allowed
typedef struct{
    alist_t** table; //the hashtable
    size_t table_size; //number of buckets
    size_t current_length; //number of actual elements
    alist_t* arraylist; //list with all the elements
}sopp_t; //sop plus form

typedef sopp_t dsopp_t; //same structure but they represent different definitions

/*
 * sopp related functions
 */
sopp_t* sopp_create(); //creates a sopp with default size
sopp_t* sopp_create_wsize(size_t expected_size); //creates a sopp with a suggested size
bool sopp_add(sopp_t*, productp_t*); //adds a product to a sopp
int sopp_value_of(sopp_t*, bool*); //returns the output of the sopp with the given variables values
bool sopp_form_of(sopp_t*, fplus_t*); //returns true if the given sopp form is valid for the given function
void sopp_print(sopp_t*); //prints the sopp
void sopp_destroy(sopp_t*); //frees the memory of a sopp form
sopp_t* sopp_synthesis(fplus_t*); //return a minimal sopp form for the given function
sopp_t* sopp_synthesis_experimental(fplus_t*); //sopp synthesis with minor changes to optimize time
long sopp_weights_sum(sopp_t*); //returns sum of weights of sopp/dsopp form
bool sopp_not_empty(sopp_t*); //true if the sopp has at least a product

/*
 * dsopp related functions
 */
bool dsopp_form_of(dsopp_t*, fplus_t*); //returns true if the given dsopp form is valid for the given function
dsopp_t* dsopp_synthesis(fplus_t*); //return a minimal dsopp form for the given function
dsopp_t* dsopp_synthesis_wexperimental(fplus_t*); //dsopp synthesis with the use of sopp_synthesis_experimental
void dsopp_print(dsopp_t*); //prints the dsopp


/*
 * fplus related functions
 */
//creates a boolean plus function with the given parameters
fplus_t* fplus_create(int* values, bool** non_zeros, int variables, int size);
fplus_t* fplus_create_empty(unsigned variables); //creates an f with all don't care values
void fplus_add_output(fplus_t*, int index, int value); //use to build from an empty function

//creates a boolean plus function with random outputs
fplus_t* fplus_create_random(unsigned variables, int max_value, unsigned non_zero_chance);
fplus_t* fplus_create_random_wundefined(unsigned, int, unsigned); //like above but with don't care values
int fplus_value_of(fplus_t*, bool*); //returns the output of the function with the given input
int fplus_value_at(fplus_t*, int); //returns the output of the function at the given index

//add given value to the output of the function in the given input
void fplus_add2value(fplus_t*, int index, int increment);

/*
 *  subtracts value to the output of the function in the given inputs.
 *  If a value reaches 0 or below is set to don't care
 */
void fplus_sub2value_sopp(fplus_t* f, int index, int decrement);

//same as above but when a value reaches 0 is not set to don't care
void fplus_sub2value_dsopp(fplus_t* f, int index, int decrement);
void fplus_update_non_zeros(fplus_t*); //re-calculates the non_zeros array
fplus_t* fplus_copy(fplus_t*); //returns a copy of f
void fplus_copy_destroy(fplus_t*); //destroys a function created with fplus_copy
void fplus_print(fplus_t*); //prints the function as Karnaugh map <=> n_variables = 4
void fplus_destroy(fplus_t*); //frees the heap taken by the function

/*
 * product plus related functions
 */
productp_t* productp_create(bool*, unsigned variables, int coeff); //creates a product plus from given values
productp_t* productp_copy(productp_t*); //generates a copy of the given product
void productp_destroy(productp_t *p); //frees the memory of a product plus

/*
 * implicants related functions
 */
implicants_t* prime_implicants(fplus_t*); //returns the prime implicants of the given bool plus function
implicants_t* implicants_copy(implicants_t*); //returns a copy of the given implicants
//merges the two implicants list and removes duplicates
bool remove_implicant_duplicates(implicants_t *source, implicants_t *to_remove, fplus_t *f);
void implicants_print(implicants_t*); //prints the list of implicants
void implicants_soft_destroy(implicants_t* impl); //destroy implicants objects leaving values
void implicants_destroy(implicants_t*); //frees the heap taken by the above function

/*
 * essential related functions
 */
essentialsp_t* essential_implicants(fplus_t*, implicants_t*); //returns the essential prime implicants
void essentials_print(essentialsp_t*, unsigned); //prints the implicants and the points
void essentials_destroy(essentialsp_t*); //frees the heap taken by the above function


#endif //DSOPP_SYNTHESIS_BOOL_PLUS_H
