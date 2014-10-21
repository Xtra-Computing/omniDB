#include "common.h"
#include "KernelScheduler.h"
#include "testScan.h"
#include "Helper.h"
#include "PrimitiveCommon.h"
#include "OpenCL_DLL.h"
// OpenCL Vars---------0 for CPU, 1 for GPU
extern cl_context Context;        // OpenCL context
extern cl_program Program;           // OpenCL program
extern cl_command_queue CommandQueue[2];// OpenCL command que
extern cl_platform_id Platform[2];      // OpenCL platform
extern cl_device_id Device[2];          // OpenCL device
extern cl_kernel Kernel[2];             // OpenCL kernel
extern cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
extern cl_device_id allDevices[10];

//the indexed relation is R!!
int INLJImpl(cl_mem R, int rLen, CUDA_CSSTree *d_tree, cl_mem d_S, int sLen, cl_mem* d_Rout,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	cl_mem	d_locations;	// Location array on device
	CL_MALLOC(&d_locations, sizeof(int) * sLen);	

	cuda_search_indexImpl(d_tree->data, d_tree->nDataNodes, d_tree->dir, d_tree->nDirNodes, d_S, d_locations, sLen,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(*index-1)%2]); 
	//bufferchecking(d_tree->data,1);
	//bufferchecking(d_tree->dir,1);
	//bufferchecking(d_locations,1);
	int result = cuda_join_after_search(d_tree->data, rLen, d_S, d_locations, sLen, d_Rout,index,eventList,Kernel,Flag_CPU_GPU,burden, _CPU_GPU);
	//bufferchecking(*d_Rout,1);
	clWaitForEvents(1,&eventList[((*index)-1)%2]); 
	CL_FREE(d_locations);
	return result;
}
void testINLJ(int rLen, int sLen)
{
	int _CPU_GPU=0;
	int result=0;
	int memSizeR=sizeof(Record)*rLen;
	int memSizeS=sizeof(Record)*sLen;
	void *h_R;
	HOST_MALLOC(h_R, memSizeR);
	//generateSort((Record*)h_R, TEST_MAX,rLen,0);
	generateRand((Record*)h_R, TEST_MAX,rLen,0);
	void *h_Temp;
	HOST_MALLOC(h_Temp, memSizeR);
	CL_RadixSort((Record*)h_R,rLen,(Record*)h_Temp,256,64,0);
	CUDA_CSSTree* h_tree;	
	void *h_S;
	HOST_MALLOC(h_S, memSizeS);
	generateRand((Record*)h_S, TEST_MAX,sLen,1);
	cl_mem d_Rout;
	Record *h_Rout;
	CL_inlj( (Record*)h_Temp, rLen, &h_tree, (Record*) h_S,  sLen, &h_Rout,_CPU_GPU);
	printf("INLJFinish\n");
}