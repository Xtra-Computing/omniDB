#include "common.h"
#include "Helper.h"
#include "testSort.h"
#include "testScan.h"
#include "PrimitiveCommon.h"
#include "CSSTree.h"
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

//gpuNLJ_int(d_temp, d_R, d_S, sStart, rLen, sLen, d_n, grid_NLJ, threads_NLJ, CPU_GPU);
void gpuNLJ_int(cl_mem d_temp, cl_mem d_R, cl_mem d_S, int sStart, int rLen, int sLen, cl_mem d_n,
			size_t* grid_NLJ, size_t* threads_NLJ,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=threads_NLJ[0];
	size_t globalWorkingSetSize=numThreadsPerBlock_x*grid_NLJ[0]*grid_NLJ[1];
	cl_getKernel("gpuNLJ_kernel",Kernel);

    // Set the Argument values
	//gpuNLJ_int_kernel(cl_mem d_temp, Record *d_R, Record *d_S, int sStart, int rLen, int sLen, int *d_n) 
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_temp);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_mem), (void*)&d_R);
	ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_mem), (void*)&d_S);
	ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_int), (void*)&sStart);
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&rLen);
	ciErr1 = clSetKernelArg((*Kernel), 5, sizeof(cl_int), (void*)&sLen);	
	ciErr1 = clSetKernelArg((*Kernel), 6, sizeof(cl_mem), (void*)&d_n);	
    ////printf("clSetKernelArg 0 - 6...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen, 38, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
	
void write_int(cl_mem d_R, cl_mem d_S, int sStart, int rLen, int sLen, cl_mem d_sum, cl_mem d_outBuf,
			size_t* grid_NLJ, size_t* threads_NLJ,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=threads_NLJ[0];
	size_t globalWorkingSetSize=numThreadsPerBlock_x*grid_NLJ[0]*grid_NLJ[1];
	cl_getKernel("nlj_write_kernel",Kernel);

    // Set the Argument values 
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_R);
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_mem), (void*)&d_S);
	ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_int), (void*)&sStart);
	ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_int), (void*)&rLen);
	ciErr1 = clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&sLen);	
	ciErr1 = clSetKernelArg((*Kernel), 5, sizeof(cl_mem), (void*)&d_sum);	
	ciErr1 = clSetKernelArg((*Kernel), 6, sizeof(cl_mem), (void*)&d_outBuf);
    ////printf("clSetKernelArg 0 - 6...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen,39, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);

}

int NINJImpl(cl_mem d_R, int rLen, cl_mem d_S, int sLen, cl_mem* d_Rout,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int numThreadPerBlock=NLJ_NUM_THREADS_PER_BLOCK;
	int numBlock_X=4*NLJ_SHARED_MEM/(sizeof(Record)*NLJ_S_BLOCK_SIZE);
	/*if(numThreadPerBlock*numBlock_X>sLen)
	{
		numBlock_X=sLen/numThreadPerBlock;
		if(sLen%numThreadPerBlock!=0)
			numBlock_X++;
	}*/
	int numBlock_Y=1;
	if(numBlock_X>NLJ_MAX_NUM_BLOCK_PER_DIM)
	{
		numBlock_Y=numBlock_X/NLJ_MAX_NUM_BLOCK_PER_DIM;
		if(numBlock_X%NLJ_MAX_NUM_BLOCK_PER_DIM!=0)
			numBlock_Y++;
		numBlock_X=NLJ_MAX_NUM_BLOCK_PER_DIM;
	}
	int numBlock=numBlock_X*numBlock_Y;
	int gridSize=numBlock*NLJ_S_BLOCK_SIZE;
	int numGrid=sLen/gridSize;
	if(sLen%gridSize!=0)
		numGrid++;
	//printf("numBlock, %d, gridSize, %d, numGrid, %d\n",numBlock, gridSize, numGrid);
	size_t  threads_NLJ[2]={ numThreadPerBlock, 1};
	size_t  grid_NLJ[2]={ numBlock_X, numBlock_Y};
	int numResults=0;
	//the number of results for threads
	
	int originalResultBufSize=grid_NLJ[0]*grid_NLJ[1]*numThreadPerBlock;
	int resultBuf=get2N(originalResultBufSize);
	cl_mem d_n;
	CL_MALLOC(&d_n, sizeof(int)*resultBuf );
	//the prefix sum for d_n
	cl_mem d_sum;//the prefix sum for d_n[1,...,n]
	CL_MALLOC(&d_sum, sizeof(int)*resultBuf );
	
	
	int h_n=0 ;	
	int h_sum =0;	
	int outSize=rLen;
	//printf("output size: %d\n",outSize);
	//one result buffer for one grid.
	cl_mem* h_outBuf=(cl_mem*)malloc(sizeof(cl_mem)*numGrid);//safe	
	int *h_numResultCurRun=(int*)malloc(sizeof(int)*numGrid); //safe
	
	for(int sg=0;sg<numGrid;sg++)	
	{
		h_outBuf[sg]=NULL;
		h_numResultCurRun[sg]=0;
	}
	cl_mem d_outBuf;//the prefix sum for d_n[1,...,n]
	CL_MALLOC( &d_outBuf, sizeof(Record)*outSize);

	int sStart=0;
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(resultBuf,SP);	
	cl_mem d_temp;
	CL_MALLOC( &d_temp, sizeof(int)*rLen );
	for(int sg=0;sg<numGrid;sg++)	
	{
		sStart=sg*gridSize;
		//printf("Start=%d, ", sStart);
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
		gpuNLJ_int(d_temp, d_R, d_S, sStart, rLen, sLen, d_n, grid_NLJ, threads_NLJ, index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
		//prescanArray(d_sum, d_n,  16);
		//prefix sum to check out the result position.
		//gpuPrint(d_n, resultBuf, "d_n");
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
		scanImpl( d_n, resultBuf, d_sum,index,eventList,Kernel,Flag_CPU_GPU,burden,SP,_CPU_GPU);
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
		cl_readbuffer((void*)&h_n, d_n, (resultBuf-1)*sizeof(int), sizeof(int),index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
		cl_readbuffer((void*)&h_sum, d_sum, (resultBuf-1)*sizeof(int), sizeof(int),index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
		
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
		h_numResultCurRun[sg]=h_n+h_sum;	
		numResults+=h_numResultCurRun[sg];
		printf("numResults=%d\n",numResults);
		if(h_numResultCurRun[sg]>0)
		{	//enlarge the output size.
			outSize=h_numResultCurRun[sg];
			CL_MALLOC(&d_outBuf, sizeof(Record)*outSize );
			write_int(d_R, d_S, sStart, rLen, sLen,d_sum, d_outBuf, grid_NLJ, threads_NLJ,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
			clWaitForEvents(1,&eventList[(*index-1)%2]); 
			h_outBuf[sg]=d_outBuf;			
		}
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
	}
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	closeScan(SP);
	//dump the final results to Rout;
	if(numResults!=0){
	CL_MALLOC(d_Rout,sizeof(Record)*numResults);
	}
	else{

		printf("!!!!numResults is 0!!!\n\n\n");
		CL_MALLOC(d_Rout,sizeof(Record)*rLen);
	}
	int rstart=0;
	for(int sg=0;sg<numGrid;sg++)
	{
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
		cl_copyBuffer(*d_Rout, rstart,h_outBuf[sg],sizeof(Record)*h_numResultCurRun[sg],index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
		rstart+=h_numResultCurRun[sg];
	}  
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	for(int sg=0;sg<numGrid;sg++)
	{
		if(h_outBuf[sg]!=NULL)
			CL_FREE(h_outBuf[sg]);
	}
	HOST_FREE(h_outBuf);
	CL_FREE(d_n);
	CL_FREE(d_sum);
	CL_FREE( d_temp );
	return numResults;
}
void testNINLJ(int rLen, int sLen)
{
	int _CPU_GPU=0;
	int memSizeR=sizeof(Record)*rLen;
	int memSizeS=sizeof(Record)*sLen;
	void* h_R;
	HOST_MALLOC(h_R, memSizeR);
	generateRand((Record*)h_R,TEST_MAX,rLen,0);
	void *h_S;
	HOST_MALLOC(h_S, memSizeS);
	generateRand((Record*)h_S, TEST_MAX,sLen,1);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;

	
	cl_mem d_Rout;
	cl_mem d_R;
	CL_MALLOC(& d_R, memSizeR);
	cl_writebuffer( d_R, h_R, memSizeR,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	cl_mem d_S;
	CL_MALLOC(& d_S, memSizeS );
	cl_writebuffer( d_S, h_S, memSizeR,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);

	result=NINJImpl(d_R,rLen,d_S,sLen,&d_Rout,  &index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

 	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	HOST_FREE(h_R);
	HOST_FREE(h_S);
	CL_FREE(d_R);
	CL_FREE(d_S);
	CL_FREE(d_Rout);
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("NINLJFinish\n");
}