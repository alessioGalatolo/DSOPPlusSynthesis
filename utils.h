//
// Created by alessio on 2/7/20.
//

#ifndef SOP_SYNTHESIS_UTILS_H
#define SOP_SYNTHESIS_UTILS_H

#include <stdio.h>

#define NULL_CHECK(x)\
    if((x) == NULL){\
        fprintf(stderr, "Null pointer");\
        return 0;\
    }

#endif //SOP_SYNTHESIS_UTILS_H
