#include <stdio.h>
#include <assert.h>
#include "bool_plus.h"
#include "utils.h"

#define VARIABLES 4
#define MAX_VALUE 10

int main() {
    //test for sopp synthesis

    fplus_t* function; //will store the function from bool vector to N

    int n_cycles = 1;
    for(int i = 0; i < n_cycles; i++) {
        NULL_CHECK(function = fplus_create_random(VARIABLES, MAX_VALUE));
        fplus_print(function);

        sopp_t *sopp;
        sopp = sopp_synthesis(function);

        sopp_print(sopp);
        assert(sopp_form_of(sopp, function));

        sopp_destroy(sopp);
        fplus_destroy(function);
    }

    fflush(stdout);
    return 0;
}