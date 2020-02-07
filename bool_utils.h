//
// Created by alessio on 2/7/20.
//

#ifndef SOP_SYNTHESIS_BOOL_UTILS_H
#define SOP_SYNTHESIS_BOOL_UTILS_H

typedef struct{
    int* values;
    int size;
}bool_f;


bool_f* f_create(int[], int variables);
bool_f* f_create_random(int variables);

typedef struct{
    int* product;
    int size;
}bool_product;

bool_product* product_create(int product[], int size);


#endif //SOP_SYNTHESIS_BOOL_UTILS_H
