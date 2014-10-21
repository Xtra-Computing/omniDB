#ifndef DATABASE_H
#define DATABASE_H
/*
 *	This file contains the utilities in the GPUQP project
 *  http://ihome.ust.hk/~saven/gpuqp.html
 */

/*
 *	Hardware parameters
 */
//CPU
#include "time.h"
#include "CL\cl.h"
void OP_bufferchecking(cl_mem R_in,size_t size);
double getTimer(int timer);
int genTimer(int timerID);
/************************************************************************/
/*  for testing
/************************************************************************/
#define TEST_MAX ((1<<30))
#define TEST_SMALL 100
typedef enum{
	BLK_NLJ,
		INDEX_NLJ,
		SM_JOIN,
		HASH_JOIN
} JOIN_MODE;

/*
 *	Utilities for generating database workloads
 */
typedef struct {
	int rid;
	int value;
	//int a;
	//int b;
} Record;

#define EVAL(Rhs, Tmp) { Rhs->value = Tmp->value;Rhs->rid = Tmp->rid;}



//generate <OP_rLen> random numbers ranging [0, <max>], maybe duplicated.
void generateRand(Record *R, int max, int OP_rLen, int seed);
void generateRandInt(int *R, int max, int OP_rLen, int seed);


void generateArray(int *R, int base, int step, int max, int OP_rLen, int seed);

//generate <OP_rLen> random sorted numbers ranging [0, <max>], maybe duplicated.
void generateSort(Record *R,  int max, int OP_rLen,int seed);

//generate <OP_rLen> random numbers ranging [0, <max>], distinct.
void generateDistinct(Record *R, int max, int OP_rLen, int seed);

//skewnesses
//generate <OP_rLen> tuples; each key value has <dup> duplicates.
void generateSkewDuplicates(Record *R,  int max, int OP_rLen,int dup, int seed);

//generate <OP_rLen> tuples with <OP_rLen*oneRatio/100> one values.
void generateSkew(Record *R, int max, int OP_rLen, double oneRatio, int seed);

//generate <OP_rLen> tuples with <OP_rLen*oneRatio/100> one values.
void generateSkew(Record *R, int max, int OP_rLen, int oneRatio, int seed);

void generateJoinSelectivity(Record *R, int OP_rLen, Record *S, int sLen, int max, float joinSel,int seed);

int generateString(int numString, int minLen, int maxLen, char **data, int **len, int **offset);


void print(Record *R, int OP_rLen);

void randomize(Record *R, int OP_rLen, int times);
void randomInt(int *R, int OP_rLen, int times);

//#define DEBUG 1
#ifdef DEBUG
#define __DEBUG__(str) {//printf("%s\n", str);}
#define __DEBUG2__(M1,M2) {//printf("%s,%s\n", M1,M2);}
#define __DEBUG3__(M1,M2,M3) {//printf("%s,%s,%s\n", M1,M2,M3);}
#define __DEBUGInt__(intValue) {//printf("%d\n", intValue);}
#define __DEBUGRecord__(intValue1, intValue2) {//printf("%d, %d\n", intValue1,intValue2);}
#else
#define __DEBUG__(str) {;}
#define __DEBUG2__(M1,M2) {;}
#define __DEBUGInt__(intValue) {;}
#define __DEBUG3__(M1,M2,M3) {;}
#define __DEBUGRecord__(intValue1, intValue2) {;}
#endif

#define NUM_SYMBOL 52


#endif