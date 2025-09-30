#include "papi.h"

#include <stdint.h> 
#include <stdio.h> 
// clang-format off 
#ifndef NUM_OF_BRANCHES 
    #define NUM_OF_BRANCHES 0 
#endif 

#define STRINGIFY(x) #x
#define MAX_PATTERN_LENGTH 100
#define NUM_OF_ITERATIONS 1000000
#define NUM_OF_EXPERIMENTS 5 
#define DUMMY_BRANCHES(N) asm volatile( \
     ".rept " STRINGIFY(N) "\n\t" \
     "cmp %%eax, %%eax\n\t"       \
     "je .+3\n\t"                 \
     "nop\n\t"                    \
     ".endr"                      \
     ::: "cc"                     \
) 
// clang-format on

void test(int iterations, int pattern_length) { 
    asm volatile("nop") ;
    for (int i = 0; i < iterations; i++) { 
        if (i % pattern_length <= 1) { 
            asm volatile("nop") ;
        } 
        
        DUMMY_BRANCHES(NUM_OF_BRANCHES); 
    } 
}

int main() { 
    PAPI_library_init(PAPI_VER_CURRENT);
    int event_set = PAPI_NULL; 
    PAPI_create_eventset(&event_set); 
    PAPI_add_event(event_set, PAPI_BR_MSP); 
    long long min_br_misses, curr_br_misses;
    for (int pattern_length = 1; pattern_length < MAX_PATTERN_LENGTH; pattern_length++) { 
        for (int i = 0; i < NUM_OF_EXPERIMENTS; i++) { 
            PAPI_start(event_set); 
            test(NUM_OF_ITERATIONS, pattern_length); 
            PAPI_stop(event_set, &curr_br_misses);
            if (i == 0 || curr_br_misses < min_br_misses) { 
                min_br_misses = curr_br_misses; 
            }
        } 
        printf("%d %lld\n", pattern_length, min_br_misses); 
    } 
    PAPI_cleanup_eventset(event_set);
    PAPI_destroy_eventset(&event_set); 
}