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
    sopp* sopp = sopp_create();
    implicant_plus* implicants = prime_implicants(function);
    printf("Final implicants: \n");
    for(int i = 0; i < implicants -> size; i++){
        for(int j = 0; j < function -> variables; j++){
            printf("%d\t", implicants -> implicants[i][j]);
        }
        printf("\n");
    }
    printf("\n");
//
    essentials* e = essential_implicants(function, implicants);

    for(int i = 0; i < e -> size; i++){
        for(int j = 0; j < function -> variables; j++){
            printf("%d\t", e -> implicants[i] . product -> product[j]);
        }
        printf("\n");
    }
    printf("\n");

    for(int i = 0; i < e -> size; i++){
        for(int j = 0; j < function -> variables; j++){
            printf("%d\t", e -> points[i][j]);
        }
        printf("\n");
    }
    printf("\n");



    return 0;
}