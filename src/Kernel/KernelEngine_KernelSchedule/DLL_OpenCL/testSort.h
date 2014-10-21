#include "common.h"
void radixSortImpl(cl_mem d_R, int rLen, int keybits, int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void testSortImpl(int rLen, int numThreadPB, int numBlock);