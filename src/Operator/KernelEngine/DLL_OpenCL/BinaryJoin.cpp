#include "common.h"
#include "testJoin.h"
#include "OpenCL_DLL.h"
#include "KernelScheduler.h"
#include "scheduler.h"
//extern "C" int CL_ninljOnly(cl_mem d_R,int rLen, cl_mem d_S, int sLen, cl_mem* d_Rout, int _CPU_GPU)
extern "C" int CL_ninlj(Record* h_R, int rLen, Record* h_S, int sLen,Record** h_Rout,int _CPU_GPU )
{
	int memSizeR=sizeof(Record)*rLen;
	int memSizeS=sizeof(Record)*sLen;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;
	cl_mem d_Rout;
	cl_mem d_R;
	CL_MALLOC(& d_R, memSizeR);
	cl_writebuffer( d_R, h_R, memSizeR,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	cl_mem d_S;
	CL_MALLOC(& d_S, memSizeS );
	cl_writebuffer( d_S, h_S, memSizeR,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	result=NINJImpl(d_R,rLen,d_S,sLen,&d_Rout, &index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE(d_R);
	CL_FREE(d_S);
	CL_FREE(d_Rout);
	clReleaseKernel(Kernel);
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("NINLJFinish\n");
	return result;
}
extern "C" int CL_smjOnly(cl_mem d_R, int rLen, cl_mem d_S, int sLen, cl_mem*  h_Joinout,int _CPU_GPU)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int numResult = SMJImpl(d_R, rLen, d_S, sLen, h_Joinout,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("NINLJFinish\n");
	return numResult;
}
extern "C" int	CL_smj( Record* h_R, int rLen, Record* h_S, int sLen, Record** h_Joinout,int _CPU_GPU )
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	cl_mem d_R;
	cl_mem d_S;
	cl_mem d_Joinout;

	CL_MALLOC( &d_R, sizeof(Record)*rLen );
	CL_MALLOC( &d_S, sizeof(Record)*sLen );
	cl_writebuffer( d_R, h_R, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_S, h_S, sizeof(Record)*sLen ,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);

	int numResult = SMJImpl(d_R, rLen, d_S, sLen, &d_Joinout,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	*h_Joinout = (Record*)malloc( sizeof(Record)*numResult );
	cl_readbuffer( *h_Joinout, d_Joinout, sizeof(Record)*numResult ,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE(d_R);
	CL_FREE(d_S);
	CL_FREE(d_Joinout);
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("NINLJFinish\n");
	return numResult;
}

extern "C" int CL_mj( void * h_Rin, int rLen, Record* h_Sin, int sLen, Record** h_Joinout,int _CPU_GPU  )
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	cl_mem d_Rin;
	cl_mem d_Sin;
	cl_mem d_Joinout;

	CL_MALLOC( &d_Rin, sizeof(Record)*rLen );
	CL_MALLOC( &d_Sin, sizeof(Record)*sLen );

	cl_writebuffer( d_Rin, h_Rin, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_Sin, h_Sin, sizeof(Record)*sLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );

	int outSize = MJImpl( d_Rin, rLen, d_Sin, sLen, &d_Joinout ,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	*h_Joinout = (Record*)malloc( sizeof(Record)*outSize );
	//HOST_MALLOC( (void**)h_Joinout, sizeof(Record)*outSize );

	cl_readbuffer( *h_Joinout, d_Joinout, sizeof(Record)*outSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE(d_Rin);
	CL_FREE(d_Sin);
	CL_FREE(d_Joinout);
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("CL_mj\n");
	return outSize;
}

extern "C" int CL_hjOnly(cl_mem d_R, int rLen, cl_mem d_S, int sLen,cl_mem* h_Rout ,int _CPU_GPU)
{
	int result = 0;
	cl_uint  rHashTableBucketNum = 2 * 1024 * 1024;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	cl_mem rHashTable;
	CL_MALLOC(&rHashTable,rLen * sizeof(Record) + rHashTableBucketNum * sizeof(cl_uint));
	result = HJImpl(d_R,rLen,d_S,sLen,rHashTable,h_Rout,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);	
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE(rHashTable);
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	return result;
}
extern "C" int CL_hj( Record* h_R, int rLen, Record* h_S, int sLen, Record** h_Rout ,int _CPU_GPU)
{
	int result = 0;
	cl_uint  rHashTableBucketNum = 2 * 1024 * 1024;

	//size of R and S tables
	int memSizeR = sizeof(Record) * rLen;
	int memSizeS = sizeof(Record) * sLen;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	cl_mem d_Rout;
	cl_mem d_R;
	CL_MALLOC(&d_R,memSizeR);
	cl_writebuffer(d_R,h_R,memSizeR,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	cl_mem d_S;
	CL_MALLOC(&d_S,memSizeS);
	cl_writebuffer(d_S,h_S,memSizeS,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	cl_mem rHashTable;
	CL_MALLOC(&rHashTable,rLen * sizeof(Record) + rHashTableBucketNum * sizeof(cl_uint));

	result = HJImpl(d_R,rLen,d_S,sLen,rHashTable,&d_Rout,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	*h_Rout = (Record*)malloc( sizeof(Record)*result );
	cl_readbuffer((*h_Rout),d_Rout,sizeof(Record)*result,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE(d_R);
	CL_FREE(d_S);
	CL_FREE(d_Rout);
	CL_FREE(rHashTable);
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("HJFinish\n");
	return result;
	
}
