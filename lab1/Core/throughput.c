#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10000000

void throughputCheck(){
	union ticks{
		unsigned long long t64;
		struct s32 { long th, tl; } t32;
	} start, end;
	register int a = 1234;
	register int b = 1283657;
	register int c = 12857;
	register int d = 1657;
	register int e = 16;
	register int f = 45;
	register int * m = (int *)malloc(SIZE * sizeof(int));
	memset(m, 0, SIZE * sizeof(int));
	for (register int i = 0; i < 10; i++){
		for (register int j = 0; j < 10; j++){
			for (register int k = 0; k < SIZE / 10; k++){
				m[k] += i * j;
			}
		}
	}

	asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));

	for (register int i = 0; i < SIZE; i++){
		a *= f;
		b *= f;
		c *= f;
		d *= f;
		e *= f;
	}

	asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));

	printf("Ticks taken: %llu\n", (end.t64-start.t64));
	printf("%d\n", a);
	printf("%d\n", b);
	printf("%d\n", c);
	printf("%d\n", d);
	printf("%d\n", e);
}

int main(){
	throughputCheck();
}

