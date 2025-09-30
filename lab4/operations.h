#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <immintrin.h>
#define ull unsigned long long

void mem_read(void * address, ull size);
void mem_write(void * address, ull size);
void mem_copy(void * address, void * another_addr, ull size);