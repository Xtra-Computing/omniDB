#include "common.h"
void setValueListImpl (cl_mem d_ValueList,int delta, int rLen, cl_mem d_destRin,  int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void getValueListImpl (cl_mem d_Rin,int delta, int rLen, cl_mem* d_ValueList, cl_mem d_tempOutput,  int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void testsetValueListImpl(int rLen, int numThreadPB , int numBlock, int CPU_GPU );