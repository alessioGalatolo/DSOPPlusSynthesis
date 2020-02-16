#include <stdio.h>
#include "bool_utils.h"
#include "bool_plus.h"

#define VARIABLES 4

int main() {
    fplus* function = fplus_create_random(VARIABLES);
    sopp* sopp = sopp_create();
    implicant_plus* implicants = prime_implicants(function);

    
    return 0;
}