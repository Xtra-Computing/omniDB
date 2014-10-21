#include "CoProcessor.h"
#include "MyThreadPoolCop.h"
#include "CPU_Dll.h"
#include "GPU_Dll.h"
#include <vector>
using namespace std;

DWORD WINAPI tp_hj( LPVOID lpParam ) 
{ 
	ws_hj* pData;
	pData = (ws_hj*)lpParam;
	Record* R=pData->R;
	int rLen=pData->rLen;
	Record* S=pData->S;
	int sLen=pData->sLen;
	int* RStartHist=pData->RStartHist;
	int* SStartHist=pData->SStartHist;
	int numPartition=pData->numPartition;
	HANDLE dispatchMutex=pData->dispatchMutex;
	HANDLE mergeMutex=pData->mergeMutex;
	vector<Record**>* tempResultVec=pData->tempResultVec;
	vector<int>* tempSizeVec=pData->tempSizeVec;
	EXEC_MODE execMode=pData->execMode;
	int RfromPos, RtoPos;
	int SfromPos, StoPos;
	int partToBeProcessed;
	int R_realBlockSize;
	int S_realBlockSize;
	int numResult=0;
	while(1)
	{
		Record *tempRout;
		//get the tuples to sort
		WaitForSingleObject( dispatchMutex, INFINITE );
		partToBeProcessed=*(pData->curPartition);
		if(partToBeProcessed>=numPartition) 
		{
			ReleaseMutex( dispatchMutex );
			break;
		}
		(*(pData->curPartition))++;
		ReleaseMutex( dispatchMutex );
		//partition for R.
		RfromPos=RStartHist[partToBeProcessed];
		if((partToBeProcessed+1)==numPartition)
			RtoPos=rLen;
		else
			RtoPos=RStartHist[partToBeProcessed+1];
		R_realBlockSize=RtoPos-RfromPos;
		if(R_realBlockSize==0) continue;

		//partition for S.
		SfromPos=SStartHist[partToBeProcessed];
		if((partToBeProcessed+1)==numPartition)
			StoPos=sLen;
		else
			StoPos=SStartHist[partToBeProcessed+1];
		S_realBlockSize=StoPos-SfromPos;
		if(S_realBlockSize==0) continue;

		if(execMode==EXEC_CPU)
		{
			clock_t tt=0;
			startTimer(tt);
			ON_CPU("tp_hj");
			cout<<"CPU: "<<R_realBlockSize<<endl;
			numResult=CPU_hj(R+RfromPos,R_realBlockSize,S+SfromPos, S_realBlockSize,&tempRout,OMP_JOIN_NUM_THREAD);
			ON_CPU_DONE("tp_hj");
			endTimer("CPU_hj",tt);
		}
		else//on the GPU
		{
			clock_t tt=0;
			startTimer(tt);
			ON_GPU("tp_hj");
			cout<<"GPU: "<<R_realBlockSize<<endl;
			numResult=GPUCopy_hj(R+RfromPos,R_realBlockSize,S+SfromPos, S_realBlockSize,&tempRout);
			ON_GPU_DONE("tp_hj");
			endTimer("GPUCopy_hj",tt);
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


int CO_hj(Record *R, int rLen, Record *S, int sLen, Record** Rout)
{
	int resultSize=0;
	HANDLE dispatchMutex = CreateMutex( NULL, FALSE, NULL );  
	HANDLE mergeMutex = CreateMutex( NULL, FALSE, NULL ); 
	MyThreadPoolCop *pool=(MyThreadPoolCop*)malloc(sizeof(MyThreadPoolCop));
	int numThread=2;//one for CPU and one for GPU.
	pool->create(numThread);
	int i=0;
	ws_hj** pData=(ws_hj**)malloc(sizeof(ws_hj*)*numThread);
	int curP=0;
	vector<Record**>* tempResultVec=new vector<Record**>();
	vector<int>* tempSizeVec=new vector<int>();
	//partition R on the CPU, and partition S on the GPU. Partition it into four partitions.
	int numPartition=2;
	int*RStartHist= new int[numPartition];
	int*SStartHist= new int[numPartition];
	Record *RTempOut=new Record[rLen];
	Record *STempOut=new Record[sLen];
	clock_t tt=0;
	startTimer(tt);
	//if(rLen<sLen)
	{
		ON_GPU("partition R in hj");
		GPUCopy_Partition(R,rLen,numPartition, RTempOut,RStartHist);
		//CPU_Partition(R,rLen,numPartition, RTempOut,RStartHist,1);
		ON_GPU_DONE("partition R in hj");
		ON_CPU("partition S in hj");
		GPUCopy_Partition(S,sLen,numPartition, STempOut,SStartHist);
		//CPU_Partition(S,sLen,numPartition, STempOut,SStartHist,1);
		ON_CPU_DONE("partition S in hj");
	}
	/*else
	{
		ON_GPU("partition S in hj");
		GPUCopy_Partition(R,rLen,numPartition, RTempOut,RStartHist);
		ON_GPU_DONE("partition S in hj");
		ON_CPU("partition R in hj");
		GPUCopy_Partition(S,sLen,numPartition, STempOut,SStartHist);
		ON_CPU_DONE("partition R in hj");
	}*/
	endTimer("partition",tt);

	for( i=0; i<numThread; i++ )
	{
		// Allocate memory for thread data.
		pData[i] = (ws_hj*) HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, sizeof(ws_hj));

		if( pData[i]  == NULL )
			ExitProcess(2);

		// as far as we know if sorting 16M tuples on the GPU is as fast 
		// as sorting 8M tuples on the GPU.
		if(i==0)//GPU first.
		{
			pData[i]->init(RTempOut,rLen,STempOut, sLen, RStartHist,SStartHist,numPartition, dispatchMutex,mergeMutex,&curP,EXEC_GPU,
				tempResultVec,tempSizeVec);
		}
		else
		{
			pData[i]->init(RTempOut,rLen,STempOut, sLen,RStartHist,SStartHist,numPartition,dispatchMutex,mergeMutex,&curP,EXEC_CPU,
				tempResultVec,tempSizeVec);			
		}

		pool->assignParameter(i, pData[i]);
		pool->assignTask(i, tp_hj);
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
	delete RTempOut;
	delete STempOut;
	return resultSize;
}
