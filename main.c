#include <stdio.h>
#include <assert.h>
#include "bool_plus.h"
#include "utils.h"

#define VARIABLES 4
#define MAX_VALUE 10

int main() {
    //test for dsopp synthesis

    fplus_t* function; //will store the function from bool vector to N

    int n_cycles = 1;
    for(int i = 0; i < n_cycles; i++) {
        NULL_CHECK(function = fplus_create_random(VARIABLES, MAX_VALUE));
        fplus_print(function);

//        dsopp_t *dsopp = dsopp_synthesis(function);
        sopp_t* sopp = sopp_synthesis(function);

//        sopp_print(sopp);
//        dsopp_print(dsopp);

//        assert(dsopp_form_of(dsopp, function));

        sopp_destroy(sopp);
//        sopp_destroy(dsopp);
        fplus_destroy(function);
    }

    fflush(stdout);
    return 0;
}