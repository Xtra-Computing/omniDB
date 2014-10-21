#ifndef THREAD_POOL_COP_H
#define THREAD_POOL_COP_H

//multiple threads for co-processing.


#include "windows.h"
#define NUM_CORE_CPU 4
#define MAX_RUN ((1<<(NUM_CORE_CPU+1))-1)
struct threadPar{
	int threadid;
	void init(int pthreadid)
	{		
		threadid=pthreadid;
	}
};
struct MyThreadPoolCop
{
public:
	/*void create()
	{
		dwThreadId=NULL;
		hThread=NULL;
		masks=NULL;
		tParam=NULL;
		numThread=0;
	}*/
	void destory()
	{
		if(dwThreadId!=NULL)
			free(dwThreadId);
		if(hThread!=NULL)
			free(hThread);
		if(masks!=NULL)
			free(masks);
		if(tParam!=NULL)
			free(tParam);
		if(pfnThreadProc!=NULL)
			free(pfnThreadProc);
	}
	// the number of threads
	int numThread;
	// //the mask assigning threads to which core/processors
	int* masks;
	DWORD *dwThreadId;
	HANDLE *hThread;
	void ** tParam;
	LPTHREAD_START_ROUTINE* pfnThreadProc;
//functions.
	int setCPUID(int tid, int cpuid)
	{
		int maskValue=(1<<cpuid)%MAX_RUN;
		masks[tid]=maskValue;
		return 0;
	}

	int assignTask(int tid, LPTHREAD_START_ROUTINE  pfn)
	{
		pfnThreadProc[tid]=pfn;
		return 0;
	}

	int assignParameter(int tid, void* pvParam)
	{
		tParam[tid]=pvParam;
		return 0;
	}

	int run(void)
	{
		int i=0;
		for( i=0; i<numThread; i++ )
		{
			hThread[i] = CreateThread( 
				NULL,              // default security attributes
				0,                 // use default stack size  
				pfnThreadProc[i],        // thread function 
				tParam[i],             // argument to thread function 
				0,                 // use default creation flags 
				&dwThreadId[i]);   // returns the thread identifier 
	 
			// Check the return value for success. 
			SetThreadAffinityMask(hThread[i],masks[i]);
	 
			if (hThread[i] == NULL) 
			{
				ExitProcess(i);
			}
		}
		// Wait until all threads have terminated.

		WaitForMultipleObjects(numThread, hThread, TRUE, INFINITE);
		// Close all thread handles upon completion.
		for(i=0; i<numThread; i++)
		{
			CloseHandle(hThread[i]);
		}
		return 0;
	}

	void create(int nThead)
	{
		numThread=nThead;
		dwThreadId=(DWORD*)malloc(sizeof(DWORD)*numThread);
		hThread=(HANDLE*)malloc(sizeof(HANDLE)*numThread);
		masks=(int*)malloc(sizeof(int)*numThread);
		int i=0;
		for(i=0;i<numThread;i++)
			masks[i]=(1<<i)%MAX_RUN;
		tParam=(void**)malloc(sizeof(void*)*numThread);
		pfnThreadProc=(LPTHREAD_START_ROUTINE*)malloc(sizeof(LPTHREAD_START_ROUTINE)*numThread);
	}
};

#endif

