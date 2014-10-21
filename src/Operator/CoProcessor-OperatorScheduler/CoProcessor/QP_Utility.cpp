#include "QP_Utility.h"
#include <stdlib.h>
#include <SDKCommon.hpp>
#include <SDKFile.hpp>
#include <SDKCommon.hpp>
#include <SDKApplication.hpp>
#include "CL\cl.h"
#include "OpenCL_DLL.h"
#include <StringLib.h>
void OP_bufferchecking(cl_mem R_in,size_t size){
	printf("Host buffer checking..size is %d",size);
	void * R_out;
	R_out=(Record*)malloc(sizeof(Record)*size);
	CopyGPUToCPU(R_in,(void*)R_out,size);
	free(R_out);
}
int OP_compare (const void * a, const void * b)
{
  return ( ((int2*)a)->y - ((int2*)b)->y );
}

void generateSort(Record *R, int maxmax, int OP_rLen, int seed)
{
	int i=0;
	const int offset=(1<<15)-1;
	srand(seed);
	for(i=0;i<OP_rLen;i++)
	{
		R[i].value=i;//((((rand()& offset)<<15)+(rand()&1))+(rand()<<1)+(rand()&1))%maxmax;
		
	}
	qsort(R,OP_rLen,sizeof(int2),OP_compare);
	for(i=0;i<OP_rLen;i++)
	R[i].rid=i;

}
void generateRand(Record *R, int max, int OP_rLen, int seed)
{
	int i=0;
	const int offset=(1<<15)-1;
	srand(seed);
	for(i=0;i<OP_rLen;i++)
	{
		R[i].value=((((rand()& offset)<<15)+(rand()&1))+(rand()<<1)+(rand()&1))%max;
		//R[i].x=i+1;
		R[i].rid=i;
		////printf("y is %d, x is %d",R[i].y,R[i].x);
	}

}


streamsdk::SDKCommon *sampleCommon=new streamsdk::SDKCommon();
int genTimer(int timerID)
{
#ifdef NV
	shrDeltaT(timerID);
	return timerID;
#else
	int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer); 
	return timer;
#endif
	
}

double getTimer(int timer)
{
	/*With Kernel Scheduler*/
//	cl_int err1=clFinish(CommandQueue[0]);
//	cl_int err2=clFinish(CommandQueue[1]);
//	printf("getTimer calls finish commandqueue first");
#ifdef NV	
	double elapsedTimeInSec = shrDeltaT(timer);

	return elapsedTimeInSec;
#else
	 sampleCommon->stopTimer(timer);
	 double writeTime = (double)(sampleCommon->readTimer(timer));
	 sampleCommon->resetTimer(timer);
	 sampleCommon->startTimer(timer); 
	 return writeTime;
#endif
}