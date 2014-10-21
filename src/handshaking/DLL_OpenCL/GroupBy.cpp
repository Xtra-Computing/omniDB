#include "common.h" 
#include "testGroupBy.h"
#include "OpenCL_DLL.h"
#include "scheduler.h"
extern "C" int CL_GroupBy(Record * h_Rin, int rLen, Record* h_Rout, int** h_startPos, 
					int numThread, int numBlock , int _CPU_GPU)
{
	cl_mem d_Rin;
	cl_mem d_Rout;
	cl_mem d_startPos;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int memSize = sizeof(Record)*rLen;


	CL_MALLOC( &d_Rin, memSize );
	CL_MALLOC(&d_Rout, memSize );
	cl_writebuffer( d_Rin, h_Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	int numGroup = 0;


	numGroup= groupByImpl(d_Rin, rLen, d_Rout, &d_startPos, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	(*h_startPos) = (int*)malloc( sizeof(int)*numGroup );

	cl_readbuffer( *h_startPos, d_startPos, sizeof(int)*numGroup,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	cl_readbuffer( h_Rout, d_Rout, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_Rout );
	CL_FREE( d_startPos );
	clReleaseKernel(Kernel);
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	printf("CL_GroupBy\n");
	return numGroup;
}
