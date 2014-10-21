#include "scheduler.h"
void kernel_initial();
void kernel_enqueue(int size,int kid,cl_uint work_dim, const size_t *groups, size_t *threads,cl_event *List,int* eventIndex,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void kernel_clean();

/////////////////////////////////////////////////
double getAddBurden_Write(int* Flag_CPU_GPU,int size);
double getAddBurden_Read(int* Flag_CPU_GPU,int size);
double getAddBurden_Copy(int* Flag_CPU_GPU,int size);
double getAddCPUBurden(int kid,int size);
double getAddGPUBurden(int kid,int size);