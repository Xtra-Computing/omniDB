#include "Helper.h"
#include "common.h"
#include "testSort.h"
#include "testScan.h"
int groupByImpl(cl_mem d_Rin, int rLen, cl_mem d_Rout, cl_mem* d_startPos, int numThread, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void testGroupByImpl( int rLen, int numThread , int numBlock);