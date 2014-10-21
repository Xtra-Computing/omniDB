#include "CoProcessor.h"
#include "MyThreadPoolCop.h"
#include "CPU_Dll.h"
#include "GPU_Dll.h"
#include <vector>
using namespace std;



#define CPU_SORT_RATIO (0.0)//(0.24)  

DWORD WINAPI tp_sort( LPVOID lpParam ) 
{ 
	ws_cosort* pData;
	pData = (ws_cosort*)lpParam;
	Record* R=pData->R;
	int rLen=pData->rLen;
	Record *Rout=pData->Rout;
	int blockSize=pData->blockSize;
	HANDLE dispatchMutex=pData->dispatchMutex;
	HANDLE mergeMutex=pData->mergeMutex;
	vector<int>* tempSizeVec=pData->tempSizeVec;
	EXEC_MODE execMode=pData->execMode;
	int fromPos;
	int toPos;
	int realBlockSize;
	Record *tempRout;
	if(execMode!=EXEC_CPU)
	{
		setHighPriority();
		set_selfCPUID(3);
		cout<<"set the GPU thread ID to core 4"<<endl;
	}
	while(1)
	{
		//get the tuples to sort
		WaitForSingleObject( dispatchMutex, INFINITE );
		fromPos=*(pData->curPosToSort);
		if(fromPos>=rLen) 
		{
			ReleaseMutex( dispatchMutex );
			break;
		}
		toPos=fromPos+blockSize;
		if(toPos>=rLen)
			toPos=rLen;
		*(pData->curPosToSort)=toPos;
		ReleaseMutex( dispatchMutex );
		realBlockSize=toPos-fromPos;
		//sort it
		tempRout=new Record[realBlockSize];
		if(execMode==EXEC_CPU)
		{
			clock_t tt=0;
			startTimer(tt);
			ON_CPU("tp_sort");
			cout<<fromPos<<", "<<realBlockSize<<endl;
			CPU_Sort(R+fromPos,realBlockSize,tempRout,OMP_SORT_NUM_THREAD);
			ON_CPU_DONE("tp_sort");
			endTimer("CPU_Sort",tt);

		}
		else//on the GPU
		{
			clock_t tt=0;
			startTimer(tt);
			ON_GPU("tp_sort");
			cout<<fromPos<<", "<<realBlockSize<<endl;
			//GPUCopy_bitonicSort(R+fromPos,realBlockSize,tempRout);
			GPUCopy_QuickSort(R+fromPos,realBlockSize,tempRout);
			ON_GPU_DONE("tp_sort");
			endTimer("GPUCopy_QuickSort",tt);
		}

		WaitForSingleObject( mergeMutex, INFINITE );
		//tempResultVec->push_back(&tempRout);	
		int curSize=0;
		for(int i=0;i<tempSizeVec->size();i++)
			curSize+=(*tempSizeVec)[i];
		memcpy(Rout+curSize,tempRout,realBlockSize*sizeof(Record));
		tempSizeVec->push_back(realBlockSize);
		ReleaseMutex( mergeMutex );
	}

	return 0;
} 


void BinaryMergeSort(Record *R, int rLen, Record* S, int sLen, Record* Rout)
{
	int pR=0;
	int pS=0;
	int cur=0;
	while(pR<rLen && pS<sLen)
	{
		if(R[pR].value==S[pS].value)
		{
			EVAL((Rout+cur),(R+pR));
			cur++;
			EVAL((Rout+cur),(S+pS));
			cur++;
			pS++;
			pR++;
		}
		else if(R[pR].value<S[pS].value)
		{
			EVAL((Rout+cur),(R+pR));
			cur++;
			pR++;
		}
		else
		{
			EVAL((Rout+cur),(S+pS));
			cur++;
			pS++;
		}
	}
	while(pR<rLen)
	{
		EVAL((Rout+cur),(R+pR));
		cur++;
		pR++;
	}
	while(pS<sLen)
	{
		EVAL((Rout+cur),(S+pS));
		cur++;
		pS++;
	}
	assert(cur==(rLen+sLen));
}



void CO_Sort(Record *R, int rLen, Record* Rout)
{
	if(rLen>24*1024*1024)
	{
		cout<<"we currently support at most 24 million tuples at most due the binary merge"<<endl;
		exit(0);
	}
	HANDLE dispatchMutex = CreateMutex( NULL, FALSE, NULL );  
	HANDLE mergeMutex = CreateMutex( NULL, FALSE, NULL ); 
	MyThreadPoolCop *pool=(MyThreadPoolCop*)malloc(sizeof(MyThreadPoolCop));
	int numThread=2;//one for CPU and one for GPU.
	pool->create(numThread);
	int i=0;
	ws_cosort** pData=(ws_cosort**)malloc(sizeof(ws_cosort*)*numThread);
	int curP=0;
	vector<int>* tempSizeVec=new vector<int>();
	int cpuBlockSize=CPU_SORT_BLOCKSIZE;
	int gpuBlockSize=GPU_SORT_BLOCKSIZE;
	if(rLen<24*1024*1024)
	{
		cpuBlockSize=(int)((double)rLen*CPU_SORT_RATIO);
		gpuBlockSize=rLen-cpuBlockSize;
	}
	__DEBUGInt__((int)(CPU_SORT_RATIO*(double)100.0));
	cout<<"size: "<<cpuBlockSize<<","<<gpuBlockSize<<endl;
	Record *tempRout=new Record[rLen];
	for( i=0; i<numThread; i++ )
	{
		// Allocate memory for thread data.
		pData[i] = (ws_cosort*) HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, sizeof(ws_cosort));

		if( pData[i]  == NULL )
			ExitProcess(2);

		// as far as we know if sorting 16M tuples on the GPU is as fast 
		// as sorting 8M tuples on the GPU.
		if(i==0)//GPU first.
		{
			pData[i]->init(R,rLen,gpuBlockSize,dispatchMutex,mergeMutex,&curP,EXEC_GPU,
				tempSizeVec, tempRout);
		}
		else
		{
			pData[i]->init(R,rLen,cpuBlockSize,dispatchMutex,mergeMutex,&curP,EXEC_CPU,
				tempSizeVec, tempRout);			
		}

		pool->assignParameter(i, pData[i]);
		pool->assignTask(i, tp_sort);
	}
	pool->run();
	for(i=0;i<numThread;i++)
	{
		HeapFree(GetProcessHeap(),0, pData[i]);
	}
	free(pData);
	pool->destory();
	CloseHandle( dispatchMutex );
	CloseHandle( mergeMutex);
	assert(tempSizeVec->size()==2);
	clock_t tt=0;
	startTimer(tt);
	BinaryMergeSort(tempRout,(*tempSizeVec)[0],tempRout+(*tempSizeVec)[0],(*tempSizeVec)[1],Rout);
	endTimer("Merge",tt);
	
	delete tempSizeVec;
	delete tempRout;
	printf("co-processing okay");
}