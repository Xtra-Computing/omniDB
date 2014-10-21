#include "CoProcessor.h"
#include "MyThreadPoolCop.h"
#include "CPU_Dll.h"
#include <vector>
using namespace std;

#define CPU_NINLJ_RATIO (0.03)  

DWORD WINAPI tp_ninlj( LPVOID lpParam ) 
{ 
	ws_coninlj* pData;
	pData = (ws_coninlj*)lpParam;
	Record* R=pData->R;
	int rLen=pData->rLen;
	Record* S=pData->S;
	int sLen=pData->sLen;
	int blockSizeOnR=pData->blockSizeOnR;
	HANDLE dispatchMutex=pData->dispatchMutex;
	HANDLE mergeMutex=pData->mergeMutex;
	vector<Record**>* tempResultVec=pData->tempResultVec;
	vector<int>* tempSizeVec=pData->tempSizeVec;
	EXEC_MODE execMode=pData->execMode;
	int fromPos;
	int toPos;
	int realBlockSize;
	int numResult=0;
	if(execMode!=EXEC_CPU)
	{
		setHighPriority();
#ifdef FIXED_CORE_TO_GPU
		set_selfCPUID(3);
		cout<<"set the GPU thread ID to core 4"<<endl;
#endif
	}
	while(1)
	{
		Record *tempRout;
		//get the tuples to sort
		WaitForSingleObject( dispatchMutex, INFINITE );
		fromPos=*(pData->curPosToJoin);
		if(fromPos>=rLen) 
		{
			ReleaseMutex( dispatchMutex );
			break;
		}
		toPos=fromPos+blockSizeOnR;
		if(toPos>=rLen)
			toPos=rLen;
		*(pData->curPosToJoin)=toPos;
		ReleaseMutex( dispatchMutex );
		realBlockSize=toPos-fromPos;
		//sort it
		if(execMode==EXEC_CPU)
		{
			ON_CPU("tp_ninlj");
			clock_t tt=0;
			startTimer(tt);
			cout<<"on CPU: "<<fromPos<<", "<<realBlockSize<<endl;
			if(realBlockSize>0)
			numResult=CPU_ninlj(R+fromPos,realBlockSize,S, sLen,&tempRout,OMP_JOIN_NUM_THREAD);
			endTimer("CPU ninlj", tt);
		}
		else//on the GPU
		{
			ON_GPU("tp_ninlj");
			clock_t tt=0;
			startTimer(tt);
			if(realBlockSize>0)
			numResult=GPUCopy_ninlj(R+fromPos,realBlockSize,S, sLen,&tempRout);
			endTimer("GPU ninlj", tt);
		}

		WaitForSingleObject( mergeMutex, INFINITE );
		if(numResult!=0)
		{
			tempResultVec->push_back(&tempRout);		
			tempSizeVec->push_back(numResult);
		}
		ReleaseMutex( mergeMutex );
	}

	return 0;
} 

int MergeJoinResult(vector<Record**>* tempResultVec, vector<int>* tempSizeVec, Record** Rout)
{
	int resultSize=0;
	int numElementInVector=tempSizeVec->size();
	int* numEleArray=new int[numElementInVector];
	int i=0;
	for(i=0;i<numElementInVector;i++)
	{
		numEleArray[i]=(*tempSizeVec)[i];
		resultSize+=numEleArray[i];
	}
	*Rout=new Record[resultSize];
	int cur=0;
	for(i=0;i<numElementInVector;i++)
	{
		//memcpy((*Rout)+cur,*((*tempResultVec)[i]),numEleArray[i]*sizeof(Record));
		cur+=numEleArray[i];		
	}
	delete numEleArray;
	return resultSize;
}

/*
we first cut on the larger relation assuming the smaller one fits into the device memory.
Otherwise, we further cut the smaller one into multiple chunks. (not implemented).
*/


int CO_ninlj(Record *R, int rLen, Record *S, int sLen, Record** Rout)
{
	int resultSize=0;
	HANDLE dispatchMutex = CreateMutex( NULL, FALSE, NULL );  
	HANDLE mergeMutex = CreateMutex( NULL, FALSE, NULL ); 
	MyThreadPoolCop *pool=(MyThreadPoolCop*)malloc(sizeof(MyThreadPoolCop));
	int numThread=2;//one for CPU and one for GPU.
	pool->create(numThread);
	int i=0;
	ws_coninlj** pData=(ws_coninlj**)malloc(sizeof(ws_coninlj*)*numThread);
	int curP=0;
	vector<Record**>* tempResultVec=new vector<Record**>();
	vector<int>* tempSizeVec=new vector<int>();
	int cpuBlockSize=CPU_NINLJ_BLOCKSIZE;
	int gpuBlockSize=GPU_NINLJ_BLOCKSIZE;
	if(rLen<24*1024*1024)
	{
		cpuBlockSize=(int)((double)rLen*CPU_NINLJ_RATIO);
		gpuBlockSize=rLen-cpuBlockSize;
	}
	__DEBUGInt__((int)(CPU_NINLJ_RATIO*(double)100.0));
	cout<<"total: "<<rLen<<"->"<<cpuBlockSize<<", "<<gpuBlockSize<<endl;
	for( i=0; i<numThread; i++ )
	{
		// Allocate memory for thread data.
		pData[i] = (ws_coninlj*) HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, sizeof(ws_coninlj));

		if( pData[i]  == NULL )
			ExitProcess(2);

		// as far as we know if sorting 16M tuples on the GPU is as fast 
		// as sorting 8M tuples on the GPU.
		if(i==0)//GPU first.
		{
			pData[i]->init(R,rLen,gpuBlockSize,S, sLen, dispatchMutex,mergeMutex,&curP,EXEC_GPU,
				tempResultVec,tempSizeVec);
			/*ON_GPU("tp_ninlj2");
			clock_t tt=0;
			Record *tempRout=NULL;
			startTimer(tt);
			int numResult=GPUCopy_ninlj(R,gpuBlockSize,S, sLen,&tempRout);
			endTimer("GPU ninlj2", tt);*/
		}
		else
		{
			pData[i]->init(R,rLen,cpuBlockSize,S, sLen, dispatchMutex,mergeMutex,&curP,EXEC_CPU,
				tempResultVec,tempSizeVec);			
		}

		pool->assignParameter(i, pData[i]);
		pool->assignTask(i, tp_ninlj);
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
	resultSize=MergeJoinResult(tempResultVec,tempSizeVec,Rout);
	
	delete tempResultVec;
	delete tempSizeVec;
	return resultSize;
}