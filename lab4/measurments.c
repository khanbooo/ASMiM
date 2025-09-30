#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include "operations.h"
#include <sched.h>

#define USE(x) asm volatile("" : "+r"(x));
#define NUM_OF_CPUS 4
#define NUM_OF_EXPERIMENTS 5
#define GB (1024ULL * 1024ULL * 1024ULL)
#define DATA_SIZE (GB * NUM_OF_GB)

#define PAGE_SIZE 4096

#ifndef NUM_OF_GB
    #define NUM_OF_GB 1
#endif

#define ld long double


void heat(uint32_t* array, uint32_t size){
    uint32_t t = 0;
    for (uint32_t i = 0; i < size; ++i){
        t = array[t];
    }
    USE(t);
}

typedef struct context{
    void * address;
    void * another_address;
    ull size;
    int cpu;
    int operation;
    pthread_barrier_t *barrier;
} context_t;

ull alignment(ull number, ull alignment_size){
    return (number + alignment_size - 1) & ~(alignment_size - 1);
}

void * allocate(ull size){
    void * address = mmap(NULL, alignment(size, PAGE_SIZE) + PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (address == MAP_FAILED){
        printf("memory allocation failed. pid : %d", getpid());
        exit(1);
    }

    return address;
}

void set_cpu(int n) {
  int err;
  cpu_set_t cpuset;
  pthread_t tid = pthread_self();

  CPU_ZERO(&cpuset);
  CPU_SET(n, &cpuset);

  err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
  if (err) {
    printf("pthread_setaffinity error. cpu_number: %d\n", n);
    return;
  }
}

void * thread_func(void * args){
    context_t * context = args;

    struct timespec start, end;

    set_cpu(context->cpu);

    int op = context->operation;

    if (op == 0){
        
        pthread_barrier_wait(context->barrier);
        // printf("im here\n");
        // fflush(stdout);
        mem_read(context->address, context->size);
        
        clock_gettime(CLOCK_MONOTONIC, &start);
        mem_read(context->address, context->size);
        // printf("im here\n");
        // fflush(stdout);
        clock_gettime(CLOCK_MONOTONIC, &end);
        mem_read(context->address, context->size);
        // printf("im here\n");
        // fflush(stdout);
    }
    else if (op == 1){
        pthread_barrier_wait(context->barrier);

        mem_write(context->address, context->size);
        clock_gettime(CLOCK_MONOTONIC, &start);
        mem_write(context->address, context->size);
        clock_gettime(CLOCK_MONOTONIC, &end);
        mem_write(context->address, context->size);
    }
    else{
        pthread_barrier_wait(context->barrier);

        mem_copy(context->address, context->another_address, context->size);
        clock_gettime(CLOCK_MONOTONIC, &start);
        mem_copy(context->address, context->another_address, context->size);
        clock_gettime(CLOCK_MONOTONIC, &end);
        mem_copy(context->address, context->another_address, context->size);
    }
    
    
    
    long seconds = (end.tv_sec - start.tv_sec) * 1000000000ULL;
    long nanoseconds = (end.tv_nsec - start.tv_nsec);
    ull total_time_nanoseconds = seconds * 1000000000ULL + nanoseconds;

    free(context);
    return (void *)total_time_nanoseconds;
}

int main(){
    void * address = allocate(DATA_SIZE);
    void * another_address = allocate(DATA_SIZE);
    ull size = DATA_SIZE;
    memset(address, 0, size);
    memset(another_address, 0, size);

    heat(address, DATA_SIZE);

    for (int threads = 1; threads <= NUM_OF_CPUS; threads++){
        // printf("threads: %d\n", threads);
        double results[3] = {0, 0, 0};

        for (int operation = 0; operation < 3; operation++){
            // printf("operation %d\n", operation);
            for (int exp = 0; exp < NUM_OF_EXPERIMENTS; exp++){
                fflush(stdout);
                pthread_t tids[threads];
                pthread_barrier_t barrier;
                pthread_barrier_init(&barrier, NULL, threads);

                fflush(stdout);

                ull part_size = size / threads;

                for (int i = 0; i < threads; i++){
                    context_t * context = (context_t *)malloc(sizeof(context_t));
                    context->cpu = i;
                    context->barrier = &barrier;
                    context->operation = operation;
                    context->size = alignment(part_size, 128);
                    context->address = (void *)alignment((size_t)(address + i * part_size), 128);
                    // printf("address : %llu\n", context->address);
                    context->another_address = (void *)alignment((size_t)(another_address + i * part_size), 128);
                    pthread_create(&tids[i], NULL, thread_func, context);
                }

                long double result = 0.0;

                for (int i = 0; i < threads; i++){
                    ull nsecs;
                    pthread_join(tids[i], &nsecs);
                    // printf("nsecs : %lf\n", nsecs);
                    // fflush(stdout);
                    result += ((ld)part_size / ((ld)GB)) / ((ld)nsecs / (ld)1e9);  
                }

                // printf("result : %lf\n", result);
                // fflush(stdout);

                pthread_barrier_destroy(&barrier);

                if (result > results[operation]){
                    results[operation] = result;
                }
            }
        }

        printf("%d %.3lf %.3lf %.3lf\n", threads, results[0], results[1], results[2]);
    }
    munmap(address, size);
    munmap(another_address, size);
}