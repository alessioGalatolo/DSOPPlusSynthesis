//
// Created by alessio on 2/7/20.
//

#ifndef SOP_SYNTHESIS_SOP_PLUS_H
#define SOP_SYNTHESIS_SOP_PLUS_H

#include "list.h"

typedef struct{
    list_t* products;
    int size;
}sop_plus;

sop_plus* sopp_create();
int sopp_add(sop_plus*, bool_product*);

#endif //SOP_SYNTHESIS_SOP_PLUS_H
