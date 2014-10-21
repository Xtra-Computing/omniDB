#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include "stdafx.h"
#include "QP_Utility.h"
#include "MyThreadPool.h"
#include "omp.h"

#define SAVEN_OPen_MP

//the splitter
typedef int (*splitter_t)(void *);

typedef int (*cmp_func)(const void *, const void *);

typedef void (*mapper_t)(void *, void*, void *);//(intput, parameter, output)

#define SEG_PER_THREAD_MODE 
#ifdef SEG_PER_THREAD_MODE
struct ws_scan
{
	Record *data;
	int startID;
	int endID;
	int sum;
};
#else
struct ws_scan
{
	Record *data;
	int startID;
	int endID;
	int delta;
	int sum;
};
#endif

template <class T> struct ws_map
{
	Record *Rin;
	T* Rout;
	mapper_t mapFunc;
	int startID;
	int endID;
	void *para;
};

//the gather and the scatter share this thread structure.
struct ws_scatter
{
	Record *R;
	int startID;
	int endID;
	int *loc;
	Record *S;
};

struct ws_split
{
	int *pidArray;
	int startID;
	int endID;
	int *hist;
	int *loc;
};

struct ws_sort
{
	Record *Rout;
	int startID;
	int endID;
	cmp_func fcn;
};




//compute the sum.

int scan_thread(Record *Rin, int Query_rLen, int numThread);
void scatter_thread(Record *R, int *loc, Record *S, int Query_rLen, int numThread);
void gather_thread(Record *R, int *loc, Record *S, int Query_rLen, int numThread);
void split_thread(Record *Rin, int Query_rLen, int numPart, Record* Rout, int* startHist, mapper_t splitFunc, void* para, int numThread);
void sort_thread(Record* Rin, int Query_rLen, cmp_func fcn, Record* Rout, int numThread);

int scan_openmp(Record *Rin, int Query_rLen, int numThread, int *pS);
void scatter_openmp(Record *R, int *loc, Record *S, int Query_rLen);
void gather_openmp(Record *R, int *loc, Record *S, int Query_rLen);
void split_openmp(Record *Rin, int Query_rLen, int numPart, Record* Rout, int* startHist, mapper_t splitFunc, void* para, int numThread);
void sort_openmp(Record* Rin, int Query_rLen, cmp_func fcn, mapper_t mapFunc, Record* Rout, int numThread);
long int reduce_openmp( Record* Rin, int Query_rLen, int numThread, int OPERATOR );
int filter_openmp( Record* Rin, int Query_rLen, int lowerKey, int higherKey, Record** Rout, int numThread );

//uniform interface

template <class T> void mapImpl(Record *Rin, int Query_rLen, mapper_t mapFunc, void* para, T* Rout, int numThread)
{
#ifndef SAVEN_OPen_MP
	mapImpl_thread<T>(Rin,Query_rLen,mapFunc, para,Rout, numThread);
#else 
	mapImpl_openmp<T>(Rin,Query_rLen,mapFunc,para, Rout);
#endif
}

int scan(Record *Rin, int Query_rLen, int numThread, int* pS);
void scatter(Record *R, int *loc, Record *S, int Query_rLen, int numThread);
void gather(Record *R, int *loc, Record *S, int Query_rLen, int numThread);
void split(Record *Rin, int Query_rLen, int numPart, Record* Rout, int* startHist, mapper_t splitFunc, void *para, int numThread);
void sort(Record* Rin, int Query_rLen, cmp_func fcn, Record* Rout, int numThread);

void sortString_openmp(char* stringData, Record* Rin, int Query_rLen, Record* Rout, int numThread);
//for quick sort
void qsort_pidfunc(void *Rin, void* para, void *Rout);
#endif
