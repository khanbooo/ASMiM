#include "stdio.h"
#include "string.h"
#include "operations.h"

#define STRINGIFY(x) #x
#define DO_PRAGMA_UNROLL(x) _Pragma(STRINGIFY(GCC unroll(x)))
#define USE(x) asm volatile("" : "+r"(x));

void mem_read(void * address, ull size){
    void * retcode = memchr(address, 0xFF, size);
    USE(retcode);
}

void mem_write(void * address, ull size){
    void * retcode = memset(address, 0x00, size);
    USE(retcode);
}

void mem_copy(void * address, void * another_address, ull size){
    void * retcode = memcpy(address, another_address, size);
    USE(retcode);
}