#include "common.h"
#include "testAggAfterGB.h"
#include "OpenCL_DLL.h"
#include "scheduler.h"
/*
aggregation after group by.
with the known number of groups, we can allocate the output for advance: d_aggResults.
*/
void agg_max_afterGroupBy(cl_mem d_Rin, int rLen, cl_mem  d_startPos, int numGroups, cl_mem  d_Ragg, cl_mem  d_aggResults, int numThread,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	aggAfterGroupByImpl(d_Rin, rLen, d_startPos, numGroups, d_Ragg, d_aggResults, REDUCE_MAX, numThread, index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

void agg_min_afterGroupBy(cl_mem d_Rin, int rLen, cl_mem  d_startPos, int numGroups, cl_mem  d_Ragg, cl_mem  d_aggResults, int numThread,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	aggAfterGroupByImpl(d_Rin, rLen, d_startPos, numGroups, d_Ragg, d_aggResults, REDUCE_MIN, numThread,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

void agg_sum_afterGroupBy(cl_mem d_Rin, int rLen, cl_mem  d_startPos, int numGroups, cl_mem  d_Ragg, cl_mem  d_aggResults, int numThread,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	aggAfterGroupByImpl(d_Rin, rLen, d_startPos, numGroups, d_Ragg, d_aggResults, REDUCE_SUM, numThread,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}

void agg_avg_afterGroupBy(cl_mem d_Rin, int rLen, cl_mem  d_startPos, int numGroups, cl_mem  d_Ragg, cl_mem  d_aggResults, int numThread,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	aggAfterGroupByImpl(d_Rin, rLen, d_startPos, numGroups, d_Ragg, d_aggResults, REDUCE_AVERAGE, numThread,index,eventList,kernel,Flag_CPU_GPU,burden,_CPU_GPU);
}


extern "C" void CL_agg_max_afterGroupBy(Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, int numThread,int _CPU_GPU ) 
{
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	cl_mem  d_Rin;
	cl_mem  d_startPos;
	cl_mem  d_Ragg;
	cl_mem  d_aggResults;

	CL_MALLOC( &d_Rin, sizeof(Record)*rLen );
	CL_MALLOC( &d_startPos, sizeof(int)*numGroups );
	CL_MALLOC(&d_Ragg, sizeof(Record)*rLen );
	CL_MALLOC(&d_aggResults, sizeof(int)*numGroups );

	cl_writebuffer( d_Rin, h_Rin, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_startPos, h_startPos, sizeof(int)*numGroups,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_Ragg, h_Ragg, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );

	agg_max_afterGroupBy(d_Rin, rLen, d_startPos, numGroups, d_Ragg, d_aggResults, numThread,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	cl_readbuffer( h_aggResults, d_aggResults, sizeof(int)*numGroups,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_startPos );
	CL_FREE( d_Ragg );
	CL_FREE( d_aggResults );
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
}
extern "C" void CL_agg_min_afterGroupBy(Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, int numThread,int _CPU_GPU ) 
{
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	cl_mem  d_Rin;
	cl_mem  d_startPos;
	cl_mem  d_Ragg;
	cl_mem  d_aggResults;

	CL_MALLOC( &d_Rin, sizeof(Record)*rLen );
	CL_MALLOC( &d_startPos, sizeof(int)*numGroups );
	CL_MALLOC(&d_Ragg, sizeof(Record)*rLen );
	CL_MALLOC(&d_aggResults, sizeof(int)*numGroups );

	cl_writebuffer( d_Rin, h_Rin, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_startPos, h_startPos, sizeof(int)*numGroups,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_Ragg, h_Ragg, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );

	agg_min_afterGroupBy(d_Rin, rLen, d_startPos, numGroups, d_Ragg, d_aggResults, numThread,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	cl_readbuffer( h_aggResults, d_aggResults, sizeof(int)*numGroups,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_startPos );
	CL_FREE( d_Ragg );
	CL_FREE( d_aggResults );
	clReleaseKernel(Kernel); 
	
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
}
extern "C" void CL_agg_sum_afterGroupBy(Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, int numThread,int _CPU_GPU ) 
{
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	cl_mem  d_Rin;
	cl_mem  d_startPos;
	cl_mem  d_Ragg;
	cl_mem  d_aggResults;

	CL_MALLOC( &d_Rin, sizeof(Record)*rLen );
	CL_MALLOC( &d_startPos, sizeof(int)*numGroups );
	CL_MALLOC(&d_Ragg, sizeof(Record)*rLen );
	CL_MALLOC(&d_aggResults, sizeof(int)*numGroups );

	cl_writebuffer( d_Rin, h_Rin, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_startPos, h_startPos, sizeof(int)*numGroups,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_Ragg, h_Ragg, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );

	agg_sum_afterGroupBy(d_Rin, rLen, d_startPos, numGroups, d_Ragg, d_aggResults, numThread,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	cl_readbuffer( h_aggResults, d_aggResults, sizeof(int)*numGroups,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_startPos );
	CL_FREE( d_Ragg );
	CL_FREE( d_aggResults );
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
}
extern "C" void CL_agg_avg_afterGroupBy(Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, int numThread,int _CPU_GPU ) 
{
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	cl_mem  d_Rin;
	cl_mem  d_startPos;
	cl_mem  d_Ragg;
	cl_mem  d_aggResults;

	CL_MALLOC( &d_Rin, sizeof(Record)*rLen );
	CL_MALLOC( &d_startPos, sizeof(int)*numGroups );
	CL_MALLOC(&d_Ragg, sizeof(Record)*rLen );
	CL_MALLOC(&d_aggResults, sizeof(int)*numGroups );

	cl_writebuffer( d_Rin, h_Rin, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_startPos, h_startPos, sizeof(int)*numGroups,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	cl_writebuffer( d_Ragg, h_Ragg, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );

	agg_avg_afterGroupBy(d_Rin, rLen, d_startPos, numGroups, d_Ragg, d_aggResults, numThread,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	cl_readbuffer( h_aggResults, d_aggResults, sizeof(int)*numGroups,&index,eventList,&CPU_GPU,&burden,_CPU_GPU );
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE( d_Rin );
	CL_FREE( d_startPos );
	CL_FREE( d_Ragg );
	CL_FREE( d_aggResults );
	clReleaseKernel(Kernel); 
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
}