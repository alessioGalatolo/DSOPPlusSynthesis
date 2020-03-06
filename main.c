#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool_utils.h"
#include "bool_plus.h"
#include "utils.h"

#define VARIABLES 4
#define MAX_VALUE 10

int main() {
    fplus_t* function = fplus_create_random(VARIABLES, MAX_VALUE);
    NULL_CHECK(function);
    if(VARIABLES == 4)
        fplus_print(function);
    sopp_t* sopp = sopp_create();
    implicantp_t* implicants = prime_implicants(function);
    implicants_print(implicants);

    essentialsp_t* e = essential_implicants(function, implicants);
    essentials_print(e, function -> variables);

    fplus_t* f_copy = fplus_copy(function);
    implicantp_t* i_copy = implicants_copy(implicants);

    cycle:
        for(int i = 0; i < e -> impl_size; i++){
            int max = 0;
            for(int j = 0; j < e -> points_size; j++){
                if(product_of((e -> implicants + i) -> product, e -> points[j])) {
                    int cur_value = fplus_value_of(function, e->points[j]);
                    if (cur_value > max)
                        max = cur_value;
                }
            }
            (e -> implicants + i) -> coeff = max; //TODO: check if may be a problem (should do new var)
            sopp_add(sopp, e -> implicants + i);
            int size;
            int* indexes = binary2decimals((e -> implicants + i) -> product -> product, f_copy -> variables, &size);
            for(int j = 0; j < size; j++)
                fplus_add2value(f_copy, indexes[j], -max);
            free(indexes);
        }

        implicantp_t* new_implicants = prime_implicants(f_copy);
        if(remove_implicant_duplicates(i_copy, new_implicants)) {
            implicants_destroy(new_implicants);
            goto cycle;
        }
    implicants_destroy(new_implicants);


    sopp_print(sopp);

    //clean up
    essentials_destroy(e);
    implicants_destroy(implicants);
    implicants_destroy(i_copy);
    fplus_destroy(function);
    fplus_copy_destroy(f_copy);
    sopp_destroy(sopp);
    fflush(stdout);
    return 0;
}