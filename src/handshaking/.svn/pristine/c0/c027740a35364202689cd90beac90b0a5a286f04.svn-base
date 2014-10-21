#include "common.h"
#include "testFilter.h"
#include "KernelScheduler.h"
#include "testScan.h"
#include "Helper.h"
#include "PrimitiveCommon.h"
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

void filterImpl_map_int(cl_mem d_Rin, int beginPos, int rLen, 
					cl_mem d_mark, int smallKey, int largeKey, cl_mem d_temp,
					int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;

	cl_getKernel("filterImpl_map_kernel",Kernel);

    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_Rin);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&beginPos);
	ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_int), (void*)&rLen);
	ciErr1 = clSetKernelArg((*Kernel), 3, sizeof(cl_mem), (void*)&d_mark);	
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&smallKey);
	ciErr1 |= clSetKernelArg((*Kernel), 5, sizeof(cl_int), (void*)&largeKey);
    ciErr1 |= clSetKernelArg((*Kernel), 6, sizeof(cl_mem), (void*)&d_temp); 
       if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	//printf("clSetKernelArg 0 - 6...\n\n"); 
	kernel_enqueue(rLen,20, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);

}
void filterImpl_outSize_int(cl_mem d_outSize,cl_mem d_mark,cl_mem d_markOutput,int rLen,int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU )
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("filterImpl_outSize_kernel",Kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_outSize);	
	ciErr1 = clSetKernelArg((*Kernel), 1, sizeof(cl_mem), (void*)&d_mark);	
	ciErr1 = clSetKernelArg((*Kernel), 2, sizeof(cl_mem), (void*)&d_markOutput);
	ciErr1 = clSetKernelArg((*Kernel), 3, sizeof(cl_mem), (void*)&rLen);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen, 21, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);


}
void filterImpl_write_int( cl_mem d_Rout, cl_mem d_Rin, cl_mem d_mark, 
						cl_mem d_markOutput, int beginPos, int rLen,
						int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel("filterImpl_write_kernel",Kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_Rout);	
	ciErr1 = clSetKernelArg((*Kernel), 1, sizeof(cl_mem), (void*)&d_Rin);	
	ciErr1 = clSetKernelArg((*Kernel), 2, sizeof(cl_mem), (void*)&d_mark);
	ciErr1 = clSetKernelArg((*Kernel), 3, sizeof(cl_mem), (void*)&d_markOutput);
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&beginPos);
	ciErr1 |= clSetKernelArg((*Kernel), 5, sizeof(cl_int), (void*)&rLen);
    //printf("clSetKernelArg 0 - 5...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen, 23, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

void filterImpl( cl_mem d_Rin, int beginPos, int rLen, cl_mem* d_Rout, int* outSize, 
				int numThread, int numBlock, int smallKey, int largeKey,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_mem d_mark;
	CL_MALLOC( &d_mark, sizeof(int)*rLen ) ;

	cl_mem d_markOutput;
	CL_MALLOC( &d_markOutput, sizeof(int)*rLen ) ;

	cl_mem d_temp;
	CL_MALLOC( &d_temp, sizeof(int)*rLen ) ;

	filterImpl_map_int( d_Rin, beginPos, rLen, d_mark, smallKey, largeKey, d_temp, numThread, numBlock,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]);
	//prefex sum
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(rLen,SP);
	scanImpl( d_mark, rLen, d_markOutput,index,eventList,Kernel,Flag_CPU_GPU,burden,SP,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	closeScan(SP);

	//get the outSize
	cl_mem d_outSize;
	CL_MALLOC(&d_outSize, sizeof(int)) ;
	filterImpl_outSize_int(d_outSize, d_mark, d_markOutput, rLen,1,1,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	//bufferchecking(d_outSize,sizeof(int));
	cl_readbuffer(outSize,d_outSize, sizeof(int),index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);

	//write the reduced result
	CL_MALLOC( d_Rout, sizeof(Record)*(*outSize) );
	filterImpl_write_int( *d_Rout, d_Rin, d_mark, d_markOutput, beginPos, rLen,numThread, numBlock,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU );
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	CL_FREE(d_mark);
	CL_FREE(d_markOutput);
	CL_FREE( d_temp );
}

void testFilterImpl( int rLen, int numThreadPB, int numBlock)//->corresponding to selection
{
	 int _CPU_GPU=0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	int beginPos = 0;
	int memSize = sizeof(Record)*rLen;
	
	void *Rin;
	HOST_MALLOC(Rin, memSize);
	generateRand( (Record *)Rin, 100, rLen, 0 );
	Record* Rout;

	int smallKey = rand()%100;
	int largeKey = smallKey;

	int* outSize = (int*)malloc( sizeof(int) );
	CL_RangeSelection((Record*) Rin,  rLen, smallKey, largeKey, &Rout, 
		numThreadPB,numBlock , _CPU_GPU);
	printf("CL_RangeSelectionFinish\n");
	CL_PointSelection((Record*) Rin,  rLen, smallKey, &Rout, 
		numThreadPB,numBlock, _CPU_GPU);
	printf("CL_PointSelectionFinish\n");

}