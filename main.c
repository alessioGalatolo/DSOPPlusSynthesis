#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool_utils.h"
#include "bool_plus.h"

#define VARIABLES 4
#define MAX_VALUE 10

int main() {
    fplus_t* function = fplus_create_random(VARIABLES, MAX_VALUE);
    fplus_print(function);
    sopp_t* sopp = sopp_create();
    implicantp_t* implicants = prime_implicants(function);
    implicants_print(implicants, function -> variables);

    essentialsp_t* e = essential_implicants(function, implicants);
    essentials_print(e, function -> variables);

    fplus_t* f_copy = fplus_copy(function);

    for(int i = 0; i < e -> impl_size; i++){
        int max = 0;
        for(int j = 0; j < e -> points_size; j++){
            if(product_covers((e -> implicants + i) -> product, e -> points[j])) {
                int cur_value = fplus_value_of(function, e->points[j]);
                if (cur_value > max)
                    max = cur_value;
            }
            (e -> implicants + i) -> coeff = max; //TODO: check if may be a problem
            sopp_add(sopp, e -> implicants + i);
        }
    }



    //clean up
    essentials_destroy(e);
    implicants_destroy(implicants);
    fplus_destroy(function);
    sopp_destroy(sopp);
    fflush(stdout);
    return 0;
}