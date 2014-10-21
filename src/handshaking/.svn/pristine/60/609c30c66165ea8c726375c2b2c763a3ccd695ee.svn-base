#include "common.h"
#include "testRIDList.h"
#include "testValue.h"
#include "OpenCL_DLL.h"
#include "KernelScheduler.h"
extern "C" void CL_setRIDList(cl_mem h_RIDList, int rLen, cl_mem h_destRin, int numThreadPB, int numBlock,int _CPU_GPU)
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	////
	int outputSize=sizeof(int)*rLen;
	cl_mem d_tempOutput;
	CL_MALLOC(&d_tempOutput, outputSize);

	int numThread=numThreadPB*numBlock;
	dim3  thread( numThreadPB, 1, 1);//????
	dim3  grid( numBlock, 1 , 1);//??????????

	setRIDListImpl (h_RIDList,d_tempOutput,numThread, rLen, h_destRin, numThreadPB, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	CL_FREE(d_tempOutput);
	//bufferchecking(h_destRin,sizeof(Record)*1);

}

extern "C" void CL_getRIDList( cl_mem h_Rin, int rLen, cl_mem* h_RIDList,int numThreadPB, int numBlock,int _CPU_GPU)
{
	//	getRIDList_kernel(Record *d_R, int delta, int rLen,int *d_RIDList, int *d_output) 
	//getRIDList_kernel<<<grid,thread>>>(d_Rin, numThread, rLen, *d_RIDList, d_tempOutput);
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int outputSize=sizeof(int)*rLen;
	CL_MALLOC(h_RIDList, outputSize);
	cl_mem d_tempOutput;
	CL_MALLOC(&d_tempOutput, outputSize);

	int numThread=numThreadPB*numBlock;
	dim3  thread( numThreadPB, 1, 1);
	dim3  grid( numBlock, 1 , 1);
	getRIDListImpl (h_Rin,numThread,rLen,*h_RIDList,d_tempOutput, 
		numThreadPB,numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	CL_FREE(d_tempOutput);
	//bufferchecking(*h_RIDList,sizeof(Record)*1);
}


extern "C" void CL_setValueList(cl_mem h_ValueList, int rLen, cl_mem h_destRin, int numThreadPB, int numBlock,int _CPU_GPU)
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int numThread=numThreadPB*numBlock;
	dim3  thread( numThreadPB, 1, 1);
	dim3  grid( numBlock, 1 , 1);
	
	setValueListImpl(h_ValueList,numThread,rLen,h_destRin,numThreadPB,numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	//bufferchecking(h_destRin,sizeof(Record)*1);
}

extern "C" void CL_getValueList( cl_mem h_Rin, int rLen, cl_mem* h_ValueList,int numThreadPB, int numBlock,int _CPU_GPU)
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int outputSize=sizeof(int)*rLen;
	CL_MALLOC(h_ValueList, outputSize);
	cl_mem d_tempOutput;
	CL_MALLOC(&d_tempOutput, outputSize);

	int numThread=numThreadPB*numBlock;
	dim3  thread( numThreadPB, 1, 1);
	dim3  grid( numBlock, 1 , 1);
//	getValueList_kernel(Record *d_R, int delta, int rLen,int *d_ValueList, int *d_output) 
//	getValueList_kernel<<<grid,thread>>>(d_Rin, numThread, rLen, *d_ValueList, d_tempOutput);
	getValueListImpl(h_Rin,numThread,rLen,h_ValueList,d_tempOutput,numThreadPB,numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	CL_FREE(d_tempOutput);
	//bufferchecking(*h_ValueList,sizeof(Record)*1);
}