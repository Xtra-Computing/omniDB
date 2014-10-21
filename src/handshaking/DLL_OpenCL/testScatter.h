#include "common.h"
void gatherImpl(cl_mem d_R, int rLen, cl_mem d_loc, cl_mem d_S, int sLen, 
				int numThreadPB , int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void scatterImpl(cl_mem d_R, int rLen, cl_mem d_loc, cl_mem d_S, 
				 int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void scatterImpl_forPart(cl_mem d_R, int rLen, int numPart, cl_mem d_loc, cl_mem d_S,
						 int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);


void testGatherImpl(int rLen, int numThread , int numBlock);
void testScatterImpl(int rLen, int numThreadPB, int numBlock);