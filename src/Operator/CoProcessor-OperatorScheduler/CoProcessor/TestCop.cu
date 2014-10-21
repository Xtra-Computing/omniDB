#ifndef TEST_COP_CU
#define TEST_COP_CU

#include "TestAll.cu"
#include "MyThreadPool.cu"

struct ws_sort
{
	Record *Rout;
	int startID;
	int endID;
	int ID;
};

HANDLE  gpuInUseMutex;                 


int compareInt (const void * a, const void * b)
{
  return ( ((int*)a) - ((int*)b));
}


DWORD WINAPI tp_sort( LPVOID lpParam ) 
{ 
	WaitForSingleObject( gpuInUseMutex, INFINITE );

	ws_sort* pData;
	pData = (ws_sort*)lpParam;
	int ID=pData->ID;
	if(ID%2==0)
	{
		printf("gpu sorted \n");
		testSort(16*1024*1024);	
		printf("gpu eneded\n");
	}
	else
	{
		printf("cpu sorted \n");
		int len=16*1024*1024;
		int *data=(int*)malloc(sizeof(int)*len);
		int i=0;
		for(i=0;i<len;i++)
			data[i]=rand();
		qsort(data,len,sizeof(int),compareInt);
		printf("cpu ended \n");
	}
	ReleaseMutex( gpuInUseMutex );

	return 0;
} 

void testCoProcessing(int argc, char** argv)
{
	gpuInUseMutex = CreateMutex( NULL, FALSE, NULL );  // Cleared 


	MyThreadPool *pool=(MyThreadPool*)malloc(sizeof(MyThreadPool));
	int numThread=4;
	pool->create(numThread);
	int i=0;
	ws_sort** pData=(ws_sort**)malloc(sizeof(ws_sort*)*numThread);
	for( i=0; i<numThread; i++ )
	{
		// Allocate memory for thread data.
		pData[i] = (ws_sort*) HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, sizeof(ws_sort));

		if( pData[i]  == NULL )
			ExitProcess(2);

		// Generate unique data for each thread.
		pData[i]->ID=i;
		pool->assignParameter(i, pData[i]);
		pool->assignTask(i, tp_sort);
	}
	pool->run();
	for(i=0;i<numThread;i++)
		HeapFree(GetProcessHeap(),0, pData[i]);
	free(pData);
	pool->destory();
	printf("co-processing okay");
	CloseHandle( gpuInUseMutex );

	
}

#endif