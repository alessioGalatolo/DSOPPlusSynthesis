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
typedef bool* bvector;

typedef struct{
    bvector values;
    int variables;
}bool_f;

typedef struct{
    bvector product; //0 -> not value, 1 -> value, 2 -> not present
    int variables;
}bool_product;

typedef struct{
    bool_product* products;
    int size;
}sop_t;

/* Utility functions */
int norm1(const bool*, int variables);
int binary2decimal(const bool *values, int size);
int* binary2decimals(const bool *values, int size, int* return_size); //same as above but it may return more decimal if values contains dashes
bvector decimal2binary(int value, int size);
bool bvector_equals(bvector, bvector, int size);


/* Other functions */
//creates a boolean function given its output values
bool_f* f_create(bool[], int variables);
//creates a random boolean function
bool_f* f_create_random(int variables);


/* Creates a product from its binary representation */
bool_product* product_create(bool product[], int size);
bool product_of(bool_product*, const bool*); //returns the value of the product with the given values
//bool product_covers(bool_product*, const bool*);


#endif //SOP_SYNTHESIS_BOOL_UTILS_H
