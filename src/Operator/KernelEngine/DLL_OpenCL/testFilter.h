#include "common.h"
void testFilterImpl( int rLen, int numThreadPB, int numBlock);

void filterImpl( cl_mem d_Rin, int beginPos, int rLen, cl_mem* d_Rout, int* outSize, 
				int numThread, int numBlock, int smallKey, int largeKey,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU);