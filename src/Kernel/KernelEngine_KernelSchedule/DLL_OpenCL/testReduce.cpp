#include "common.h"
#include "PrimitiveCommon.h"
#include "Helper.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
#include "CL\opencl.h"
#include "testReduce.h"
#include "KernelScheduler.h"
#include "OpenCL_DLL.h"
//#include "test
#ifndef	REDUCE_IMPL_CU
#define REDUCE_IMPL_CU
#endif
#define NUM_BANKS 16
#define LOG_NUM_BANKS 4
#define NUM_BANKS 16
#define LOG_NUM_BANKS 4
#define BLOCK_SIZE 256

// OpenCL Vars---------0 for CPU, 1 for GPU
extern cl_context Context;        // OpenCL context
extern cl_program Program;           // OpenCL program
extern cl_command_queue CommandQueue[2];// OpenCL command que
extern cl_platform_id Platform[2];      // OpenCL platform
extern cl_device_id Device[2];          // OpenCL device
extern cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
extern cl_device_id allDevices[10];
inline bool isPowerOfTwo(int n)
{
    return ((n&(n-1))==0) ;
}
bool howPartition( int rLen, int maxNumThread, int* info )
{
	int maxNumElePerBlock = maxNumThread*2;
	int numBlock = (int)ceil((float)rLen/(maxNumElePerBlock));
	bool isMul = (rLen%maxNumElePerBlock == 0)?true:false;

	if( isMul )
	{
		info[0] = numBlock;
		info[1] = maxNumThread;
	}
	else
	{
		info[0] = numBlock - 1;

		int remainer = rLen - (numBlock - 1)*maxNumElePerBlock;
		if( isPowerOfTwo(remainer) )
		{
			info[1] = remainer/2;
		}
		else
		{
			info[1] = floorPow2( remainer );
		}
	}

	return isMul;
}
void preallocBlockSums( int rLen, int numThread,tempResult *tR )
{
	unsigned int blockSize = numThread; // max size of the thread blocks
    unsigned int numElts = rLen;

    int level = 0;

    do
    {       
        unsigned int numBlocks = 
            max(1, (int)ceil((float)numElts / (2.f * blockSize)));
        if (numBlocks > 1)
        {
            level++;
        }
        numElts = numBlocks;
    } while (numElts > 1);

	//at least one level, the last level has the sum
	tR->d_numLevelsAllocated = level + 1;
	tR->d_scanBlockSums=(cl_mem*)malloc(sizeof(cl_mem)*1000);
	tR->levelSize = (int*)malloc( sizeof(int)*tR->d_numLevelsAllocated ); 

	numElts = rLen;
    level = 0;
    
    do
    {       
        unsigned int numBlocks = 
            max(1, (int)ceil((float)numElts / (2.f * blockSize)));
        if (numBlocks > 1) 
        {
			tR->levelSize[level] = numBlocks;
            CL_MALLOC(&tR->d_scanBlockSums[level],  sizeof(int) * numBlocks );
			level++;
        }
        numElts = numBlocks;
    } while (numElts > 1);

	//the last level
 	tR->levelSize[tR->d_numLevelsAllocated - 1] = 1;
	CL_MALLOC(&tR->d_scanBlockSums[tR->d_numLevelsAllocated - 1], sizeof(int) ) ;
//	printf("exit preallocBlockSums\n");
}
/*	void perscan_kernel( cl_mem d_odata, cl_mem d_idata, int numElementsPerBlock, bool isFull, int base, int d_odataOffset, int OPERATOR )*/
void perscan_kernel_int(cl_mem temp,cl_mem d_odata,cl_mem d_idata, int numElementsPerBlock, bool isFull, int base, int d_odataOffset, int OPERATOR,int numBlock, int numThread, int sharedMemSize,int rLen,
int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	//OpenCL limit:
	int bit_isFull;
	if(isFull)
		bit_isFull=1;
	else 
		bit_isFull=0;

	size_t numThreadsPerBlock_x=numThread;
	size_t globalWorkingSetSize=numThread*numBlock;

	cl_getKernel("perscan_kernel",kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_odata);
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_mem), (void*)&temp);
	ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_idata);
    ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_int), (void*)&numElementsPerBlock);
    ciErr1 |= clSetKernelArg((*kernel), 4, sizeof(cl_int), (void*)&bit_isFull);    
	ciErr1 |= clSetKernelArg((*kernel), 5, sizeof(cl_int), (void*)&base);    
	ciErr1 |= clSetKernelArg((*kernel), 6, sizeof(cl_int), (void*)&d_odataOffset);    
	ciErr1 |= clSetKernelArg((*kernel), 7, sizeof(cl_int), (void*)&OPERATOR);  
	ciErr1 |= clSetKernelArg((*kernel), 8, sizeof(cl_int), (void*)&sharedMemSize);   
 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	

	kernel_enqueue(1, 7//seems not related to size
		,1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
void getResult_kernel_init(cl_mem d_Result, cl_mem d_Rout, int rLen,int OPERATOR, int numThread,int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
// cl_mem d_Result, cl_mem d_Rout, int rLen, int OPERATOR
{
	size_t numThreadsPerBlock_x=numThread;
	size_t globalWorkingSetSize=numThread*numBlock;

	cl_getKernel("getResult_kernel",kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_Result);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_mem), (void*)&d_Rout);
    ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_int), (void*)&rLen);
    ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_mem), (void*)&OPERATOR);    

	if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	kernel_enqueue(rLen,
		1,1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
int reduceBlockSums( cl_mem d_Rout, int maxNumThread, int OPERATOR, int rLen,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,tempResult *tR, int _CPU_GPU)
{
	int* info = (int*)malloc( sizeof(int)*2 );
	cl_mem temp=NULL;
	CL_MALLOC(&temp, sizeof(int)*rLen );
	//get the information of partition	
	//return bool: if is multiple of maxNumThread
	//if yes, info[0]: number of blocks, info[1] = maxNumThread
	//if no, info[0]: number of blocks except of the last block, info[1]: number of thread in the last block
	for( int level = 0; level < ( tR->d_numLevelsAllocated - 1 ); level++ )
	{
		bool isMul = howPartition( tR->levelSize[level], maxNumThread, info );

		unsigned int numBlock = info[0];
		unsigned int numElementsPerBlock = 0;
		unsigned int extraSpace = 0;
		unsigned int sharedMemSize = 0;
		
		//scan the isP2 blocks
		if( numBlock > 0 )
		{
			numElementsPerBlock = maxNumThread*2;
			extraSpace = numElementsPerBlock/NUM_BANKS;
			sharedMemSize = sizeof(int)*( numElementsPerBlock + extraSpace );			
			perscan_kernel_int(temp, tR->d_scanBlockSums[level + 1], tR->d_scanBlockSums[level], numElementsPerBlock, true, 0, 0, OPERATOR, numBlock, maxNumThread,sharedMemSize,rLen,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
			clWaitForEvents(1,&eventList[(*index-1)%2]); 
		}
		clWaitForEvents(1,&eventList[(*index-1)%2]); 
		//scan the single not isP2 block
		if( (!isMul) || (numBlock == 0) )
		{
			unsigned int base = numElementsPerBlock*info[0];
			unsigned int offset = info[0];
			unsigned int remainer = tR->levelSize[level] - numElementsPerBlock*info[0];

			int numThread = info[1];//update the numThread
			clWaitForEvents(1,&eventList[(*index-1)%2]); 
			//only one number in the last block
			if( numThread == 0 )
			{				
				cl_copyBuffer((tR->d_scanBlockSums[level+1]), offset,
					tR->d_scanBlockSums[level], base, sizeof(int), index,eventList,Flag_CPU_GPU,burden,_CPU_GPU); 
			
			}
			else
			{
				numBlock = 1;
				numElementsPerBlock = numThread*2;
				extraSpace = numElementsPerBlock/NUM_BANKS;
				sharedMemSize = sizeof(int)*( numElementsPerBlock + extraSpace );
				
				if( isPowerOfTwo( remainer ) )
				{
					perscan_kernel_int(temp, tR->d_scanBlockSums[level + 1],tR->d_scanBlockSums[level], remainer, true, base, offset, OPERATOR,numBlock, numThread, sharedMemSize,rLen,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU); 
					clWaitForEvents(1,&eventList[(*index-1)%2]); 
				}
				else
				{
					
					perscan_kernel_int(temp,tR->d_scanBlockSums[level + 1], tR->d_scanBlockSums[level], remainer, false, base, offset, OPERATOR,numBlock, numThread, sharedMemSize,rLen,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
					clWaitForEvents(1,&eventList[(*index-1)%2]); 
				}
			}
			
		}
	}
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	getResult_kernel_init(tR->d_scanBlockSums[tR->d_numLevelsAllocated - 1], d_Rout, rLen, OPERATOR,1,1,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	CL_FREE(temp);
	return 1;
}
void reduce_deallocBlockSums(tempResult *tR)
{
    for (int i = 0; i < tR->d_numLevelsAllocated; i++)
    {
		CL_FREE(tR->d_scanBlockSums[i]);
    }

    free((void**)tR->d_scanBlockSums);
    tR->d_numLevelsAllocated = 0;
}
void perscanFirstPass_kernel_int(cl_mem t_temp,cl_mem d_temp,cl_mem d_odata,cl_mem d_idata, int numElementsPerBlock,bool isFull, int base, int d_odataOffset, int OPERATOR,int numBlock, int numThread, unsigned int sharedMemSize, int rLen,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	//OpenCL limit:
	int bit_isFull;
	if(isFull)
		bit_isFull=1;
	else 
		bit_isFull=0;

	size_t numThreadsPerBlock_x=numThread;
	size_t globalWorkingSetSize=numThread*numBlock;

	cl_getKernel("perscanFirstPass_kernel",kernel);
   
	cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&t_temp);	
    ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_mem), (void*)&d_temp);	
	ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_odata);
    ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_mem), (void*)&d_idata);
    ciErr1 |= clSetKernelArg((*kernel), 4, sizeof(cl_int), (void*)&numElementsPerBlock);   
	ciErr1 |= clSetKernelArg((*kernel), 5, sizeof(cl_int), (void*)&bit_isFull);   
	ciErr1 |= clSetKernelArg((*kernel), 6, sizeof(cl_int), (void*)&base);   
	ciErr1 |= clSetKernelArg((*kernel), 7, sizeof(cl_int), (void*)&d_odataOffset);   
	ciErr1 |= clSetKernelArg((*kernel), 8, sizeof(cl_int), (void*)&OPERATOR);   
	ciErr1 |= clSetKernelArg((*kernel), 9, sizeof(cl_int), (void*)&sharedMemSize);   
    
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	kernel_enqueue(1,6,
			1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
void copyLastElement_kernel_int(cl_mem d_Rout,cl_mem d_Rin, int base, int offset, int numBlock, int numThread,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThread;
	size_t globalWorkingSetSize=numThread*numBlock;
	
	cl_getKernel("copyLastElement_kernel",kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_Rout);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_mem), (void*)&d_Rin);
    ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&base);
    ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_int), (void*)&offset);   

    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	kernel_enqueue(1, 5,
		1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
void reduceFirstPass( cl_mem d_Rin, int rLen, int numThread, int numMaxBlock, int OPERATOR,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,tempResult *tR, int _CPU_GPU)
{
	int* info = (int*)malloc( sizeof(int)*2 );
	//get the information of partition	
	//return bool: if is multiple of maxNumThread
	//if yes, info[0]: number of blocks, info[1] = maxNumThread
	//if no, info[0]: number of blocks except of the last block, info[1]: number of thread in the last block
	bool isMul = howPartition( rLen, numThread, info );

	//scan the isP2 blocks	
	unsigned int numBlock = info[0];
	unsigned int numElementsPerBlock = 0;
	unsigned int extraSpace = 0;
	unsigned int sharedMemSize = 0;

	cl_mem d_temp; //for coalsed 
	CL_MALLOC( &d_temp, sizeof(int)*rLen );
	cl_mem t_temp=NULL;//!!!!!!!!!!
	CL_MALLOC( &t_temp, sizeof(int)*rLen );
	
	
	unsigned int base = 0;
	unsigned int offset = 0;
	cl_mem d_data;

	if( numBlock > 0 )
	{
		int numChunk = ceil( (float)numBlock/numMaxBlock );

		for( int chunkIdx = 0; chunkIdx < numChunk; chunkIdx++ )
		{
			base = chunkIdx*numElementsPerBlock*numMaxBlock;
			offset = chunkIdx*numMaxBlock;
			int subNumBlock = (chunkIdx == (numChunk - 1))?( numBlock - chunkIdx*numMaxBlock ):(numMaxBlock);

			numElementsPerBlock = numThread*2;
			extraSpace = numElementsPerBlock/NUM_BANKS;
			sharedMemSize = sizeof(int)*( numElementsPerBlock + extraSpace );
  			perscanFirstPass_kernel_int(t_temp, d_temp, tR->d_scanBlockSums[0], d_Rin, numElementsPerBlock, true, base, offset, OPERATOR,subNumBlock, numThread, sharedMemSize,rLen,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU );
			clWaitForEvents(1,&eventList[(*index-1)%2]); 
		}
	
	}
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	//scan the single not isP2 block
	if( (!isMul) || (numBlock == 0) )
	{
		base = numElementsPerBlock*info[0];
		offset = info[0];
		unsigned int remainer = rLen - numElementsPerBlock*info[0];

		numThread = info[1];//update the numThread
		
		//if only one elements
		if( numThread == 0 )
		{
			copyLastElement_kernel_int(tR->d_scanBlockSums[0], d_Rin, base, offset,1, 1,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
			clWaitForEvents(1,&eventList[(*index-1)%2]); 
		}
		else
		{
			numBlock = 1;
			numElementsPerBlock = numThread*2;
			extraSpace = numElementsPerBlock/NUM_BANKS;
			sharedMemSize = sizeof(int)*( numElementsPerBlock + extraSpace );
			
			if( isPowerOfTwo( remainer ) )
			{

				perscanFirstPass_kernel_int(t_temp, d_temp, tR->d_scanBlockSums[0], d_Rin, remainer, true, base, offset, OPERATOR ,numBlock, numThread, sharedMemSize,rLen, index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU );
				clWaitForEvents(1,&eventList[(*index-1)%2]); 
			}
			else
			{
				perscanFirstPass_kernel_int(t_temp,d_temp, tR->d_scanBlockSums[0], d_Rin, remainer, false, base, offset, OPERATOR ,numBlock, numThread, sharedMemSize,rLen,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU );
				clWaitForEvents(1,&eventList[(*index-1)%2]); 
			}	
		}
	
	}
	clWaitForEvents(1,&eventList[(*index-1)%2]);
	CL_FREE( d_temp );
	CL_FREE( t_temp );
}
int reduceImpl( cl_mem d_Rin, int rLen, cl_mem d_Rout, int OPERATOR, int numThread, int numMaxBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,tempResult *tR, int _CPU_GPU)
{
	preallocBlockSums( rLen, numThread,tR );
	reduceFirstPass( d_Rin, rLen, numThread, numMaxBlock, OPERATOR,index,eventList,kernel,Flag_CPU_GPU,burden,tR,_CPU_GPU );
	int result = reduceBlockSums( d_Rout, numThread, OPERATOR, rLen,index,eventList,kernel,Flag_CPU_GPU,burden,tR,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]);
	return result;
}
//return bool: if is multiple of maxNumThread
//if yes, info[0]: number of blocks, info[1] = maxNumThread
//if no, info[0]: number of blocks except of the last block, info[1]: number of thread in the last block
void testReduceImpl( int rLen, int OPERATOR, int numThreadPB , int numMaxBlock)
{
	int _CPU_GPU=0;
	int result=0;
	int memSize = sizeof(Record)*rLen;

	void * h_Rin;
	HOST_MALLOC(h_Rin, memSize );
	generateRand((Record *)h_Rin, TEST_MAX - 11111, rLen, 0 );

	void* h_Rout;

	unsigned int numResult = 0;
	cl_mem d_Rin=NULL;
	cl_mem d_Rout;
	CL_MALLOC( &d_Rin, memSize );
	cl_writebuffer( d_Rin, h_Rin, memSize,0);
	numResult= CL_AggMaxOnly( d_Rin, rLen, &d_Rout, numThreadPB, numMaxBlock,_CPU_GPU);
	HOST_MALLOC(h_Rout, sizeof(Record)*numResult );
	cl_readbuffer( h_Rout, d_Rout, sizeof(Record)*numResult,_CPU_GPU);
	//validateReduce((Record *)h_Rin, rLen,((Record *)h_Rout)[0].y, OPERATOR );	
	HOST_FREE( h_Rin );
	HOST_FREE( h_Rout );
	CL_FREE( d_Rin );
	CL_FREE( d_Rout );
	printf("testReduceFinish\n");
}
