#include "papi.h"

#include <stdint.h> 
#include <stdio.h> 
// clang-format off 
#ifndef NUM_OF_BRANCHES 
    #define NUM_OF_BRANCHES 0 
#endif 

#ifndef ZERO 
    #define ZERO 0 
#endif 

#ifndef ONE 
    #define ONE 1
#endif 

#define STRINGIFY(x) #x
#define MAX_PATTERN_LENGTH 100
#define NUM_OF_ITERATIONS (1000 * 1000 * 1) 
#define NUM_OF_EXPERIMENTS 100
#define NOP asm volatile("nop") 
#define DUMMY_BRANCHES(N) asm volatile( \
     ".rept " STRINGIFY(N) "\n\t" \
     "cmp %%eax, %%eax\n\t"       \
     "je .+3\n\t"                 \
     "nop\n\t"                    \
     ".endr"                      \
     ::: "cc"                     \
) 
// clang-format on

void test0(int iterations, int pattern_length) { 
    for (int i = 0; i < iterations; i++) { 
        if (i % pattern_length == 0) { 
            NOP; 
        } 
    } 
}

int main() { 
    PAPI_library_init(PAPI_VER_CURRENT);
    
    long long without_misses[2] = {0};
    long long with_misses[2] = {0}; 
    int pattern_length = 19;
    long long min_without[2] = {0};
    long long min_with[2] = {0};

    for (int i = 0; i < NUM_OF_EXPERIMENTS; i++){
        int event_set = PAPI_NULL; 

        PAPI_create_eventset(&event_set); 
        PAPI_add_event(event_set, PAPI_TOT_CYC); 
        PAPI_add_event(event_set, PAPI_BR_MSP); 

        PAPI_start(event_set); 
        test0(NUM_OF_ITERATIONS, pattern_length); 
        PAPI_stop(event_set, without_misses);

        PAPI_cleanup_eventset(event_set);
        PAPI_destroy_eventset(&event_set); 

        if (i == 0){
            min_without[0] = without_misses[0];
            min_without[1] = without_misses[1];
        }
        else{
            if (without_misses[0] < min_without[0]){
                min_without[0] = without_misses[0];
            }
            if (without_misses[1] < min_without[1]){
                min_without[1] = without_misses[1];
            }
        }
       
    }

    pattern_length++;
    
    for (int i = 0; i < NUM_OF_EXPERIMENTS; i++){
        int another_event_set = PAPI_NULL; 

        PAPI_create_eventset(&another_event_set); 
        PAPI_add_event(another_event_set, PAPI_TOT_CYC); 
        PAPI_add_event(another_event_set, PAPI_BR_MSP); 

        PAPI_start(another_event_set); 
        test0(NUM_OF_ITERATIONS, pattern_length); 
        PAPI_stop(another_event_set, with_misses);
        
        PAPI_cleanup_eventset(another_event_set);
        PAPI_destroy_eventset(&another_event_set); 

        if (i == 0){
            min_with[0] = with_misses[0];
            min_with[1] = with_misses[1];
        }
        else{
            if (with_misses[0] < min_with[0]){
                min_with[0] = with_misses[0];
            }
            if (with_misses[1] < min_with[1]){
                min_with[1] = with_misses[1];
            }
        }
    }
    

    printf("Without misses: %lld %lld\n", min_without[0], min_without[1]);
    printf("With misses: %lld %lld\n", min_with[0], min_with[1]);

    long double dCYC_dMSP = ((long double)min_with[0] - (long double)min_without[0]) /
                             ((long double)min_with[1] - (long double)min_without[1]);

    printf("Cycles per branch: %Lf\n", dCYC_dMSP);
}