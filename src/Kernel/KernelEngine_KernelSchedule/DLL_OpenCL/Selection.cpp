#include "testFilter.h"
#include "KernelScheduler.h"
#include "OpenCL_DLL.h"
int point_selection(cl_mem d_Rin, int rLen, int matchingKeyValue, cl_mem* d_Rout, 
					int numThreadPB, int numBlock , int *index,cl_event *eventList,cl_kernel* Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int* outSize = (int*)malloc( sizeof(int) );
	int beginPos = 0;

	filterImpl( d_Rin, beginPos, rLen, d_Rout, outSize, 
				numThreadPB, numBlock, matchingKeyValue, matchingKeyValue,index,eventList,Kernel, Flag_CPU_GPU,burden,_CPU_GPU );

	return (*outSize);
}
extern "C" int CL_PointSelection( Record* h_Rin, int rLen, int matchingKeyValue, Record** h_Rout, 
															  int numThreadPB, int numBlock ,int _CPU_GPU)
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	cl_mem d_Rin;
	cl_mem d_Rout;

	CL_MALLOC( &d_Rin, sizeof(Record)*rLen );
	cl_writebuffer( d_Rin, h_Rin, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	int outSize = point_selection(d_Rin, rLen, matchingKeyValue, &d_Rout, numThreadPB, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	(*h_Rout) = (Record*)malloc( sizeof(Record)*outSize );
	cl_readbuffer(*h_Rout, d_Rout, sizeof(Record)*outSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);	
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	CL_FREE(d_Rin);
	CL_FREE(d_Rout);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("FilterFinish\n");
	return outSize;
}
extern "C" int CL_PointSelectionOnly(cl_mem d_Rin, int rLen, int matchingKeyValue, cl_mem* d_Rout, 
															  int numThreadPB, int numBlock,int _CPU_GPU )
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int outSize = point_selection( d_Rin, rLen, matchingKeyValue, d_Rout, 
										numThreadPB, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("FilterFinish\n");
	return outSize;
}

int range_selection(cl_mem d_Rin, int rLen, int rangeSmallKey, int rangeLargeKey, cl_mem* d_Rout,
					int numThreadPB , int numBlock , int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int* outSize = (int*)malloc( sizeof(int) );
	int beginPos = 0;

	filterImpl( d_Rin, beginPos, rLen, d_Rout, outSize, 
				numThreadPB, numBlock, rangeSmallKey, rangeLargeKey,index,eventList,Kernel,Flag_CPU_GPU,burden,_CPU_GPU );

	return (*outSize);
}


extern "C" int CL_RangeSelectionOnly(cl_mem d_Rin, int rLen, int rangeSmallKey, int rangeLargeKey, cl_mem* d_Rout, 
															  int numThreadPB, int numBlock,int _CPU_GPU )
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;
	int outSize = range_selection( d_Rin, rLen, rangeSmallKey, rangeLargeKey, d_Rout, 
										numThreadPB, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	deschedule(CPU_GPU,burden);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("FilterFinish\n");
	return outSize;
}
extern "C" int CL_RangeSelection(Record* h_Rin, int rLen, int rangeSmallKey, int rangeLargeKey, Record** h_Rout, 
															  int numThreadPB, int numBlock,int _CPU_GPU )
{
	cl_event eventList[2];
	int index=0;
	cl_kernel Kernel; 
	int CPU_GPU;
	double burden;

	cl_mem d_Rin;
	cl_mem d_Rout;//->be care d_Rout is allocate later@!
	CL_MALLOC( &d_Rin, sizeof(Record)*rLen );
	cl_writebuffer( d_Rin, h_Rin, sizeof(Record)*rLen,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);

	int outSize = range_selection( d_Rin, rLen, rangeSmallKey, rangeLargeKey, &d_Rout, 
		numThreadPB, numBlock,&index,eventList,&Kernel,&CPU_GPU,&burden,_CPU_GPU);

	(*h_Rout) = (Record*)malloc( sizeof(Record)*outSize );
	cl_readbuffer(*h_Rout, d_Rout, sizeof(Record)*outSize,&index,eventList,&CPU_GPU,&burden,_CPU_GPU);
	clWaitForEvents(1,&eventList[(index-1)%2]);
	deschedule(CPU_GPU,burden);
	CL_FREE(d_Rin);
	CL_FREE(d_Rout);
	clReleaseKernel(Kernel);  
	clReleaseEvent(eventList[0]);
	clReleaseEvent(eventList[1]);
	//printf("FilterFinish\n");
	return outSize;
}
