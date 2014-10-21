#include "testSort.h"
#include "KernelScheduler.h"
#include "OpenCL_DLL.h"
extern "C" void CL_RadixSortOnly(cl_mem d_Rin, int rLen,int numThread, int numBlock, int _CPU_GPU)
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	radixSortImpl(d_Rin, rLen, 32, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	deschedule(CPU_GPU,burden);
}
extern "C" void CL_BitonicSortOnly(cl_mem d_Rin, int rLen,cl_mem d_Rout,int numThread, int numBlock, int _CPU_GPU)
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	cl_copyBuffer(d_Rout,d_Rin,sizeof(Record) * rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	radixSortImpl(d_Rin, rLen, 32, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	deschedule(CPU_GPU,burden);
}
extern "C" void CL_RadixSort(Record* h_Rin, int rLen,Record* h_Rout,int numThread, int numBlock, int _CPU_GPU)
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	int memSize = sizeof(Record) * rLen;
	cl_mem d_Rin;
	CL_MALLOC(&d_Rin,memSize);

	cl_writebuffer(d_Rin, h_Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);

	radixSortImpl( d_Rin, rLen, 32, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	cl_readbuffer( h_Rout, d_Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	//validateSort((Record*) h_Rout, rLen);	
	CL_FREE(d_Rin);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
}