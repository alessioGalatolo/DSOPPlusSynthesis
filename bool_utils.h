/*
 * Library containing the utilities used in boolean algebra
 * and some basic operations
 */

#ifndef SOP_SYNTHESIS_BOOL_UTILS_H
#define SOP_SYNTHESIS_BOOL_UTILS_H

typedef unsigned char bool;

typedef struct{
    bool* values;
    int variables;
}bool_f;

typedef struct{
    bool* product; //0 -> not value, 1 -> value, 2 -> not present
    int variables;
}bool_product;

/* Utility functions */
int binary2decimal(const bool *values, int size);
bool* decimal2binary(int value, int size);


/* Other functions */
//creates a boolean function given its output values
bool_f* f_create(bool[], int variables);
//creates a random boolean function
bool_f* f_create_random(int variables);


/* Creates a product from its binary representation */
bool_product* product_create(bool product[], int size);
bool product_of(bool_product*, const bool*); //returns the value of the product with the given values

typedef struct{
    bool_product* products;
    int size;
}sop_t;

#endif //SOP_SYNTHESIS_BOOL_UTILS_H
