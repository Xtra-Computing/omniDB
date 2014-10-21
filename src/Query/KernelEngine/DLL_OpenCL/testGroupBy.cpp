#include "common.h"
#include "PrimitiveCommon.h"
#include "testGroupBy.h"
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
void groupByImpl_int(cl_mem d_R, int rLen,cl_mem d_groupLabel, int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("scanGroupLabel_kernel",kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&rLen);
    ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_groupLabel);
   
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	    /* Enqueue a kernel run call.*/
	kernel_enqueue(rLen,9,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
void groupByImpl_outSize_int(cl_mem d_outSize,cl_mem d_mark,cl_mem d_markOutput, int rLen, int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{ 
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("groupByImpl_outSize_kernel",kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_outSize);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_mem), (void*)&d_mark);
    ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_markOutput);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_int), (void*)&rLen);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	kernel_enqueue(rLen,2,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);

}
void groupByImpl_write_int(cl_mem d_startPos,cl_mem d_groupLabel,cl_mem d_writePos, int rLen, int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{ 
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("groupByImpl_write_kernel",kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_startPos);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_mem), (void*)&d_groupLabel);
    ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_writePos);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_int), (void*)&rLen);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	kernel_enqueue(rLen,8,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);

}
int groupByImpl(cl_mem d_Rin, int rLen, cl_mem d_Rout, cl_mem* d_startPos, int numThread, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_mem d_groupLabel=NULL;
	cl_mem d_writePos=NULL;
	cl_mem d_numGroup=NULL;
	int numGroup = 0;
	int memSize=sizeof(Record)*rLen;	
	//sort
	cl_copyBuffer(d_Rout,d_Rin,memSize,index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
	radixSortImpl(d_Rout, rLen,32, numThread, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU );
	CL_MALLOC(&d_groupLabel, sizeof(int)*rLen );
	groupByImpl_int(d_Rout, rLen, d_groupLabel,numThread, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	CL_MALLOC( &d_writePos, sizeof(int)*rLen );
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(rLen,SP);
	scanImpl( d_groupLabel, rLen, d_writePos,index,eventList,kernel,Flag_CPU_GPU,burden,SP,_CPU_GPU );
	CL_MALLOC( &d_numGroup, sizeof(int));
	groupByImpl_outSize_int( d_numGroup, d_groupLabel, d_writePos, rLen,1, 1,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 

	int test;
	cl_readbuffer(&test,d_numGroup,sizeof(int),0);



	CL_MALLOC(d_startPos, sizeof(int)*numGroup );
	groupByImpl_write_int((*d_startPos), d_groupLabel, d_writePos, rLen,numThread, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	closeScan(SP);
	CL_FREE(d_groupLabel);
	CL_FREE( d_writePos);
	CL_FREE(d_numGroup );
	return numGroup;
}
void testGroupByImpl( int rLen, int numThread, int numBlock)
{
	int _CPU_GPU=0;
	int memSize = sizeof(Record)*rLen;

	void* h_Rin;
	HOST_MALLOC(h_Rin, memSize );
	void* h_Rout;
	HOST_MALLOC(h_Rout, memSize );
 	generateRand((Record *)h_Rin, 64, rLen, 0 );
	int* h_startPos;

	int numGroup = 0;
	//group by
	numGroup=CL_GroupBy((Record *) h_Rin, rLen, (Record*) h_Rout, &h_startPos, numThread, numBlock,_CPU_GPU);
	//copy back
	validateGroupBy( (Record*)h_Rin, rLen, (Record*)h_Rout, h_startPos, numGroup );	
	HOST_FREE(h_startPos);
	HOST_FREE( h_Rin );
	HOST_FREE( h_Rout );
}
