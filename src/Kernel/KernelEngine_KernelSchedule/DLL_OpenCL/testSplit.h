#include "common.h"
#define SPLIT (4)
#define PARTITION (5)

void testSplitImpl(int rLen, int numPart, int numThreadPB , int numThreadBlock);
void splitImpl(cl_mem d_R, int rLen, int numPart, cl_mem d_S, cl_mem d_bound,
			   int numThreadPB, int numBlock, int SPLIT_TYPE,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);







/*additional*/
void countHist_int(cl_mem d_pidArray, int rLen, int numPart, cl_mem d_Hist, 
			   int numThreadPB, int numBlock, int sharedMemSize,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void mapPart(cl_mem d_R, int rLen, int numPart, cl_mem d_S, int SPLIT_TYPE,
			 int numThreadPB, int numBlock ,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);