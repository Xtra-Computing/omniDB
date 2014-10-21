#include "common.h"
#include "testScan.h"
#include "Helper.h"
#include "PrimitiveCommon.h"
#include "OpenCL_DLL.h"
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



void gCreateIndex_int(cl_mem g_data, cl_mem g_ptrDir, unsigned int nDirNodes, 
					   unsigned int tree_size, unsigned int bottom_start,  unsigned int nNodesPerBlock,
					  size_t threadPB, size_t numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=threadPB;
	size_t globalWorkingSetSize=threadPB*numBlock;
	cl_getKernel("gCreateIndex_kernel",Kernel);

    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&g_data);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_mem), (void*)&g_ptrDir);
    ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_int), (void*)&nDirNodes);
    ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_int), (void*)&tree_size);    
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&bottom_start);    
	ciErr1 |= clSetKernelArg((*Kernel), 5, sizeof(cl_int), (void*)&nNodesPerBlock);
    ////printf("clSetKernelArg 0 - 5...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(nNodesPerBlock, 43, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	
}

void cuda_create_indexImpl(cl_mem g_data, unsigned int nDataNodes, 
					   cl_mem* g_ptrDir, unsigned int* ptrDirSize,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	//#region Calculate parameters on host
	unsigned int lvlDir = uintCeilingLog(TREE_FANOUT, nDataNodes);
	unsigned int nDirNodes = uintCeilingDiv(nDataNodes - 1, TREE_NODE_SIZE);
	unsigned int tree_size = nDirNodes + nDataNodes;
	unsigned int bottom_start = ( uintPower(TREE_FANOUT, lvlDir) - 1 ) / TREE_NODE_SIZE;
	//#endregion

	// Allocate space for directory on device
	CL_MALLOC( g_ptrDir, sizeof(IDirectoryNode) * nDirNodes ) ;

	unsigned int nNodesPerBlock = uintCeilingDiv(nDirNodes, BLCK_PER_GRID_create);

	//#region Execute on device
	int numThreadPB=THRD_PER_BLCK_create;
	int numBlock= BLCK_PER_GRID_create;
	gCreateIndex_int (g_data, *g_ptrDir, nDirNodes, tree_size, bottom_start, nNodesPerBlock, numThreadPB, numBlock,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	//#endregion
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	// Passback the size of the directory
	*ptrDirSize = nDirNodes;
}

void gpu_init_treeImpl(cl_mem d_R, int rLen, int f, CUDA_CSSTree* h_me,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_mem d_data;
	cl_mem d_dir;
	unsigned int nDataNodes;
	unsigned int nDirNodes;

	nDataNodes = uintCeilingDiv(rLen, TREE_NODE_SIZE);
	unsigned int rSize = sizeof(Record) * rLen;
	unsigned int dSize = sizeof(IDataNode) * nDataNodes;
	if(rSize!=dSize)
	{
		CL_MALLOC(&d_data, dSize);
		memset_int(d_data, dSize, 0x7f,256,512,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU );
		cl_copyBuffer(d_data, d_R, rSize,index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
		printf("!!! allocating % byte for new data array~", dSize);
		CL_FREE(d_R);
	}
	else
		d_data=d_R;

	cuda_create_indexImpl(d_data, nDataNodes, &d_dir, &nDirNodes,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	h_me->data = d_data;
	h_me->nDataNodes = nDataNodes;
	h_me->dir = d_dir;
	h_me->nDirNodes = nDirNodes;

}
int gpu_constructCSSTreeImpl(cl_mem d_Rin, int rLen, CUDA_CSSTree **h_tree,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	//__DEBUd__("cuda_constructCSSTree");

	*h_tree = (CUDA_CSSTree*)malloc(sizeof(CUDA_CSSTree));
	gpu_init_treeImpl(d_Rin, rLen, 0, *h_tree,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);

	return 0;
}

void gSearchTree_int(cl_mem d_data, unsigned int nDataNodes, cl_mem d_dir, 
					   unsigned int nDirNodes, int lvlDir,
					   cl_mem d_keys, cl_mem d_locations, unsigned int nSearchKeys, unsigned int nKeysPerThread,
					   unsigned int tree_size, unsigned int bottom_start,
					  size_t threadPB, size_t numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=threadPB;
	size_t globalWorkingSetSize=threadPB*numBlock;
	cl_getKernel("gSearchTree_kernel",Kernel);

    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_data);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&nDataNodes);
    ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_mem), (void*)&d_dir);
    ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_int), (void*)&nDirNodes);    
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&lvlDir);    
	ciErr1 |= clSetKernelArg((*Kernel), 5, sizeof(cl_mem), (void*)&d_keys);    
	ciErr1 |= clSetKernelArg((*Kernel), 6, sizeof(cl_mem), (void*)&d_locations);    
	ciErr1 |= clSetKernelArg((*Kernel), 7, sizeof(cl_int), (void*)&nSearchKeys);    
	ciErr1 |= clSetKernelArg((*Kernel), 8, sizeof(cl_int), (void*)&nKeysPerThread); 
	ciErr1 |= clSetKernelArg((*Kernel), 9, sizeof(cl_int), (void*)&tree_size);    
	ciErr1 |= clSetKernelArg((*Kernel), 10, sizeof(cl_int), (void*)&bottom_start); 
    //printf("clSetKernelArg 0 - 10...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	int rLen=nDirNodes*nKeysPerThread/THRD_PER_GRID_search;
	kernel_enqueue(rLen, 40, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	
}

void cuda_search_indexImpl(cl_mem d_data, unsigned int nDataNodes, cl_mem d_dir, 
					   unsigned int nDirNodes,cl_mem d_keys, cl_mem d_locations, unsigned int nSearchKeys,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	unsigned int lvlDir = uintCeilingLog(TREE_FANOUT, nDataNodes);
	unsigned int tree_size = nDataNodes + nDirNodes;
	unsigned int bottom_start = ( uintPower(TREE_FANOUT, lvlDir) - 1 ) / TREE_NODE_SIZE;

	size_t threadPB=THRD_PER_BLCK_search;
	size_t numBlock=BLCK_PER_GRID_search;

	unsigned int nKeysPerThread = uintCeilingDiv(nSearchKeys, THRD_PER_GRID_search);
		gSearchTree_int (d_data, nDataNodes, d_dir, nDirNodes, lvlDir,
			d_keys, d_locations, nSearchKeys, nKeysPerThread, tree_size, bottom_start,
			threadPB, numBlock,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

void gIndexJoin(cl_mem d_R, unsigned int rLen,
				 cl_mem d_S, cl_mem d_locations, unsigned int sLen,
				 cl_mem d_ResNums, unsigned int clusterSize,
					  size_t threadPB, size_t numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=threadPB;
	size_t globalWorkingSetSize=threadPB*numBlock;
	cl_getKernel("gIndexJoin_kernel",Kernel);

    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&rLen);
    ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_mem), (void*)&d_S);
    ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_mem), (void*)&d_locations);    
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&sLen);    
	ciErr1 |= clSetKernelArg((*Kernel), 5, sizeof(cl_mem), (void*)&d_ResNums);    
	ciErr1 |= clSetKernelArg((*Kernel), 6, sizeof(cl_int), (void*)&clusterSize);
    //printf("clSetKernelArg 0 - 10...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(sLen, 41, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	
}

void gJoinWithWrite(cl_mem d_R, unsigned int rLen,
				 cl_mem d_S, cl_mem d_locations, unsigned int sLen,
				 cl_mem d_sum,
				 cl_mem pd_Results, unsigned int clusterSize,
					  size_t threadPB, size_t numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=threadPB;
	size_t globalWorkingSetSize=threadPB*numBlock;
	cl_getKernel("gJoinWithWrite_kernel",Kernel);

    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&rLen);
    ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_mem), (void*)&d_S);
    ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_mem), (void*)&d_locations);    
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&sLen);    
	ciErr1 |= clSetKernelArg((*Kernel), 5, sizeof(cl_mem), (void*)&d_sum);    
	ciErr1 |= clSetKernelArg((*Kernel), 6, sizeof(cl_mem), (void*)&pd_Results);    
	ciErr1 |= clSetKernelArg((*Kernel), 7, sizeof(cl_int), (void*)&clusterSize);
    //printf("clSetKernelArg 0 - 10...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	kernel_enqueue(rLen, 42, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	
}

int cuda_join_after_search(cl_mem d_R, int rLen, cl_mem d_S, cl_mem d_locations, 
						   unsigned int sLen, 	cl_mem* pd_Results,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int clusterSize = uintCeilingDiv(sLen, THRD_PER_GRID_join);

	size_t threadPB=BLCK_PER_GRID_join;
	size_t numBlock=THRD_PER_BLCK_join;

	cl_mem d_ResNums;
	CL_MALLOC(&d_ResNums, sizeof(int) * THRD_PER_GRID_join);
	gIndexJoin(d_R, rLen, d_S, d_locations, sLen, d_ResNums, clusterSize,threadPB, numBlock,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);

	cl_mem d_sum;	
	CL_MALLOC(&d_sum, sizeof(int) * THRD_PER_GRID_join);
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(THRD_PER_GRID_join,SP);
	scanImpl(d_ResNums, THRD_PER_GRID_join, d_sum,index,eventList,Kernel,Flag_CPU_GPU,burden,SP,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	closeScan(SP);

	int sum = 0;
	int last;

	cl_readbuffer((void*)&last, d_ResNums, (THRD_PER_GRID_join-1)*sizeof(int), sizeof(int),index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	cl_readbuffer((void*)&sum, d_sum, (THRD_PER_GRID_join-1)*sizeof(int), sizeof(int),index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
   	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	sum += last;
	CL_MALLOC(pd_Results, sizeof(Record) * sum);
	gJoinWithWrite(d_R, rLen, d_S, d_locations, sLen, d_sum, *pd_Results, clusterSize, threadPB, numBlock,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	CL_FREE(d_ResNums);
	CL_FREE(d_sum);
	return sum;
}
void gSearchTree_usingKeys(cl_mem d_data, unsigned int nDataNodes, cl_mem d_dir, 
					   unsigned int nDirNodes, int lvlDir,
					   cl_mem d_keys, cl_mem d_locations, unsigned int nSearchKeys, unsigned int nKeysPerThread,
					   unsigned int tree_size, unsigned int bottom_start,
					  size_t threadPB, size_t numBlock,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=threadPB;
	size_t globalWorkingSetSize=threadPB*numBlock;
	cl_getKernel("gSearchTree_usingKeys_kernel",Kernel);

    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_data);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&nDataNodes);
    ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_mem), (void*)&d_dir);
    ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_int), (void*)&nDirNodes);    
	ciErr1 |= clSetKernelArg((*Kernel), 4, sizeof(cl_int), (void*)&lvlDir);    
	ciErr1 |= clSetKernelArg((*Kernel), 5, sizeof(cl_mem), (void*)&d_keys);    
	ciErr1 |= clSetKernelArg((*Kernel), 6, sizeof(cl_mem), (void*)&d_locations);    
	ciErr1 |= clSetKernelArg((*Kernel), 7, sizeof(cl_int), (void*)&nSearchKeys);    
	ciErr1 |= clSetKernelArg((*Kernel), 8, sizeof(cl_int), (void*)&nKeysPerThread); 
	ciErr1 |= clSetKernelArg((*Kernel), 9, sizeof(cl_int), (void*)&tree_size);    
	ciErr1 |= clSetKernelArg((*Kernel), 10, sizeof(cl_int), (void*)&bottom_start); 
    //printf("clSetKernelArg 0 - 10...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	int rLen=nDirNodes*nKeysPerThread/THRD_PER_GRID_search;
	kernel_enqueue(rLen, 45, 
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	
}

void cuda_search_index_usingKeys(cl_mem g_data, unsigned int nDataNodes, cl_mem g_dir, 
								 unsigned int nDirNodes, cl_mem g_keys, cl_mem g_locations, 
								 unsigned int nSearchKeys,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	unsigned int lvlDir = uintCeilingLog(TREE_FANOUT, nDataNodes);
	unsigned int tree_size = nDataNodes + nDirNodes;
	unsigned int bottom_start = ( uintPower(TREE_FANOUT, lvlDir) - 1 ) / TREE_NODE_SIZE;

	int numThreadPB=THRD_PER_BLCK_search;
	int numBlock=BLCK_PER_GRID_search;

	unsigned int nKeysPerThread = uintCeilingDiv(nSearchKeys, THRD_PER_GRID_search);

	gSearchTree_usingKeys (g_data, nDataNodes, g_dir, nDirNodes, lvlDir, g_keys, 
		g_locations, nSearchKeys, nKeysPerThread, tree_size, bottom_start, numThreadPB, numBlock,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU );
}