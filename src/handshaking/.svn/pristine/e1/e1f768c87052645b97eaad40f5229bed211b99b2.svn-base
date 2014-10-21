#include "common.h"
#include "testMap.h"
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
extern cl_ulong totalGlobalMemory[2];      /**< Max global memory allowed */
//void mapImpl_int(Record *d_R, int rLen, int *d_S1, cl_mem d_S2, int numThreadPB=256, int numBlock=512)
void mapImpl_int (char* kernelName, cl_mem d_R, int rLen, 
	cl_mem d_S1, cl_mem d_S2, int numThreadPB, int numBlock,
	int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x=numThreadPB;
	size_t globalWorkingSetSize=numThreadPB*numBlock;
	cl_getKernel(kernelName,Kernel);
    // Set the Argument values
    cl_int ciErr1 = clSetKernelArg((*Kernel), 0, sizeof(cl_mem), (void*)&d_R);	
	ciErr1 |= clSetKernelArg((*Kernel), 1, sizeof(cl_int), (void*)&rLen);
    ciErr1 |= clSetKernelArg((*Kernel), 2, sizeof(cl_mem), (void*)&d_S1);
    ciErr1 |= clSetKernelArg((*Kernel), 3, sizeof(cl_mem), (void*)&d_S2);    
    ////printf("clSetKernelArg 0 - 4...\n\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }	
	kernel_enqueue(rLen, 14,1, &globalWorkingSetSize, 
		&numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}
void testMapImpl(int rLen, int numThreadPB , int numBlock)
{	
	//int _CPU_GPU=0;
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU=0;
	double burden;

	int result=0;
	int memSize=sizeof(Record)*rLen;
	int outSize=sizeof(int)*rLen;
	void *Rin;
	HOST_MALLOC(Rin, memSize);
	generateRand((Record *)Rin,TEST_MAX,rLen,0);
	void *Rout;
	HOST_MALLOC(Rout, outSize);
	cl_mem d_Rin=NULL;
	CL_MALLOC(&d_Rin, memSize);
	cl_mem d_Rout=NULL;
	CL_MALLOC(&d_Rout, outSize);
	cl_mem d_Rout2=NULL;
	CL_MALLOC(&d_Rout2, outSize);
	//printf("Write buffer!\n");
	cl_writebuffer( d_Rin, Rin, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,0);
	//printf("Write buffer Done!\n");
	mapImpl_int("mapImpl_kernel", d_Rin,rLen,d_Rout, d_Rout2, numThreadPB, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,0);

	cl_readbuffer( Rout, d_Rout, outSize,&index,eventList,&CPU_GPU,&burden,0 );

	/*for(int i=0;i<rLen;i++)
	{
		if(((int*)Rout)[i]!=i)
		{
		//	break;
			printf("%d, %d\t",i, ((int*)Rout)[i]);
		}
	}*/
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	CL_FREE(d_Rin);
	CL_FREE(d_Rout);
	CL_FREE(d_Rout2);
	HOST_FREE(Rin);	
	HOST_FREE(Rout);
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("MapFinish\n");
}