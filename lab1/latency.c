#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void latencyCheck(){
	union ticks{
		unsigned long long t64;
		struct s32 { long th, tl; } t32;
	} start, end;
	register int a = 1;
	register int b = 1;
	int * m = malloc(1000000 * sizeof(int));
	memset(m, 0, 1000000 * sizeof(int));
	for (int i = 0; i < 10; i++){
		for (int j = 0; j < 10; j++){
			for (int k = 0; k < 1000000; k++){
				m[k] += i * j;
			}
		}
	}

	asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));

	for (register int i = 0; i < 0; i++){
		a *= b;
		a *= b;
		a *= b;
		a *= b;
	}

	asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));

	printf("Ticks taken: %llu\n", (end.t64-start.t64));
}

int main(){
	latencyCheck();
}

