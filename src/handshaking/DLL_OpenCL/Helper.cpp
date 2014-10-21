#include "common.h"
#include "Helper.h"
#include <SDKCommon.hpp>
#include <SDKFile.hpp>
#include <SDKCommon.hpp>
#include <SDKApplication.hpp>
#include "KernelScheduler.h"
// OpenCL Vars---------0 for CPU, 1 for GPU
extern cl_context Context;        // OpenCL context
extern cl_program Program;           // OpenCL program
extern cl_command_queue CommandQueue[2];// OpenCL command que
extern cl_platform_id Platform[2];      // OpenCL platform
extern cl_device_id Device[2];          // OpenCL device
extern cl_kernel Kernel[2];             // OpenCL kernel
extern cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
extern cl_device_id allDevices[10];

streamsdk::SDKCommon *DLL_sampleCommon=new streamsdk::SDKCommon();
int DLL_genTimer(int timerID)
{
#ifdef NV
	shrDeltaT(timerID);
	return timerID;
#else
	int timer = DLL_sampleCommon->createTimer();
    DLL_sampleCommon->resetTimer(timer);
    DLL_sampleCommon->startTimer(timer); 
	return timer;
#endif
	
}
void DLL_stopTimer(int timer){
	DLL_sampleCommon->stopTimer(timer);
}
double DLL_getTimer(int timer)
{
	/*With Kernel Scheduler*/
//	cl_int err1=clFinish(CommandQueue[0]);
//	cl_int err2=clFinish(CommandQueue[1]);
//	printf("DLL_getTimer calls finish commandqueue first");
#ifdef NV	
	double elapsedTimeInSec = shrDeltaT(timer);

	return elapsedTimeInSec;
#else
	 DLL_sampleCommon->stopTimer(timer);
	 double writeTime = (double)(DLL_sampleCommon->readTimer(timer));
	 DLL_sampleCommon->resetTimer(timer);
	 DLL_sampleCommon->startTimer(timer); 
	 return writeTime;
#endif
}
double endTime(char *info)
{
	double result=DLL_getTimer(0);
	printf("***%s, time, %f, ms***\n", info, result);
	return result;
}


double endTimer( char* info, int* timer )
{
	double result=DLL_getTimer(*timer);
	printf("***%s costs, %f, ms***\n", info, result);
	return result;
}


/*double endTimer(char *info, unsigned *timer)
{
	cudaThreadSynchronize();
	CUT_SAFE_CALL( cutStopTimer( *timer));
	double result=cutGetTimerValue(*timer);
	printf("***%s costs, %f, ms***\n", info, result);
    CUT_SAFE_CALL( cutDeleteTimer( *timer));
	return result;
}*/
/*#define container_of(ptr, type, member) ({                      \
       const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
		*/


void memset_int(cl_mem d_R, int rLen, int value, int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("memset_int_kernel",Kernel);

    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&rLen);
    ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_int), (void*)&value);   
    ////printf("clSetKernelArg 0 - 2...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen, 16, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}