#include "common.h"
#include "Helper.h"
#include "testSort.h"
#include "testScan.h"
#include "PrimitiveCommon.h"
#include "KernelScheduler.h"
#include "OpenCL_DLL.h"
#include "testJoin.h"
// OpenCL Vars---------0 for CPU, 1 for GPU
extern cl_context Context;        // OpenCL context
extern cl_program Program;           // OpenCL program
extern cl_command_queue CommandQueue[2];// OpenCL command que
extern cl_platform_id Platform[2];      // OpenCL platform
extern cl_device_id Device[2];          // OpenCL device
extern cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
extern cl_device_id allDevices[10];

void getQuantile(cl_mem d_R, int rLen, int interval, cl_mem d_output, int numQuantile,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int numThreadsPerBlock_x=interval;
	int numThreadsPerBlock_y=1;
	int numBlock_X=divRoundUp(rLen, interval);
	int numBlock_Y=1;
	if(numBlock_X>NLJ_MAX_NUM_BLOCK_PER_DIM)
	{
		numBlock_Y=numBlock_X/NLJ_MAX_NUM_BLOCK_PER_DIM;
		if(numBlock_X%NLJ_MAX_NUM_BLOCK_PER_DIM!=0)
			numBlock_Y++;
		numBlock_X=NLJ_MAX_NUM_BLOCK_PER_DIM;
	}
	size_t  thread[2]={numThreadsPerBlock_x, numThreadsPerBlock_y};
	size_t  grid[2]={ numBlock_X*numThreadsPerBlock_x* numBlock_Y*numThreadsPerBlock_y, 1 };
	//quanMap_kernel<<<grid,thread, interval*sizeof(Record)>>>(d_R, interval,rLen, d_output);
	cl_getKernel("quanMap_kernel",kernel);

    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&interval);
    ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_int), (void*)&rLen);
    ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_mem), (void*)&d_output);    
	ciErr1 |= clSetKernelArg((*kernel), 4, interval*sizeof(Record), NULL);    
   // printf("clSetKernelArg 0 - 4...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen, 44
		,1, grid, thread,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

void joinMBCount(cl_mem d_R, int rLen, cl_mem d_S, 
			int sLen, cl_mem d_quanLocS, int numQuan,
			cl_mem d_n, dim3 grid_NLJ, dim3 threads_NLJ,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=threads_NLJ.x;
	size_t globalWorkingSetSize=grid_NLJ.x*grid_NLJ.y*threads_NLJ.x;
	cl_getKernel("joinMBCount_kernel",kernel);

    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&rLen);
	ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_S);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_int), (void*)&sLen);
	ciErr1 |= clSetKernelArg((*kernel), 4, sizeof(cl_mem), (void*)&d_quanLocS);
	ciErr1 |= clSetKernelArg((*kernel), 5, sizeof(cl_int), (void*)&numQuan);	
	ciErr1 |= clSetKernelArg((*kernel), 6, sizeof(cl_mem), (void*)&d_n);	

    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }

		kernel_enqueue(rLen, 46
		,1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}


void joinMBWrite(cl_mem d_R, int rLen, cl_mem d_S, 
			int sLen, cl_mem d_quanLocS, int numQuan,
			cl_mem d_sum, cl_mem d_outBuf, dim3 grid_NLJ, dim3 threads_NLJ,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=threads_NLJ.x;
	size_t globalWorkingSetSize=grid_NLJ.x*grid_NLJ.y*threads_NLJ.x;
	cl_getKernel("joinMBWrite_kernel",kernel);

    // Set the Argument values
	//gpuNLJ_int_kernel(cl_mem d_temp, Record *d_R, Record *d_S, int sStart, int rLen, int sLen, int *d_n) 
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&rLen);
	ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_S);
	ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_int), (void*)&sLen);
	ciErr1 |= clSetKernelArg((*kernel), 4, sizeof(cl_mem), (void*)&d_quanLocS);
	ciErr1 |= clSetKernelArg((*kernel), 5, sizeof(cl_int), (void*)&numQuan);	
	ciErr1 |= clSetKernelArg((*kernel), 6, sizeof(cl_mem), (void*)&d_sum);	
	ciErr1 |= clSetKernelArg((*kernel), 7, sizeof(cl_mem), (void*)&d_outBuf);	

    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
		kernel_enqueue(rLen, 47
		,1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

int joinMatchingBlocks(cl_mem d_R, int rLen, cl_mem d_S, int sLen, 
					   cl_mem d_quanLocS, int numQuan, cl_mem * d_Rout,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int numResults=0;
	int numThreadPerBlock =SMJ_NUM_THREADS_PER_BLOCK;
	int numBlock_X=numQuan;
	int numBlock_Y=1;
	if(numBlock_X>NLJ_MAX_NUM_BLOCK_PER_DIM)
	{
		numBlock_Y=numBlock_X/NLJ_MAX_NUM_BLOCK_PER_DIM;
		if(numBlock_X%NLJ_MAX_NUM_BLOCK_PER_DIM!=0)
			numBlock_Y++;
		numBlock_X=NLJ_MAX_NUM_BLOCK_PER_DIM;
	}
	dim3  threads_NLJ( numThreadPerBlock, 1, 1);
	dim3  grid_NLJ( numBlock_X, numBlock_Y, 1);
	int resultBuf=grid_NLJ.x*grid_NLJ.y*numThreadPerBlock;
	cl_mem d_n;
	CL_MALLOC(&d_n, sizeof(int)*resultBuf );
	//the prefix sum for d_n
	cl_mem d_sum;//the prefix sum for d_n[1,...,n]
	CL_MALLOC(&d_sum, sizeof(int)*resultBuf );
	
	
	int h_n =0;	
	int h_sum =0;
	joinMBCount(d_R, rLen, d_S, sLen, 
				d_quanLocS, numQuan, d_n, grid_NLJ, threads_NLJ,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(resultBuf,SP);
	scanImpl(d_n, resultBuf, d_sum,index,eventList,kernel,Flag_CPU_GPU,burden,SP,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	closeScan(SP);
	cl_readbuffer((void*)&h_n, d_n, (resultBuf-1)*sizeof(int), sizeof(int),index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
	cl_readbuffer((void*)&h_sum, d_sum, (resultBuf-1)*sizeof(int), sizeof(int),index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	numResults=h_n+h_sum;
	cl_mem d_outBuf;
	if(numResults>0)
	{
		CL_MALLOC(&d_outBuf, sizeof(Record)*numResults );
		*d_Rout=d_outBuf;
			joinMBWrite(d_R, rLen, d_S, sLen, 
				d_quanLocS, numQuan,d_sum, d_outBuf, grid_NLJ, threads_NLJ,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	}
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	CL_FREE(d_n);
	CL_FREE(d_sum);
	return numResults;
}

int MJImpl( cl_mem d_Rin, int rLen, cl_mem d_Sin, int sLen, cl_mem* d_Joinout,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden, int _CPU_GPU)
{
	int numResult = 0;

	int interval=SMJ_NUM_THREADS_PER_BLOCK;
	int numQuanR=divRoundUp(rLen,interval);
	cl_mem d_quanKeyR;
	CL_MALLOC(&d_quanKeyR,numQuanR*sizeof(int)*2); 

	getQuantile(d_Rin, rLen, interval,d_quanKeyR,numQuanR,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);

	cl_mem d_quanLocS;
	CL_MALLOC(&d_quanLocS,numQuanR*sizeof(int)*2); 
	CUDA_CSSTree* tree;
	gpu_constructCSSTreeImpl(d_Rin, rLen, &tree,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	cuda_search_index_usingKeys(tree->data, tree->nDataNodes, tree->dir,
		tree->nDirNodes, d_quanKeyR, d_quanLocS, numQuanR*2,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);	
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	numResult=joinMatchingBlocks(d_Rin, rLen,d_Sin,sLen, 
		d_quanLocS, numQuanR, d_Joinout,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	CL_FREE(d_quanKeyR);
	CL_FREE(d_quanLocS);
	delete tree;
	return numResult;
}

//R is the smaller relation:)
int SMJImpl(cl_mem d_R, int rLen, cl_mem d_S, int sLen, cl_mem* d_Joinout,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int numResult=0;
	//radixSortImpl(d_R,rLen,d_Rout);
	//int numThreadPB=ctaSize;
	int numThreadPB=256;
	int numBlock=64;
	radixSortImpl(d_R,rLen,32,numThreadPB, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	radixSortImpl(d_S,sLen,32,numThreadPB, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	numResult = MJImpl( d_R, rLen, d_S, sLen, d_Joinout,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);

	return numResult;
}

void testSMJ(int rLen, int sLen)
{
	int _CPU_GPU=0;
	int result=0;
	int memSizeR=sizeof(Record)*rLen;
	int memSizeS=sizeof(Record)*sLen;
	void *h_R;
	HOST_MALLOC(h_R, memSizeR);
	generateRand((Record*)h_R, TEST_MAX,rLen,0);
	void *h_S;
	HOST_MALLOC(h_S, memSizeS);
	generateRand((Record*)h_S, TEST_MAX,sLen,1);
	cl_mem d_Rout;
	Record* h_Joinout;
	CL_smj( (Record* )h_R,rLen, (Record* )h_S, sLen, &h_Joinout,_CPU_GPU);
	printf("SMJ finished!\n");
}
void testMJ(int rLen, int sLen)
{
	int _CPU_GPU=0;
	int result=0;
	int memSizeR=sizeof(Record)*rLen;
	int memSizeS=sizeof(Record)*sLen;
	void *h_R;
	HOST_MALLOC(h_R, memSizeR);
	generateRand((Record*)h_R, TEST_MAX,rLen,0);
	void *h_S;
	HOST_MALLOC(h_S, memSizeS);
	generateRand((Record*)h_S, TEST_MAX,sLen,1);
	cl_mem d_Rout;
	Record* h_Joinout;
	CL_mj( (Record* )h_R,rLen, (Record* )h_S, sLen, &h_Joinout,_CPU_GPU );
	printf("MJ finished!\n");
}