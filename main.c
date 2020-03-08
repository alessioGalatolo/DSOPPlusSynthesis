#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bool_utils.h"
#include "bool_plus.h"
#include "utils.h"

#define VARIABLES 4
#define MAX_VALUE 10

int main() {


    fplus_t* function; //will store the function from bool vector to N

    NULL_CHECK(function = fplus_create_random(VARIABLES, MAX_VALUE));
    if(VARIABLES == 4)
        fplus_print(function);

    sopp_t* sopp;
    sopp = sopp_synthesis(function);

    sopp_print(sopp);
    assert(sopp_form_of(sopp, function));

    sopp_destroy(sopp);
    fplus_destroy(function);
    fflush(stdout);
    return 0;
}