#include "common.h"
struct tempResult{
unsigned int d_numLevelsAllocated;
cl_mem* d_scanBlockSums;//assume max 1000 level.....
int * levelSize;
};
int reduceImpl( cl_mem d_Rin, int rLen, cl_mem d_Rout, int OPERATOR, int numThread, int numMaxBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,tempResult *tR, int _CPU_GPU);
void testReduceImpl( int rLen, int OPERATOR, int numThreadPB, int numMaxBlock);
