//common utilities
#ifndef SOP_SYNTHESIS_UTILS_H
#define SOP_SYNTHESIS_UTILS_H

#include <stdio.h>


#define NULL_CHECK(x)\
    if((x) == NULL){\
        fprintf(stderr, "Null pointer\n");\
        return 0;\
    }

//does a malloc and checks return values, if malloc failed prints error does 'clean' command and returns 0
#define MALLOC(x, s, clean)\
    if(((x) = malloc(s)) == NULL){\
        fprintf(stderr, "Malloc returned a null pointer\n");\
        clean;\
        return 0;\
    }

#define REALLOC(x, s, clean)\
    {\
        void* tmp = realloc(x, s);\
        if(tmp == NULL){\
            fprintf(stderr, "Realloc unable to allocate %ld memory. Returned a null pointer\n", s);\
            clean;\
        }\
        (x) = tmp;\
    }

#endif //SOP_SYNTHESIS_UTILS_H
