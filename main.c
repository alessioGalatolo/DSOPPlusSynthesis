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
//    printf("Final implicants: \n");
//    for(int i = 0; i < implicants -> size; i++){
//        for(int j = 0; j < function -> variables; j++){
//            printf("%d\t", implicants -> implicants[i][j]);
//        }
//        printf("\n");
//    }
//    printf("\n");
//
//    essentialsp_t* e = essential_implicants(function, implicants);
//
//    printf("Essential implicants: \n");
//    for(int i = 0; i < e -> impl_size; i++){
//        for(int j = 0; j < function -> variables; j++){
//            printf("%d\t", (e -> implicants + i) -> product -> product[j]);
//        }
//        printf("\n");
//    }
//    printf("\n");
//
//    printf("Essential points: \n");
//    for(int i = 0; i < e -> points_size; i++){
//        for(int j = 0; j < function -> variables; j++){
//            printf("%d\t", e -> points[i][j]);
//        }
//        printf("\n");
//    }
//    printf("\n");
//
//    essentials_destroy(e);
    implicants_destroy(implicants);
    fplus_destroy(function);
    sopp_destroy(sopp);
    fflush(stdout);
    return 0;
}