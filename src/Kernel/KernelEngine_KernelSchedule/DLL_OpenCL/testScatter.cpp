#include "common.h"
#include "testGroupBy.h"
#include "PrimitiveCommon.h"
#include "Helper.h"
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

void optScatter_int(cl_mem d_R, int rLen, cl_mem d_loc, 
				int from, int to,cl_mem d_S,
					int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("optScatter_kernel",kernel);
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&rLen);
	ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_loc);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_int), (void*)&from);
	ciErr1 |= clSetKernelArg((*kernel), 4, sizeof(cl_int), (void*)&to);
	ciErr1 |= clSetKernelArg((*kernel), 5, sizeof(cl_mem), (void*)&d_S);	
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen, 24,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
void optGather_int(cl_mem d_R, int rLen, cl_mem d_loc, 
				int from, int to,cl_mem d_S, int sLen,
					int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;

	cl_getKernel("optGather_kernel",Kernel);

    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&rLen);
	ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_mem), (void*)&d_loc);
	ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_int), (void*)&from);
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&to);
	ciErr1 = clSetKernelArg((*Kernel), 5, sizeof(cl_mem), (void*)&d_S);	
	ciErr1 |= clSetKernelArg((*Kernel), 6, sizeof(cl_int), (void*)&sLen);

    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	//int kid,cl_uint work_dim, 
	kernel_enqueue(sLen,25,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);

}

void scatterImpl(cl_mem d_R, int rLen, cl_mem d_loc, cl_mem d_S, 
				 int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int numRun=8;
	if(rLen<256*1024)
		numRun=1;
	else
		if(rLen<1024*1024)
			numRun=2;
		else
			if(rLen<8192*1024)
				numRun=4;
	int runSize=rLen/numRun;	
	if(rLen%numRun!=0)
		runSize+=1;
	int from, to;

	for(int i=0;i<numRun;i++)
	{
		from=i*runSize;
		to=(i+1)*runSize;
		optScatter_int(d_R, rLen, d_loc,from, to, d_S, numThreadPB, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	}
}

void scatterImpl_forPart(cl_mem d_R, int rLen, int numPart, cl_mem d_loc, cl_mem d_S,
						 int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int numRun=8;
	if(numPart<=8)
		numRun=1;
	else if(numPart<=16)
			numRun=2;
	else if(numPart<=32)
			numRun=4;
	else
		numRun=8;
	int runSize=rLen/numRun;
	if(rLen%numRun!=0)
		runSize+=1;
	//printf("run, %d\n", numRun);
	int from, to;
	for(int i=0;i<numRun;i++)
	{
		from=i*runSize;
		to=(i+1)*runSize;
		optScatter_int(d_R, rLen, d_loc,from, to, d_S, numThreadPB, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	}
}


void gatherImpl(cl_mem d_R, int rLen, cl_mem d_loc, cl_mem d_S, int sLen, 
				int numThreadPB , int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int numRun=8;
	if(sLen<256*1024)
		numRun=1;
	else
		if(sLen<1024*1024)
			numRun=2;
		else
			if(sLen<8192*1024)
				numRun=4;
	int runSize=rLen/numRun;	
	if(rLen%numRun!=0)
		runSize+=1;
	int from, to;

	for(int i=0;i<numRun;i++)
	{
		from=i*runSize;
		to=(i+1)*runSize;
		optGather_int(d_R, rLen, d_loc,from, to, d_S,sLen, numThreadPB,numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
		
	}
}
void testScatterImpl(int rLen, int numThreadPB, int numBlock)
{
	int _CPU_GPU=0;
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	int result=0;
	int memSize=sizeof(Record)*rLen;
	int outSize=sizeof(Record)*rLen;
	void *Rin;
	HOST_MALLOC(Rin, memSize);
	generateRand((Record*)Rin,TEST_MAX,rLen,0);
	void *loc;
	HOST_MALLOC(loc, sizeof(int)*rLen);
	generateRandInt((int*)loc, rLen,rLen,0);
	void* Rout;
	HOST_MALLOC(Rout, outSize);
	cl_mem d_Rin;
	CL_MALLOC(&d_Rin, memSize);
	cl_mem d_Rout;
	CL_MALLOC(&d_Rout, outSize);
	cl_mem d_loc;
	CL_MALLOC(&d_loc, sizeof(int)*rLen);

	cl_writebuffer(d_Rin, Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	cl_writebuffer(d_loc, loc, sizeof(int)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	//scatter
	scatterImpl(d_Rin,rLen,d_loc,d_Rout,numThreadPB, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	cl_readbuffer(Rout, d_Rout, outSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	HOST_FREE(Rin);
	HOST_FREE(Rout);
	HOST_FREE(loc);
	CL_FREE(d_Rin);
	CL_FREE(d_Rout);
	CL_FREE(d_loc);
}

void testGatherImpl(int rLen, int numThread , int numBlock)
{
	int _CPU_GPU=0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;


	int result=0;
	int memSize=sizeof(Record)*rLen;
	int outSize=sizeof(Record)*rLen;
	void *Rin;
	HOST_MALLOC(Rin, memSize);
	generateRand((Record*)Rin,TEST_MAX,rLen,0);
	void *loc;
	HOST_MALLOC(loc, sizeof(int)*rLen);
	generateRandInt((int*)loc, rLen,rLen,0);
	void* Rout;
	HOST_MALLOC(Rout, outSize);
	cl_mem d_Rin;
	CL_MALLOC(&d_Rin, memSize);
	cl_mem d_Rout;
	CL_MALLOC(&d_Rout, outSize);
	cl_mem d_loc;
	CL_MALLOC(&d_loc, sizeof(int)*rLen);
	
	cl_writebuffer( d_Rin, Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	cl_writebuffer( d_loc, loc,  sizeof(int)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	
	//scatter
	gatherImpl(d_Rin,rLen,d_loc,d_Rout,rLen, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	
	//copy back
	cl_readbuffer(Rout, d_Rout, outSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	HOST_FREE(Rin);
	HOST_FREE(Rout);
	HOST_FREE(loc);
	CL_FREE(d_Rin);
	CL_FREE(d_Rout);
	CL_FREE(d_loc);
	//printf("testGatherImplFinish\n");
}