/*
 * Used to test the functionalities of bool_plus lib
 */

#include <stdio.h>
#include <assert.h>
#include <bits/time.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "bool_plus.h"
#include "utils.h"

#define VARIABLES 4
#define MAX_VALUE 10
#define NON_ZERO_CHANCE 50


int main(int argc, char** argv) {
    if(argc == 1)
        return 1;

    //test time of sopp experimental
    if(strcmp(argv[1], "sopp_e_time") == 0){
        assert(argc >= 3);
        fplus_t* f = fplus_create_random(strtol(argv[2], NULL, 0), MAX_VALUE, PROBABILITY_NON_ZERO_VALUE);
        sopp_destroy(sopp_synthesis_experimental(f));
        fplus_destroy(f);
        return 0;
    }
    //test time of sopp
    if(strcmp(argv[1], "sopp_time") == 0){
        assert(argc >= 3);
        fplus_t* f = fplus_create_random(strtol(argv[2], NULL, 0), MAX_VALUE, PROBABILITY_NON_ZERO_VALUE);
        sopp_destroy(sopp_synthesis(f));
        fplus_destroy(f);
        return 0;
    }
    //test time of dsopp
    if(strcmp(argv[1], "dsopp_time") == 0){
        assert(argc >= 3);
        fplus_t* f = fplus_create_random(strtol(argv[2], NULL, 0), MAX_VALUE, PROBABILITY_NON_ZERO_VALUE);
        sopp_destroy(dsopp_synthesis(f));
        fplus_destroy(f);
        return 0;
    }
    //test dsopp
    if(strcmp(argv[1], "dsopp") == 0){
        assert(argc >= 3);
        fplus_t* f = fplus_create_random(strtol(argv[2], NULL, 0), MAX_VALUE, PROBABILITY_NON_ZERO_VALUE);
        dsopp_t* dsopp = dsopp_synthesis(f);
        dsopp_print(dsopp);
        sopp_destroy(dsopp);
        fplus_destroy(f);
        return 0;
    }
    return 1;
}