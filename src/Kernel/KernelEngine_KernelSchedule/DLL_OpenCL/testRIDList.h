#include "common.h"
void setRIDListImpl (cl_mem d_RIDList, cl_mem d_tempOutput,int delta, int rLen, cl_mem d_destRin,  int numThreadPB, int numBlock
	,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void getRIDListImpl (cl_mem d_RIDList,int delta, int rLen, cl_mem d_destRin, cl_mem d_tempOutput,  int numThreadPB, int numBlock
	,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);