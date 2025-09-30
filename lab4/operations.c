#include "stdio.h"
#include "operations.h"

#define STRINGIFY(x) #x
#define DO_PRAGMA_UNROLL(x) _Pragma(STRINGIFY(GCC unroll(x)))
#define USE(x) asm volatile("" : "+v"(x));

void mem_read(void * address, ull size){
    TYPE_T * array = address;
    DO_PRAGMA_UNROLL(UNROLL) for (ull i = 0; i < size / sizeof(TYPE_T); i++){
        // printf("i = %d\n", i);
        TYPE_T value = READ_VALUE(&array[i]);
        USE(value);
    }
}

void mem_write(void * address, ull size){
    TYPE_T * array = address;
    TYPE_T null_value = NULL_VALUE;
    DO_PRAGMA_UNROLL(UNROLL) for (ull i = 0; i < size / sizeof(TYPE_T); i++){
        WRITE_VALUE(&array[i], null_value);
    }
}

void mem_copy(void * address, void * another_address, ull size){
    TYPE_T * array2 = address;
    TYPE_T * array1 = another_address;

    DO_PRAGMA_UNROLL(UNROLL) for (ull i = 0; i < size / sizeof(TYPE_T); i++){
        TYPE_T value = READ_VALUE(&array1[i]);
        WRITE_VALUE(&array2[i], value);
    }
}


