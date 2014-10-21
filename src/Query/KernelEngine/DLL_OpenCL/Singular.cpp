#include "common.h"
#include "Helper.h"
#include "testReduce.h"
#include "KernelScheduler.h"
#include "OpenCL_DLL.h"
inline void reduce_deallocBlockSums(tempResult *tR, int _CPU_GPU)
{
    for (int i = 0; i < tR->d_numLevelsAllocated; i++)
    {
		CL_FREE(tR->d_scanBlockSums[i]);
    }

    free((void**)tR->d_scanBlockSums);
    tR->d_numLevelsAllocated = 0;
}

inline int agg_max(cl_mem d_Rin, int rLen, cl_mem d_Rout, int numThread, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,tempResult *tR, int _CPU_GPU)
{	
	return reduceImpl( d_Rin, rLen, d_Rout, REDUCE_MAX, numThread, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,tR,_CPU_GPU);
}
inline int agg_min(cl_mem d_Rin, int rLen, cl_mem d_Rout, int numThread, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,tempResult *tR, int _CPU_GPU)
{
	return reduceImpl( d_Rin, rLen, d_Rout, REDUCE_MIN, numThread, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,tR,_CPU_GPU);
}
inline int agg_sum(cl_mem d_Rin, int rLen, cl_mem d_Rout, int numThread, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,tempResult *tR, int _CPU_GPU)
{	
	return reduceImpl( d_Rin, rLen, d_Rout, REDUCE_SUM, numThread, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,tR,_CPU_GPU );
}
inline int agg_avg(cl_mem d_Rin, int rLen, cl_mem d_Rout, int numThread, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden ,tempResult*tR,int _CPU_GPU)
{
	return reduceImpl( d_Rin, rLen, d_Rout, REDUCE_AVERAGE, numThread, numBlock,index,eventList,kernel,Flag_CPU_GPU,burden,tR,_CPU_GPU);
}

int CL_AggMaxOnly( cl_mem d_Rin, int rLen, cl_mem* d_Rout,
													  int numThread, int numBlock , int _CPU_GPU)
{
	tempResult *tR;
	tR=(tempResult*)malloc(sizeof(tempResult));	
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CL_MALLOC(d_Rout,sizeof(Record));
	result=agg_max( d_Rin, rLen, *d_Rout, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,tR,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	HOST_FREE(tR);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("CL_AggMaxFinish\n");
	return result;
}
int CL_AggSumOnly( cl_mem d_Rin, int rLen, cl_mem* d_Rout,
													  int numThread, int numBlock , int _CPU_GPU)
{
	tempResult *tR;
	tR=(tempResult*)malloc(sizeof(tempResult));	
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CL_MALLOC(d_Rout,sizeof(Record));
	result=agg_sum( d_Rin, rLen, *d_Rout, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,tR,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	HOST_FREE(tR);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("CL_AggMaxFinish\n");
	return result;
}
int CL_AggAvgOnly( cl_mem d_Rin, int rLen, cl_mem* d_Rout,
													  int numThread, int numBlock , int _CPU_GPU)
{
	tempResult *tR;
	tR=(tempResult*)malloc(sizeof(tempResult));	
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CL_MALLOC(d_Rout,sizeof(Record));
	result=agg_avg( d_Rin, rLen, *d_Rout, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,tR,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	HOST_FREE(tR);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("CL_AggMaxFinish\n");
	return result;
}
int CL_AggMinOnly( cl_mem d_Rin, int rLen, cl_mem* d_Rout,
													  int numThread, int numBlock , int _CPU_GPU)
{
	tempResult *tR;
	tR=(tempResult*)malloc(sizeof(tempResult));	
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CL_MALLOC(d_Rout,sizeof(Record));
	result=agg_min( d_Rin, rLen, *d_Rout, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,tR,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	HOST_FREE(tR);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("CL_AggMaxFinish\n");
	return result;
}

int CL_AggMax( Record * h_Rin, int rLen, Record** h_Rout,
													  int numThread, int numBlock , int _CPU_GPU)
{
	tempResult *tR;
	tR=(tempResult*)malloc(sizeof(tempResult));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;
	int memSize = sizeof(Record)*rLen;	
	cl_mem d_Rin=NULL;
	cl_mem d_Rout=NULL;
	CL_MALLOC( &d_Rin, memSize );
	CL_MALLOC( &d_Rout, memSize );
	cl_writebuffer( d_Rin, h_Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	result=agg_max( d_Rin, rLen, d_Rout, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,tR,_CPU_GPU);
	*h_Rout = (Record*)malloc( sizeof(Record)*result );
	cl_readbuffer( h_Rout, d_Rout, sizeof(Record)*result,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_Rout );
	HOST_FREE(tR);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("CL_AggMaxFinish\n");
	return result;
}
int CL_AggMin(Record * h_Rin, int rLen, Record** h_Rout,
													  int numThread , int numBlock , int _CPU_GPU  )
{
		tempResult *tR;
	tR=(tempResult*)malloc(sizeof(tempResult));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;
	int memSize = sizeof(Record)*rLen;	
	cl_mem d_Rin=NULL;
	cl_mem d_Rout=NULL;

	CL_MALLOC( &d_Rin, memSize );
	CL_MALLOC( &d_Rout, memSize );
	cl_writebuffer( d_Rin, h_Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	result=agg_min( d_Rin, rLen, d_Rout, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,tR,_CPU_GPU);
	*h_Rout = (Record*)malloc( sizeof(Record)*result );
	cl_readbuffer( h_Rout, d_Rout, sizeof(Record)*result,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_Rout );
	HOST_FREE(tR);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("CL_AggMinFinish\n");
	return result;
}
int CL_AggSum( Record * h_Rin, int rLen, Record** h_Rout,
													  int numThread, int numBlock , int _CPU_GPU)
{
	tempResult *tR;
	tR=(tempResult*)malloc(sizeof(tempResult));
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;
	int memSize = sizeof(Record)*rLen;	
	cl_mem d_Rin=NULL;
	cl_mem d_Rout=NULL;

	CL_MALLOC( &d_Rin, memSize );
	CL_MALLOC( &d_Rout, memSize );
	cl_writebuffer( d_Rin, h_Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	result=agg_sum( d_Rin, rLen, d_Rout, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,tR,_CPU_GPU);
	*h_Rout = (Record*)malloc( sizeof(Record)*result );
	cl_readbuffer( h_Rout, d_Rout, sizeof(Record)*result,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_Rout );
	HOST_FREE(tR);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("CL_AggSumFinish\n");
	return result;
}

int CL_AggAvg( Record * h_Rin, int rLen, Record** h_Rout,
													  int numThread , int numBlock  , int _CPU_GPU )
{	
		tempResult *tR;
	tR=(tempResult*)malloc(sizeof(tempResult));
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int result=0;
	int memSize = sizeof(Record)*rLen;	
	cl_mem d_Rin=NULL;
	cl_mem d_Rout=NULL;

	CL_MALLOC( &d_Rin, memSize );
	CL_MALLOC( &d_Rout, memSize );
	cl_writebuffer( d_Rin, h_Rin, memSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	result=agg_avg( d_Rin, rLen, d_Rout, numThread, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,tR,_CPU_GPU);
	*h_Rout = (Record*)malloc( sizeof(Record)*result );
	cl_readbuffer( h_Rout, d_Rout, sizeof(Record)*result,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_Rout );
	HOST_FREE(tR);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("CL_AggAvgFinish\n");
	return result;
}
