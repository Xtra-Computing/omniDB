#include "common.h"
#include "OpenCL_DLL.h"
#include "Helper.h"
#include "testJoin.h"
#include "KernelScheduler.h"
#include "scheduler.h"
#include "CSSTree.h"
extern "C" int  CL_inljOnly( cl_mem h_Rin, int rLen, CUDA_CSSTree** h_tree,cl_mem h_Sin, int sLen, cl_mem* h_Rout, int _CPU_GPU )
{		
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	//bufferchecking(h_Rin,100);
	//bufferchecking(h_Sin,100);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	gpu_constructCSSTreeImpl(h_Rin, rLen, h_tree,  &index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	int outSize = INLJImpl(h_Rin, rLen, *h_tree, h_Sin, sLen, h_Rout, &index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("INLJFinish\n");
	return outSize;
}


extern "C" int CL_inlj( Record* h_Rin, int rLen, CUDA_CSSTree** h_tree, Record* h_Sin, int sLen, Record** h_Rout,int _CPU_GPU )
{		
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_mem d_Rin;
	cl_mem d_Sin;
	cl_mem d_Rout;

	CL_MALLOC( &d_Rin, sizeof(Record)*rLen );
	CL_MALLOC( &d_Sin, sizeof(Record)*sLen );

	cl_writebuffer( d_Rin, h_Rin, sizeof(Record)*rLen, &index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	gpu_constructCSSTreeImpl(d_Rin, rLen, h_tree,  &index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	cl_writebuffer( d_Sin, h_Sin, sizeof(Record)*sLen, &index,eventList,&CPU_GPU,&burden,_CPU_GPU);

	int outSize = INLJImpl(d_Rin, rLen, *h_tree, d_Sin, sLen, &d_Rout, &index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	*h_Rout = (Record*)malloc( sizeof(Record)*outSize );
	cl_readbuffer( *h_Rout, d_Rout, sizeof(Record)*outSize, &index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_Sin );
	CL_FREE( d_Rout );
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("INLJFinish\n");
	return outSize;
	
}

