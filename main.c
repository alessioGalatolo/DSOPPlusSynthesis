#include <stdio.h>
#include <assert.h>
#include "bool_plus.h"
#include "utils.h"

#define VARIABLES 4
#define MAX_VALUE 10
#define NON_ZERO_CHANCE 50

int main() {
    //test for dsopp synthesis
    fplus_t* function; //will store the function from bool vector to N

    int n_cycles = 100000;
    for(int i = 0; i < n_cycles; i++) {
        NULL_CHECK(function = fplus_create_random(VARIABLES, MAX_VALUE, NON_ZERO_CHANCE));
        fplus_print(function);

//        dsopp_t *dsopp = dsopp_synthesis(function);
        sopp_t* sopp = sopp_synthesis(function);
        sopp_t* sopp_e = sopp_synthesis_experimental(function);
        sopp_print(sopp);
        sopp_print(sopp_e);
        assert(sopp_form_of(sopp_e, function));
        assert(sopp_form_of(sopp, function));
        assert(sopp_equals(sopp, sopp_e, function));
        sopp_print(sopp);
        sopp_print(sopp_e);
//        dsopp_print(dsopp);

//        assert(dsopp_form_of(dsopp, function));

        sopp_destroy(sopp);
        sopp_destroy(sopp_e);
//        sopp_destroy(dsopp);
        fplus_destroy(function);
    }

    fflush(stdout);
    return 0;
}