//
// Created by alessio on 2/7/20.
//

#include "SOP_plus.h"
#include "utils.h"
#include <stdlib.h>

sop_plus* sopp_create(){
    sop_plus* sopp = malloc(sizeof(sop_plus));
    NULL_CHECK(sopp);
    sopp -> products = list_create();
    sopp -> size = 0;
    return sopp;
}

int sopp_add(sop_plus* sopp, bool_product* sop1){
    sopp -> products = list_add(sopp -> products, sop1);
    return sopp -> products == NULL ? 0: 1;
}