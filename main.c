#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool_utils.h"
#include "bool_plus.h"

#define VARIABLES 4
#define MAX_VALUE 10

int main() {
    fplus* function = fplus_create_random(VARIABLES, MAX_VALUE);
    fplus_print(function);
//    sopp* sopp = sopp_create();
    implicant_plus* implicants = prime_implicants(function);
//    for(int i = 0; i < implicants -> size; i++){
//        for(int j = 0; j < function -> variables; j++){
//            printf("%d\t", implicants -> implicants[i][j]);
//        }
//        printf("\n");
//    }
//    printf("\n");
//
//    essentials* e = essential_implicants(function, implicants);



    return 0;
}