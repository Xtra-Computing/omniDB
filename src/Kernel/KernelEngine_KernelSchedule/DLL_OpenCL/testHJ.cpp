#include "common.h"
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

void HJbuild_int(cl_mem d_R, cl_mem rHashTable, int rLen, int sLen, int rHashTableBucketNum,
	size_t globalSize, size_t groupSize,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x = groupSize;
	size_t globalWorkingSetSize = globalSize;
	cl_getKernel("build_kernel",Kernel);

	//configure build kernel
	cl_int ciErr1 =  clSetKernelArg((*Kernel),0,sizeof(cl_mem),&d_R);
	ciErr1 |= clSetKernelArg((*Kernel),1,sizeof(cl_mem),&rHashTable);
	ciErr1 |= clSetKernelArg((*Kernel),2,sizeof(cl_uint),(void*)&rLen);
	ciErr1 |= clSetKernelArg((*Kernel),3,sizeof(cl_uint),(void*)&sLen);
	ciErr1 |= clSetKernelArg((*Kernel),4,sizeof(cl_uint),(void*)&rHashTableBucketNum);

	if (ciErr1 != CL_SUCCESS)
	{
		printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
		cl_clean(EXIT_FAILURE);
	}
	kernel_enqueue(rLen, 49
		,1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

void HJprobe_int(cl_mem rHashTable, cl_mem d_S, cl_mem* d_Rout, int rLen, int sLen,
	int rHashTableBucketNum, int resultsNum, size_t globalSize, size_t groupSize,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	size_t numThreadsPerBlock_x = groupSize;
	size_t globalWorkingSetSize = globalSize;
	cl_getKernel("probe_kernel",Kernel);

	//configure probe kernel
	cl_int ciErr1 =  clSetKernelArg((*Kernel),0,sizeof(cl_mem),&rHashTable);
	ciErr1 |= clSetKernelArg((*Kernel),1,sizeof(cl_mem),&d_S);
	ciErr1 |= clSetKernelArg((*Kernel),2,sizeof(cl_mem),d_Rout);
	ciErr1 |= clSetKernelArg((*Kernel),3,sizeof(cl_uint),(void*)&rLen);
	ciErr1 |= clSetKernelArg((*Kernel),4,sizeof(cl_uint),(void*)&sLen);
	ciErr1 |= clSetKernelArg((*Kernel),5,sizeof(cl_uint),(void*)&rHashTableBucketNum);
	ciErr1 |= clSetKernelArg((*Kernel),6,sizeof(cl_uint),(void*)&resultsNum);

	if (ciErr1 != CL_SUCCESS)
	{
		printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
		cl_clean(EXIT_FAILURE);
	}
	kernel_enqueue(rLen/rHashTableBucketNum, 50
		,1, &globalWorkingSetSize, &numThreadsPerBlock_x,eventList,index,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

int HJImpl(cl_mem d_R, cl_uint rLen, cl_mem d_S, cl_uint sLen, cl_mem rHashTable, cl_mem* d_Rout, int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_uint  resultsNum = rLen;
	cl_uint  rHashTableBucketNum = 2 * 1024 * 1024;
	size_t groupSize = 256, globalSize = 8192;
	CL_MALLOC(d_Rout,sizeof(Record) * resultsNum * 2);

HJbuild_int(d_R, rHashTable, rLen , sLen, rHashTableBucketNum,
	globalSize,groupSize,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);

HJprobe_int(rHashTable,d_S,d_Rout,rLen,sLen,rHashTableBucketNum,
	resultsNum,globalSize,groupSize,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	return resultsNum;
}

void testHJ(int rLen, int sLen)
{
	int _CPU_GPU=0;
	int result = 0;
	cl_uint  rHashTableBucketNum = 2 * 1024 * 1024;

	//size of R and S tables
	int memSizeR = sizeof(Record) * rLen;
	int memSizeS = sizeof(Record) * sLen;
	void * h_R;
	HOST_MALLOC(h_R,memSizeR);
	generateRand((Record*)h_R,TEST_MAX,rLen,0);
	void * h_S;
	HOST_MALLOC(h_S,memSizeS);
	generateRand((Record*)h_S,TEST_MAX,sLen,1);
	Record * h_Rout;
	CL_hj((Record *)h_R,rLen,(Record*)h_S,sLen,&h_Rout,_CPU_GPU);
	printf("HJFinish\n");
}