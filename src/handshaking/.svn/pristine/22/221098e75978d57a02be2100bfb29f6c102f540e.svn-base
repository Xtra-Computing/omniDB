#include "common.h"
#include "PrimitiveCommon.h"
#include "testSort.h"
#include "Helper.h"
#include "KernelScheduler.h"
#include "OpenCL_DLL.h"
// OpenCL Vars---------0 for CPU, 1 for GPU
extern cl_context Context;        // OpenCL context
extern cl_program Program;           // OpenCL program
extern cl_command_queue CommandQueue[2];// OpenCL command que
extern cl_platform_id Platform[2];      // OpenCL platform
extern cl_device_id Device[2];          // OpenCL device
extern cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
extern cl_device_id allDevices[10];

void radixSort_int(cl_mem d_R, int rLen, int numThreadPB, int numBlock, int sortAscending, int numStages,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_int err;

	cl_int stage;
	cl_int passOfStage;

	size_t global_work_size[1] = { rLen / 2};         //number of global work-items, specific to this program
	size_t local_work_size[1] = {numThreadPB};             //work-group size

	cl_getKernel("BitonicSort_kernel",kernel);

	err  = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void *) &d_R);
	err |= clSetKernelArg((*kernel), 3, sizeof(cl_uint), (void *) &rLen);
	err |= clSetKernelArg((*kernel), 4, sizeof(cl_uint), (void *) &sortAscending);
	if (err != CL_SUCCESS)
	{
		printf("ERROR: Failed to set input kernel arguments\n");
		cl_clean(EXIT_FAILURE);
	}


	for (stage = 0; stage < numStages; ++stage)
	{
		err = clSetKernelArg((*kernel),1,sizeof(cl_uint),(void*)&stage);
		for (passOfStage = 0; passOfStage < stage + 1; ++passOfStage)
		{
			err = clSetKernelArg((*kernel),2,sizeof(cl_uint),(void*)&passOfStage);
			kernel_enqueue(rLen, 31,
				1, global_work_size, local_work_size,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
			clWaitForEvents(1,&eventList[(*index-1)%2]);
		}
		clWaitForEvents(1,&eventList[(*index-1)%2]);
	}
}

void radixSortImpl(cl_mem d_R, int rLen, int meaningless, int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_int  sortAscending = 1; //1: ascending order, 0: descending order
	cl_uint temp;

	cl_int numStages = 0; //stages in total
	for (temp = rLen; temp > 1; temp >>= 1)
		++numStages;

	radixSort_int(d_R,rLen,numThreadPB,numBlock,sortAscending,numStages,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}


void testSortImpl(int rLen, int numThreadPB, int numBlock)
{
	int _CPU_GPU=0;
	int memSize = sizeof(Record) * rLen;
	void* h_Rin;
	HOST_MALLOC(h_Rin,memSize);
	generateRand((Record*) h_Rin, 200, rLen, 0);
	void * h_Rout;
	HOST_MALLOC(h_Rout,memSize);

	CL_RadixSort((Record*) h_Rin,  rLen,(Record*) h_Rout, numThreadPB,  numBlock,_CPU_GPU);
	validateSort((Record*) h_Rout, rLen);

	HOST_FREE(h_Rin);
	HOST_FREE(h_Rout);
	printf("Sort test finished\n");
}


