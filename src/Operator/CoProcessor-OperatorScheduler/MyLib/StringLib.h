#ifndef STRING_GEN_H
#define STRING_GEN_H
#include "math.h"
#include "stdlib.h"
#include "stdio.h"


#define NUM_SYMBOL 52

struct int4{
	int x;
	int y;
	int z;
	int w;
};

struct char4{
	char x;
	char y;
	char z;
	char w;
};

struct int2{
	int x;
	int y;
};

typedef int4 cmp_type_t;

int generateStringGPU(int numString, int minLen, int maxLen, char **data, int **len, int **offset);
void printString(void * rawData, cmp_type_t* d_R, int OP_rLen);
int compareStringCPULocal(const void *d_a, const void *d_b);

int sort_CPU (void * h_inputKeyArray, int totalKeySize, void * h_inputValArray, int totalValueSize, 
		  cmp_type_t * h_inputPointerArray, int OP_rLen, 
		  void ** h_outputKeyArray, void ** h_outputValArray, 
		  cmp_type_t ** h_outputPointerArray, int2 ** h_outputKeyListRange
		  );

#endif

