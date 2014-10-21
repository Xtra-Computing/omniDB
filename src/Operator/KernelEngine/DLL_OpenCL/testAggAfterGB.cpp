#include "common.h"
#include "testGroupBy.h"
#include "PrimitiveCommon.h"
#include "testScatter.h"
#include "Helper.h"
#include "OpenCL_DLL.h"
#include "KernelScheduler.h"
// OpenCL Vars---------0 for CPU, 1 for GPU
extern cl_context Context;        // OpenCL context
extern cl_program Program;           // OpenCL program
extern cl_command_queue CommandQueue[2];// OpenCL command que
extern cl_platform_id Platform[2];      // OpenCL platform
extern cl_device_id Device[2];          // OpenCL device
extern cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
extern cl_device_id allDevices[10];
#ifndef REDUCE_AFTER_GROUPBY
#define REDUCE_AFTER_GROUPBY
#define MAX_NUM_BLOCK (512)
void mapBeforeGather_int(cl_mem d_Rin, int rLen,cl_mem d_loc,cl_mem d_temp,int numBlock, int numThread,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThread;
	size_t globalWorkingSetSize=numThread*numBlock;
	cl_getKernel("mapBeforeGather_kernel",kernel);

    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_Rin);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&rLen);
    ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_loc);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_mem), (void*)&d_temp);
    //printf("clSetKernelArg 0 - 3...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	kernel_enqueue(rLen,3, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);	
}
void gatherBeforeAgg(cl_mem d_Rin, int rLen, cl_mem d_Ragg, cl_mem d_S, int numThread, int numBlock ,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_mem d_loc;
	CL_MALLOC(&d_loc, sizeof(int)*rLen ) ;
	clWaitForEvents(1,&eventList[(*index-1)%2]);
	cl_mem d_temp;
	CL_MALLOC(&d_temp, sizeof(int)*rLen ) ;

	mapBeforeGather_int( d_Rin, rLen, d_loc, d_temp,numBlock, numThread,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	gatherImpl( d_Ragg, rLen, d_loc, d_S,rLen, numThread, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	CL_FREE(d_temp);
	clWaitForEvents(1,&eventList[(*index-1)%2]);
	CL_FREE(d_loc);
}
void parallelAggregate_init(cl_mem d_S,cl_mem d_startPos,cl_mem d_aggResults,
							int OPERATOR,int blockOffset, int numGroups, int numBlock, int numThread,int sharedMemSize, int rLen,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{ 
	size_t numThreadsPerBlock_x=numThread;
	size_t globalWorkingSetSize=numThread*numBlock;
	cl_getKernel("parallelAggregate_kernel", kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_S);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_mem), (void*)&d_startPos);
    ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_aggResults);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_int), (void*)&OPERATOR);
	ciErr1 |= clSetKernelArg((*kernel), 4, sizeof(cl_int), (void*)&blockOffset);
	ciErr1 |= clSetKernelArg((*kernel), 5, sizeof(cl_int), (void*)&numGroups);
	ciErr1 |= clSetKernelArg((*kernel), 6, sizeof(cl_int), (void*)&rLen);
	ciErr1 |= clSetKernelArg((*kernel), 7, sharedMemSize * sizeof(cl_int), NULL);
    //printf("clSetKernelArg 0 - 6...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	kernel_enqueue(rLen,4, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);

}
void aggAfterGroupByImpl(cl_mem d_Rin, int rLen, cl_mem d_startPos, int numGroups, cl_mem d_Ragg, cl_mem d_aggResults, int OPERATOR,  int numThread,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	//gather=============================================================
	cl_mem d_S;
	CL_MALLOC( &d_S, sizeof(Record)*rLen ) ;
	gatherBeforeAgg( d_Rin, rLen, d_Ragg, d_S, 512, 256,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]);
	//parallel aggregation after gather======================================
	//numThread = 1;
	int numChunk = ceil(((float)numGroups)/MAX_NUM_BLOCK);
	int numBlock;
	int blockOffset;

	int sharedMemSize = sizeof(int)*numThread;

	for( int chunkIdx = 0; chunkIdx < numChunk; chunkIdx++ )
	{
		blockOffset = chunkIdx*MAX_NUM_BLOCK;
		if( chunkIdx == ( numChunk - 1 ) )
		{
			numBlock = numGroups - chunkIdx*MAX_NUM_BLOCK;
		}
		else
		{
			numBlock = MAX_NUM_BLOCK;
		}
		parallelAggregate_init(d_S, d_startPos, d_aggResults, OPERATOR, blockOffset, numGroups,numBlock, numThread, sharedMemSize, rLen,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
		clWaitForEvents(1,&eventList[(*index-1)%2]);
	}
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	CL_FREE( d_S );
}
#endif


void testAggAfterGroupByImpl( int rLen, int OPERATOR, int numThread, int numBlock)
{
	int _CPU_GPU=0;
	int memSize = sizeof(Record)*rLen;
	void* h_Rin;
	void* h_Rout;
	void* h_Sin;
	int* h_startPos;
	HOST_MALLOC( h_Rin, memSize );
	HOST_MALLOC( h_Rout, memSize );
	HOST_MALLOC( h_Sin, memSize );
	generateRand((Record *) h_Rin, 50, rLen, 0 );
	generateRand((Record *) h_Sin, TEST_MAX, rLen, 0 );  
	int numGroup = 0;
	numGroup = CL_GroupBy((Record *)h_Rin, rLen,(Record *) h_Rout, &h_startPos, numThread, numBlock,_CPU_GPU);
	validateGroupBy((Record*) h_Rin, rLen, (Record*)h_Rout,h_startPos, numGroup );
	void* h_aggResults;
	HOST_MALLOC(h_aggResults, sizeof(int)*numGroup );
	switch(OPERATOR){
	case REDUCE_MAX:
		{
			CL_agg_max_afterGroupBy((Record *)h_Rout,rLen,h_startPos,numGroup,(Record *)h_Sin,(int *)h_aggResults,numThread,_CPU_GPU);
			break;
		}
	case REDUCE_MIN:
		{
			CL_agg_min_afterGroupBy((Record *)h_Rout,rLen,h_startPos,numGroup,(Record *)h_Sin,(int *)h_aggResults,numThread,_CPU_GPU);
			break;
		}
	case REDUCE_SUM:
		{
			CL_agg_sum_afterGroupBy((Record *)h_Rout,rLen,h_startPos,numGroup,(Record *)h_Sin,(int *)h_aggResults,numThread,_CPU_GPU);
			break;
		}
	case REDUCE_AVERAGE:
		{
			CL_agg_avg_afterGroupBy((Record *)h_Rout,rLen,h_startPos,numGroup,(Record *)h_Sin,(int *)h_aggResults,numThread,_CPU_GPU);
			break;
		}
	}
	validateAggAfterGroupBy((Record*) h_Rin, rLen, (int*)h_startPos, numGroup,(Record*) h_Sin, (int *)h_aggResults, OPERATOR);
}