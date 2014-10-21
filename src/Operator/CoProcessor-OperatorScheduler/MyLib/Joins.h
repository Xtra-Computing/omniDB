#ifndef JOINS_H
#define JOINS_H
#include "stdafx.h"
#include "QP_Utility.h"
#include "omp.h"
#include "CC_CSSTree.h"
#include <iostream>
using namespace std;

#define PRED_EQUAL2(DATA) (DATA[0]==DATA[1])
#define CPU_CACHE_SIZE (256*1024)//1024*1024 bytes
#define CPU_BLOCK_SIZE 128//bytes
#define NLJ_BLOCK_SIZE (CPU_CACHE_SIZE/2/sizeof(Record))
#define BASE_BITS 6

void partNinlj(Record *R, int startR, int endR, Record *S, int startS, int endS, LinkedList *ll, int cpuid, int numThread);
int ninlj_omp(Record *R, int OP_rLen, Record *S, int sLen, Record** Rout, int numThread);
int inlj_omp(Record *R, int OP_rLen, CC_CSSTree *tree, Record *S, int sLen, Record** Rout, int numThread);
int smj_omp(Record *R, int OP_rLen, Record *S, int sLen, Record** Rout, int numThread);
int hj_omp(Record *R, int OP_rLen, Record *S, int sLen, Record** Rout, int numThread);
int MergeJoinSortedRelation_omp(Record *R, int OP_rLen, Record *S, int sLen, Record** Rout, int numThread);



#endif
