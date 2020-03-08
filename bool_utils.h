/*
 * Library containing the utilities used in boolean algebra
 * and some basic operations
 */

#ifndef SOP_SYNTHESIS_BOOL_UTILS_H
#define SOP_SYNTHESIS_BOOL_UTILS_H

#define false 0
#define true 1
#define not_present 2
#define dont_care 3
#define dash 4 //used to join two vectors

typedef unsigned char bool;
typedef bool* bvector; //its value may assume the defined above (false, true, not_present, dash)

typedef struct{
    bvector values;
    int variables;
}bool_f;

typedef struct{
    bvector product;
    int variables;
}bool_product;

typedef struct{
    bool_product* products;
    int size;
}sop_t;

/* Utility functions */
int norm1(const bool*, int variables); //returns the norm 1 of the vector
int binary2decimal(const bool *values, int size); //returns the decimal of the given binary number
int* binary2decimals(const bool *values, int size, int* return_size); //same as above but it may return more decimal if values contains dashes
bvector decimal2binary(int value, int size); //returns the binary representation of the given value
bool bvector_equals(bvector, bvector, int size); //returns true if the 2 vector are the same one


/* Other functions */
bool_f* f_create(bool[], int variables); //creates a boolean function given its output values
bool_f* f_create_random(int variables); //creates a random boolean function



/* Creates a product from its binary representation */
bool_product* product_create(bool product[], int size);
bool product_of(bool_product*, const bool*); //returns the value of the product with the given values
//bool product_covers(bool_product*, const bool*);


#endif //SOP_SYNTHESIS_BOOL_UTILS_H
