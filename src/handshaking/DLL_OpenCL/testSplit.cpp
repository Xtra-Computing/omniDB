#include "common.h"
#include "PrimitiveCommon.h"
#include "testSplit.h"
#include "testScan.h"
#include "testScatter.h"
#include "Helper.h"
#include "KernelScheduler.h"
// OpenCL Vars---------0 for CPU, 1 for GPU
extern cl_context Context;        // OpenCL context
extern cl_program Program;           // OpenCL program
extern cl_command_queue CommandQueue[2];// OpenCL command que
extern cl_platform_id Platform[2];      // OpenCL platform
extern cl_device_id Device[2];          // OpenCL device
extern cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
extern cl_device_id allDevices[10];
//partition_kernel(Record *d_R, int rLen, int numPart, int *d_output1, int *d_output2)
void Partition_int(cl_mem d_R, int rLen, int numPart,
				cl_mem d_output1, cl_mem d_output2,
					int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("partition_kernel",kernel);
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&rLen);
	ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_int), (void*)&numPart);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_mem), (void*)&d_output1);
	ciErr1 |= clSetKernelArg((*kernel), 4, sizeof(cl_mem), (void*)&d_output2);
    //printf("clSetKernelArg 0 - 4...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen,26,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
void Map_int(cl_mem d_R, int rLen, int numPart,
				cl_mem d_output1, cl_mem d_output2,
					int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("mapPart_kernel",kernel);

    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&rLen);
	ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_int), (void*)&numPart);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_mem), (void*)&d_output1);
	ciErr1 |= clSetKernelArg((*kernel), 4, sizeof(cl_mem), (void*)&d_output2);
    //printf("clSetKernelArg 0 - 4...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen,27,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

//countHist_int(d_pidArray, numThread, rLen, numPart, d_Hist, numThreadsPerBlock_x, numBlock_x, sharedMemSize)
void countHist_int(cl_mem d_pidArray, int rLen, int numPart, cl_mem d_Hist, 
			   int numThreadPB, int numBlock, int sharedMemSize,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("countHist_kernel",kernel);

    // Set the Argument values
	//(__global Record *d_R, int rLen,__global int *loc, int from, int to, __global Record *d_S)
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_pidArray);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&rLen);
	ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_int), (void*)&numPart);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_mem), (void*)&d_Hist);
	ciErr1 |= clSetKernelArg((*kernel), 4, sharedMemSize, NULL);
    //printf("clSetKernelArg 0 - 4...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(numPart,28,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}


//writeHist_int(d_pidArray, rLen, numPart, d_psSum, d_loc, numThreadsPerBlock_x, numBlock_x, sharedMemSize);
void writeHist_int(cl_mem d_pidArray, int rLen, int numPart, cl_mem d_psSum, cl_mem d_loc,
			   int numThreadPB, int numBlock, int sharedMemSize,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("writeHist_kernel",Kernel);

    // Set the Argument values
	//(__global Record *d_R, int rLen,__global int *loc, int from, int to, __global Record *d_S)
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_pidArray);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&rLen);
	ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_int), (void*)&numPart);
	ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_mem), (void*)&d_psSum);
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_mem), (void*)&d_loc);
	ciErr1 |= clSetKernelArg((*Kernel), 5, sharedMemSize, NULL);
    //printf("clSetKernelArg 0 - 4...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen,29,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	
}
void getBound_int(cl_mem d_psSum, int interval, int rLen, 
	int numPart, cl_mem d_bound,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	if(d_bound!=NULL)
	{
		size_t numThreadsPerBlock_x=32;
		size_t numBlock_x=ceil((double)numPart/(double)numThreadsPerBlock_x);
		size_t globalWorkingSetSize=numThreadsPerBlock_x*numBlock_x;
		cl_getKernel("getBound_kernel",Kernel);
		cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_psSum);	
		ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&interval);
		ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_int), (void*)&numPart);
		ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_mem), (void*)&d_bound);		
		
		if (ciErr1 != CL_SUCCESS)
		{
			printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
			cl_clean(EXIT_FAILURE);
		}
	kernel_enqueue(1,30,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	}
}



void mapPart(cl_mem d_R, int rLen, int numPart, cl_mem d_S, int SPLIT_TYPE,
			 int numThreadPB, int numBlock ,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_mem d_extra;
	CL_MALLOC(&d_extra,rLen*sizeof(int));
	
	if( SPLIT_TYPE == SPLIT )
	{
		Map_int(d_R, rLen, numPart, d_extra, d_S, numThreadPB, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	}
	else if( SPLIT_TYPE == PARTITION )
	{
		Partition_int( d_R, rLen, numPart, d_extra, d_S, numThreadPB, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	}
	else
	{
		printf( "split type error! \n" ); 
	}
	clWaitForEvents(1,&eventList[(*index-1)%2]);
	CL_FREE(d_extra);
}


void splitImpl(cl_mem d_R, int rLen, int numPart, cl_mem d_S, cl_mem d_bound,
			   int numThreadPB, int numBlock, int SPLIT_TYPE,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	//map->pid
	cl_mem d_pidArray;
	CL_MALLOC(&d_pidArray, sizeof(int)*rLen);
	mapPart(d_R, rLen, numPart, d_pidArray, SPLIT,numThreadPB, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	int numThreadsPerBlock_x=0;
	if(numThreadPB==-1)
		numThreadsPerBlock_x=1<<(log2((int)(SHARED_MEMORY_PER_PROCESSOR/(numPart*sizeof(int)))));
	else
		numThreadsPerBlock_x=numThreadPB;
	if(numThreadsPerBlock_x>256)
		numThreadsPerBlock_x=256;
	int sharedMemSize=numThreadsPerBlock_x*numPart*sizeof(int);

	//assert(numThreadsPerBlock_x>=16);
	int numThreadsPerBlock_y=1;
	int numBlock_x;
	int numThreadBlock=-1;
	if(numThreadBlock==-1)
		numBlock_x=512;
	else
		numBlock_x=numThreadBlock;
	//printf("numThreadsPerBlock_x, %d,sharedMemSize, %d,  numBlock_x, %d\n", numThreadsPerBlock_x, sharedMemSize,numBlock_x);
	int numBlock_y=1;
	int numThread=numBlock_x*numThreadsPerBlock_x;
	int numInPS=numThread*numPart;
	cl_mem d_Hist;
	CL_MALLOC(&d_Hist, sizeof(int)*numInPS);
	countHist_int(d_pidArray, rLen, numPart, d_Hist, numThreadsPerBlock_x, numBlock_x, sharedMemSize,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	//prefix sum
	cl_mem d_psSum;
	CL_MALLOC(&d_psSum, sizeof(int)*numInPS);
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(numInPS,SP);
	scanImpl(d_Hist, numInPS, d_psSum,index,eventList,kernel,Flag_CPU_GPU,burden,SP,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	closeScan(SP);
	CL_FREE(d_Hist);
	
	cl_mem d_loc;
	CL_MALLOC(&d_loc, sizeof(int)*rLen);
	writeHist_int(d_pidArray, rLen, numPart, d_psSum, d_loc, numThreadsPerBlock_x, numBlock_x, sharedMemSize,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);

	getBound_int(d_psSum, numThread, rLen, numPart,d_bound,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);

	//scatter	
	scatterImpl_forPart(d_R, rLen, numPart, d_loc, d_S, numThreadPB, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]);
	CL_FREE(d_pidArray);	
	CL_FREE(d_psSum);
}

void testSplitImpl(int rLen, int numPart, int numThreadPB , int numThreadBlock )
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
	void* Rin;
	HOST_MALLOC(Rin, memSize);
	void* h_bound;
	HOST_MALLOC(h_bound, numPart*sizeof(Record));
	generateRand((Record*)Rin,TEST_MAX,rLen,0);
	void *Rout;
	HOST_MALLOC(Rout, outSize);
	cl_mem d_Rin;
	CL_MALLOC(&d_Rin, memSize);
	cl_mem d_Rout;
	CL_MALLOC(&d_Rout, outSize);
	cl_mem d_bound;
	CL_MALLOC(&d_bound, numPart*sizeof(Record));
	
	//copy to
	cl_writebuffer(d_Rin, Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);

	splitImpl(d_Rin,rLen,numPart, d_Rout, d_bound,numThreadPB,numThreadBlock,SPLIT,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	//copy back
	cl_readbuffer(Rout, d_Rout, outSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	cl_readbuffer(h_bound, d_bound, sizeof(Record)*numPart,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);

	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	validateSplit((Record*)Rout, rLen, numPart);	
	HOST_FREE(Rin);
	HOST_FREE(Rout);
	HOST_FREE(h_bound);
	CL_FREE(d_Rin);
	CL_FREE(d_Rout);
	CL_FREE(d_bound);
}