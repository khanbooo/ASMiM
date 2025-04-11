#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define STRINGIFY(x) #x
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)

#define GEN_NOPS(N) asm volatile (".rept " EXPAND_AND_STRINGIFY(N) "\n\t nop\n\t .endr")
#ifndef TEST_NOPS
    #define TEST_NOPS 10
#endif

#define OPERATIONS (1024 * 1024 * 20)

void used(long value) {
    asm volatile("" : "+r"(value));
}

void heat(){
    double value = 3.0;

    for (uint32_t i = 0; i < OPERATIONS; i++){
        value = sqrt(value);
    }

    used(value);
}

void work(uint32_t operations){
    double value = 3.0;

    uint64_t beg = __builtin_ia32_rdtsc();

    for (uint32_t i = 0; i < operations; ++i){
        value = sqrt(value);
        GEN_NOPS(TEST_NOPS);
    }

    uint64_t end = __builtin_ia32_rdtsc();
    printf("%.3lf", (double)(end - beg) / (double)operations);
    used(value);
}

int main(){
    work(OPERATIONS);
    return 0;
}
