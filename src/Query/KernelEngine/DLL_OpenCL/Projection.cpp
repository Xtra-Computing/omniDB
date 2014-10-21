#include "common.h"
#include "testProjection.h"
#include "KernelScheduler.h"
#include "OpenCL_DLL.h"
/*finilize-tony*/
extern "C" void CL_ProjectionOnly(cl_mem d_Rin,int rLen, cl_mem d_projTable, int pLen, 
														   int numThread, int numBlock , int _CPU_GPU)
{	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	projectionImpl( d_Rin, rLen, d_projTable, pLen,numThread,numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
}

extern "C" void CL_Projection( Record * h_Rin, int rLen, Record* h_projTable, int pLen, 
														   int numThread, int numBlock , int _CPU_GPU)
{
	int memSize=sizeof(Record)*rLen;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	cl_mem d_Rin;
	cl_mem d_projTable;
	CL_MALLOC( &d_Rin, sizeof(Record)*rLen );
	CL_MALLOC( &d_projTable, sizeof(Record)*pLen );
	cl_writebuffer( d_Rin, h_Rin, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	cl_writebuffer( d_projTable, h_projTable, sizeof(Record)*pLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	//double t1=DLL_getTimer(timer);
	projectionImpl( d_Rin, rLen, d_projTable, pLen,numThread,numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	//double t2=DLL_getTimer(timer);

	cl_readbuffer( h_projTable, d_projTable, sizeof(Record)*pLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );

	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_projTable );
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("testProjectionFinish\n");
}