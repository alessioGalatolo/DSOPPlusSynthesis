/*
 * Library containing the utilities used in boolean algebra
 * and some basic operations
 */

#ifndef DSOPP_SYNTHESIS_BOOL_UTILS_H
#define DSOPP_SYNTHESIS_BOOL_UTILS_H

#define false 0
#define true 1
#define not_present 2
#define dont_care 3
#define dash 4 //used to join two vectors
//TODO: maybe join not_present and dash to unique value

typedef unsigned char bool;
typedef bool* bvector; //its value may assume the defined above (false, true, not_present, dash)

typedef struct{
    bvector values;
    unsigned int variables;
}bool_f;

typedef struct{
    bvector product;
    unsigned variables;
}bool_product;

typedef struct{
    bool_product* products;
    int size;
}sop_t;

//stores a list of implicants
typedef struct {
    bool** bvectors; //array of implicants
    int size; //size of above array
    unsigned variables;
}implicants_t;

/* Utility functions */
int norm1(const bool*, unsigned variables); //returns the norm 1 of the vector
int binary2decimal(const bool *values, unsigned variables); //returns the decimal of the given binary number
int* binary2decimals(const bool *values, unsigned variables, int* return_size); //same as above but it may return more decimal if values contains dashes
bvector decimal2binary(int value, unsigned variables); //returns the binary representation of the given value
bool bvector_equals(const bool* b1, const bool* b2, unsigned variables); //returns true if the 2 vector are the same one

/* Other functions */
bool_f* f_create(bool[], int variables); //creates a boolean function given its output values

/* Creates a product from its binary representation */
bool_product* product_create(bool product[], unsigned variables);
bool product_of(bool_product*, const bool*); //returns the value of the product with the given values

#endif //DSOPP_SYNTHESIS_BOOL_UTILS_H
