/*
 * Used to test the functionalities of bool_plus lib
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "bool_plus.h"
#include "utils.h"

//max value for the boolean plus function output
#define MAX_VALUE 10

typedef enum {
    sopp,
    dsopp,
    sopp_e_time,
    sopp_time,
    dsopp_e_time,
    dsopp_time,
}test_type;

int main(int argc, char** argv) {
    if(argc < 3) {
        printf("Usage: \"%s test_type n_variables [n_tests]\", available test types:\nsopp\ndsopp\n", argv[0]);
        return 1;
    }
    long n_tests = 1;
    if(argc >= 4)
        n_tests = strtol(argv[3], NULL, 0);

    //set type test once and for all
    test_type test;
    //test sopp: does sopp synthesis and prints the form found
    if(strcmp(argv[1], "sopp") == 0)
        test = sopp;
    //test dsopp: does dsopp synthesis and prints the form found
    else if(strcmp(argv[1], "dsopp") == 0)
        test = dsopp;
    //test time of sopp experimental: does sopp synthesis with the experimental version
    //then prints sum of weights of the form found
    else if (strcmp(argv[1], "sopp_e_time") == 0)
        test = sopp_e_time;
    //test time of sopp: does sopp synthesis and prints sum of weights of the form found
    else if(strcmp(argv[1], "sopp_time") == 0)
        test = sopp_time;
    //test time of dsopp: does dsopp synthesis and prints sum of weights of the form found
    else if(strcmp(argv[1], "dsopp_time") == 0)
        test = dsopp_time;
    //test time dsopp experimental: a dsopp time test where sopp_synthesis_experimental is used instead
    //of standard procedure
    else if(strcmp(argv[1], "dsopp_e_time") == 0)
        test = dsopp_e_time;
    else{
        fprintf(stderr, "Test type not recognised, please use one of the following:\nsopp\ndsopp\n");
        return 1;
    }

    for(long i = 0; i < n_tests; i++) {
        long variables = strtol(argv[2], NULL, 0);

        fplus_t *f = fplus_create_random(variables, MAX_VALUE, PROBABILITY_NON_ZERO_VALUE);
        sopp_t *ds = NULL;
        switch(test){
            case sopp:
                assert(f);
                fplus_print(f);
                ds = sopp_synthesis(f);
                assert(ds);
                sopp_print(ds);
                break;
            case dsopp:
                assert(f);
                fplus_print(f);
                ds = dsopp_synthesis(f);
                assert(ds);
                dsopp_print(ds);
                break;
            case sopp_e_time:
                assert(f);
                ds = sopp_synthesis_experimental(f);
                assert(ds);
                printf("%ld\n", sopp_weights_sum(ds));
                break;
            case sopp_time:
                assert(f);
                ds = sopp_synthesis(f);
                assert(ds);
                printf("%ld\n", sopp_weights_sum(ds));
                break;
            case dsopp_e_time:
                assert(f);
                ds = dsopp_synthesis_wexperimental(f);
                assert(ds);
                printf("%ld\n", sopp_weights_sum(ds));
                break;
            case dsopp_time:
                assert(f);
                ds = dsopp_synthesis(f);
                assert(ds);
                printf("%ld\n", sopp_weights_sum(ds));
                break;
        }
        fplus_destroy(f);
        sopp_destroy(ds);
    }
    return 0;
}