#include "common.h"
#include "OpenCL_DLL.h"
int INLJImpl(cl_mem R, int rLen, CUDA_CSSTree *d_tree, cl_mem d_S, int sLen, cl_mem* d_Rout,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU);

void testINLJ(int rLen, int sLen);

int NINJImpl(cl_mem d_R, int rLen, cl_mem d_S, int sLen, cl_mem* d_Rout,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU);

void testNINLJ(int rLen, int sLen);

int MJImpl( cl_mem d_Rin, int rLen, cl_mem d_Sin, int sLen, cl_mem* d_Joinout,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden ,int _CPU_GPU);
int SMJImpl(cl_mem d_R, int rLen, cl_mem d_S, int sLen, cl_mem* d_Joinout,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void joinMBCount(cl_mem d_R, int rLen, cl_mem d_S, 
			int sLen, cl_mem d_quanLocS, int numQuan,
			cl_mem d_n, dim3 grid_NLJ, dim3 threads_NLJ,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);

void getQuantile(cl_mem d_R, int rLen, int interval, cl_mem d_output, int numQuantile,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);

void testSMJ(int rLen, int sLen);
void testMJ(int rLen, int sLen);

int HJImpl(cl_mem d_R, cl_uint rLen, cl_mem d_S, cl_uint sLen, cl_mem rHashTable, cl_mem* d_Rout,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void testHJ(int rLen, int sLen);