#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#define STRINGIFY(x) #x
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)

#define GEN_NOPS(N) asm volatile (".rept " EXPAND_AND_STRINGIFY(N) "\n\t nop\n\t .endr")
#ifndef TEST_NOPS
    #define TEST_NOPS 10
#endif

#define SIZE (uint32_t)(1024 * 1024 * 20)

void used(long value) {
    asm volatile("" : "+r"(value));
}

void heat(uint32_t* array, uint32_t size){
    uint32_t t = 0;
    for (uint32_t i = 0; i < size; ++i){
        t = array[t];
    }
    used(t);
}

void swap(uint32_t* first, uint32_t* second){
    uint32_t temp = *second;
    (*second) = (*first);
    (*first) = temp;
}

void shuffle(uint32_t* array, uint32_t size){
    for (uint32_t i = size - 1; i > 0; i--){
        swap(&array[i], &array[rand() % i]);
    }
}

void fillAnArray(uint32_t* array, uint32_t size){
    for (uint32_t i = 0; i < size; ++i){
        array[i] = i;
    }
    shuffle(array, size);
}

void printAnArray(uint32_t* array, uint32_t size){
    for (uint32_t i = 0; i < size; ++i){
        printf("%d ", array[i]);
    }
    printf("\n");
}

void memoryReorderBufferVolumeTest(uint32_t* array, uint32_t operations){
    uint32_t k = 0;

    uint64_t beg = __builtin_ia32_rdtsc();

    for (uint32_t i = 0; i < operations; ++i){
        k = array[k];
        GEN_NOPS(TEST_NOPS);
    }

    uint64_t end = __builtin_ia32_rdtsc();
    printf("%.3lf ", (double)(end - beg) / (double)operations);
    used(k);
}

int main(){\
    srand(time(NULL));
    uint32_t * array = (uint32_t *)malloc(SIZE);
    fillAnArray(array, SIZE / sizeof(uint32_t));
    heat(array, SIZE / sizeof(uint32_t));
    memoryReorderBufferVolumeTest(array, SIZE);
    return 0;
}
