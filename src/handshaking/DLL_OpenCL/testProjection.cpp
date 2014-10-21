#include "common.h"
#include "PrimitiveCommon.h"
#include "testScatter.h"
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

void projection_map_int(char* kernelName, cl_mem d_projTable, int pLen, cl_mem d_loc, cl_mem d_temp, int numThreadPB, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel(kernelName,kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*kernel), 0, sizeof(cl_mem), (void*)&d_projTable);	
	ciErr1 |= clSetKernelArg((*kernel), 1, sizeof(cl_int), (void*)&pLen);
    ciErr1 |= clSetKernelArg((*kernel), 2, sizeof(cl_mem), (void*)&d_loc);
    ciErr1 |= clSetKernelArg((*kernel), 3, sizeof(cl_mem), (void*)&d_temp);    
    //printf("clSetKernelArg 0 - 4...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	kernel_enqueue(pLen, 0,1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
void projectionImpl(cl_mem d_Rin, int rLen, cl_mem d_projTable, int pLen, 
					int numThread, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_mem d_loc;
	CL_MALLOC( &d_loc, sizeof(int)*rLen ) ;
	cl_mem d_temp=NULL;
	CL_MALLOC(&d_temp, sizeof(int)*rLen);
	projection_map_int("projection_map_kernel", d_projTable, pLen, d_loc, d_temp,numBlock, numThread,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	CL_FREE( d_temp );
   //gatherImpl(d_Rin,rLen,d_loc,d_Rout,rLen, numThread, numBlock,CPU_GPU);
	gatherImpl( d_Rin, rLen, d_loc, d_projTable,pLen, numThread, numBlock, index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	CL_FREE( d_loc );
}

void testProjection( int rLen, int pLen, int numThread, int numBlock ) 
{
	int _CPU_GPU=0;
	int memSize=sizeof(Record)*rLen;
	void *h_Rin;
	Record *h_projTable;
	Record *originalProjTable; //for check
	HOST_MALLOC(h_Rin, memSize);
	//HOST_MALLOC(h_projTable, memSize);
	//HOST_MALLOC(originalProjTable, memSize);
	h_projTable=(Record *)malloc(sizeof(Record)*pLen );
	originalProjTable=(Record *)malloc(sizeof(Record)*pLen );

	generateRand((Record *)h_Rin, TEST_MAX, rLen, 0 );
	for( int i = 0; i < pLen; i++ )
	{
		h_projTable[i].x = rand()%rLen;
		originalProjTable[i].x = h_projTable[i].x;
	}
	CL_Projection( (Record *) h_Rin,  rLen,( Record* )h_projTable,  pLen, 
														    numThread ,  numBlock , _CPU_GPU);
	
}
