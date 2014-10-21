#include "CL\cl.h"
#include <stdio.h>
#include "string.h"
#include <cstdlib>
#include "stdlib.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <SDKFile.hpp>
#include <SDKCommon.hpp>
#include <SDKApplication.hpp>
#include "verctor_types.h"
#define _tonyPrint_(STR) printf(STR)
//#define _tonyPrint_(STR)
#include "assert.h"
#define COALESCED
void bufferchecking(cl_mem R_in,size_t size);
typedef uint2 Rec;
typedef uint2 Record;
#define TEST_MAX (1<<30)
#define TEST_MIN (0)
//new
int floorPow2(int n);
cl_int cl_gpu_malloc(cl_mem *mem, cl_mem_flags flag, cl_int size);
int compare (const void * a, const void * b);
void randomize(int2 rLen, int times);
void int_randomize(int *R, int rLen, int times);
void fillint(int*R,int rLen);
void generateRand(Record *R, int maxmax, int rLen, int seed);
void generateRandInt(int *R, int max, int rLen, int seed);
void generateSkewDuplicates(int2 *R,  int rLen,int2 *S, int sLen, int max, int dup, int seed);
void generateJoinSelectivity(int2 *R, int rLen, int2 *S, int sLen, int max, float joinSel,int seed);
void generateArray(int *R, int base, int step, int max, int rLen, int seed);
void generateSort(Record *R, int maxmax, int rLen, int seed);
void generate16Bits(int *a, int max, int len, int seed);
void generateDistinct(int2 *R, int max, int rLen, int seed);
void print(int2 *R, int rLen);
void generateSkew(int2 *R, int max, int rLen, float oneRatio, int seed);

int log2(int value);
int log2Ceil(int value);


cl_int cl_malloc(cl_mem *mem, cl_mem_flags flag, cl_int size);
void wait(int index,cl_event *eventList);
/*KERNEL schedule enabled.*/
void cl_readbuffer(void* to, cl_mem from, size_t siz,int *index,cl_event *eventList,int *CPU_GPU,double * burden,int _CPU_GPU);
void cl_readbuffer(void* to, cl_mem from, size_t offset, size_t size,int *index,cl_event *eventList,int *CPU_GPU,double * burden,int _CPU_GPU);
void cl_writebuffer(cl_mem to, void* from, size_t size,int *index,cl_event *eventList,int *CPU_GPU,double * burden,int _CPU_GPU);
void cl_copyBuffer(cl_mem dest, cl_mem src, size_t size,int *index,cl_event *eventList,int *CPU_GPU,double * burden,int _CPU_GPU);
void cl_copyBuffer(cl_mem dest, int destOffset, cl_mem src, size_t size,int *index,cl_event *eventList,int *CPU_GPU,double * burden,int _CPU_GPU);
void cl_copyBuffer(cl_mem dest, int destOffset, cl_mem src, int srcOffset, size_t size,int *index,cl_event *eventList,int *CPU_GPU,double * burden,int _CPU_GPU);
/*Original*/
void cl_readbuffer(void* to, cl_mem from, size_t siz,int CPU_GPU);
void cl_writebuffer(cl_mem to, void* from, size_t size,int CPU_GPU);
void cl_copyBuffer(cl_mem dest, cl_mem src, size_t size,int CPU_GPU);

void cl_init(cl_device_type TYPE);
void cl_init_common ();
void cl_clean (int iExitCode);
void cl_prepareProgram(char* cSourceFile, char* dir);
void cl_getKernel(char* kernelName,cl_kernel *kernel);
void cl_getKernel(char* kernelName,int CPU_GPU);
void cl_launchKernel(cl_uint work_dim, const size_t *groups, size_t *threads,cl_kernel *Kernel,int CPU_GPU);

#define HOST_MALLOC(PTR,SIZE) PTR=(void *)malloc(SIZE);
#define HOST_FREE(PTR) free(PTR)

#define CL_MALLOC_R(PTR,SIZE) cl_malloc(PTR,CL_MEM_READ_ONLY,SIZE)
#define CL_MALLOC(PTR,SIZE) cl_malloc(PTR,CL_MEM_READ_WRITE,SIZE)
#define CL_MALLOC_W(PTR,SIZE) cl_malloc(PTR,CL_MEM_WRITE_ONLY,SIZE)
#define CL_FREE(PTR) if(PTR)clReleaseMemObject(PTR);

#define __DEBUG__(STR) printf(STR);

#define GPU


#define REDUCE_SUM (0)
#define REDUCE_MAX (1)
#define REDUCE_MIN (2)
#define REDUCE_AVERAGE (3)
#define SHARED_MEMORY_PER_PROCESSOR (8*1024)
#define NLJ_NUM_PROCESSOR 16//for GTX
#define NLJ_SHARED_MEM_PER_PROCESSOR (SHARED_MEMORY_PER_PROCESSOR)
#define NLJ_SHARED_MEM (NLJ_SHARED_MEM_PER_PROCESSOR*NLJ_NUM_PROCESSOR)
#define NLJ_MAX_NUM_BLOCK_PER_DIM (32*1024)
#define NLJ_NUM_THREADS_PER_BLOCK 256
#define NLJ_NUM_TUPLE_PER_THREAD 2
#define NLJ_S_BLOCK_SIZE (NLJ_NUM_THREADS_PER_BLOCK*NLJ_NUM_TUPLE_PER_THREAD)
#define NLJ_R_BLOCK_SIZE NLJ_NUM_THREADS_PER_BLOCK

#define NUM_DELTA_PER_BLOCK 8 
#define SMJ_NUM_THREADS_PER_BLOCK 256
#define IntCeilDiv(a, b) ( (int)ceilf((a) / float(b))	)

#define MaxTag 64
//debug?
#define _bDebug 0

#define _maxPartLen (_bDebug? 2: 512)	//max partition length. Limited by shared memory size (4k to be safe): sizeof(Rec) * maxPartLen <= 4k
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

