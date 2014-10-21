#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "stdafx.h"
#include "windows.h"

#define NUM_CORE_CPU 4
#define MAX_RUN ((1<<NUM_CORE_CPU)-1)

class MyThreadPool
{
public:
	MyThreadPool(void);
	~MyThreadPool(void);
	// the number of threads
	int numThread;
	// //the mask assigning threads to which core/processors
	int* masks;
	DWORD *dwThreadId;
	HANDLE *hThread;
	void ** tParam;
	int setMask(int tid, int maskValue);
	int assignTask(LPTHREAD_START_ROUTINE  pfnThreadProc);
	int assignParameter(int tid, void* pvParam);
	LPTHREAD_START_ROUTINE pfnThreadProc;
	int run(void);
	int init(int nThead, LPTHREAD_START_ROUTINE  pfn);
};

#endif

