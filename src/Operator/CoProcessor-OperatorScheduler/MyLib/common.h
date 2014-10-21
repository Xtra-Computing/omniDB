#ifndef COMMON_H
#define COMMON_H
#include "CL\cl.h"
#include "QP_Utility.h"
#include "Primitive.h"
#include "LinkedList.h"
#include "hashTable.h"
#include <iostream>
using namespace std;

#define divRoundDown(n,s)  ((n) / (s))
#define divRoundUp(n,s)    (((n) / (s)) + ((((n) % (s)) > 0) ? 1 : 0))

#define TEST_MIN (0)

#define REDUCE_SUM (0)
#define REDUCE_MAX (1)
#define REDUCE_MIN (2)

#define FILTER_CONDITION ( Rin[i].value < 500000000 )

//three procedures for the split
void computeHist(int *pidArray, int start, int end, int *localHist, int cpuid=0, int numThread=3);
void histToOffset(int *startHist, int **hist, int numPart, int numThread);
void outputLocInSplit(int* pidArray, int start, int end, int *localHist, int* loc, int cpuid=0, int numThread=3);

//pivots 
void getPivot(Record *Rin, int OP_rLen, int sampleLevel, cmp_func fcn, Record** sampleRout);

//log2
int log2(int value);
int log2Ceil(int value);
int compare (const void * a, const void * b);
int compareStringCPU(const void *d_a, const void *d_b);
void printString(char *data, Record* d_R, int OP_rLen);

//two producdure for validation
void validateSort(Record *R, int OP_rLen);
void validateSplit(Record *R, int OP_rLen, int numPart);

//merge multiple linked list into a relation, return the total number of elements.
int dumpMLLtoArray(LinkedList **llList, int numLL, Record **Rout);
void initMLL(LinkedList **llList, int numLL);
void freeMLL(LinkedList **llList, int numLL);

//binary search value searchValue, so that R[i-1].y< x < R[i].y, R is sorted
int findLargestSmaller(Record *R, int start, int end, int searchValue);
//find the quantile, R is sorted.
void findQuantile(Record *R, int start, int end, int numQuantile, int* quanPos);

#endif
