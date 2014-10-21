#include "CoProcessor.h"
#include "MyThreadPoolCop.h"
#include "CPU_Dll.h"
#include "GPU_Dll.h"
#include <vector>
using namespace std;

#define CPU_INLJ_RATIO (0.115) 

DWORD WINAPI tp_inlj( LPVOID lpParam ) 
{ 
	ws_coinlj* pData;
	pData = (ws_coinlj*)lpParam;
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
	CC_CSSTree *cpu_tree=pData->cpu_tree;
	CUDA_CSSTree *gpu_tree=pData->gpu_tree;
	if(execMode!=EXEC_CPU)
	{
		setHighPriority();
		set_selfCPUID(3);
		cout<<"set the GPU thread ID to core 4"<<endl;
	}
	int fromPos;
	int toPos;
	int realBlockSize;
	int numResult=0;
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
			ON_CPU("tp_inlj");
			clock_t tt=0;
			startTimer(tt);
			//cout<<"CPU: "<<fromPos<<", "<<realBlockSize<<endl;
			numResult=CPU_inlj(R,rLen,cpu_tree,S+fromPos,realBlockSize,&tempRout,OMP_JOIN_NUM_THREAD);
			endTimer("CPU inlj", tt);
			ON_CPU_DONE("tp_inlj");
			//cout<<"resultSizeCpu,"<<numResult<<endl;
		}
		else//on the GPU
		{
			ON_GPU("tp_inlj");
			clock_t tt=0;
			startTimer(tt);
			//cout<<"GPU: "<<fromPos<<", "<<realBlockSize<<endl;
			numResult=GPUCopy_inlj(R,rLen,gpu_tree,S+fromPos,realBlockSize,&tempRout);
			endTimer("GPU inlj", tt);
			ON_GPU_DONE("tp_inlj");
			//cout<<"resultSizeGPU,"<<numResult<<endl;
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


/*
we first cut on the larger relation assuming the smaller one fits into the device memory.
Otherwise, we further cut the smaller one into multiple chunks. (not implemented).
*/


int CO_inlj(Record *R, int rLen, CC_CSSTree *cT,  CUDA_CSSTree* gT, Record *S, int sLen, Record** Rout)
{
	int resultSize=0;
	HANDLE dispatchMutex = CreateMutex( NULL, FALSE, NULL );  
	HANDLE mergeMutex = CreateMutex( NULL, FALSE, NULL ); 
	MyThreadPoolCop *pool=(MyThreadPoolCop*)malloc(sizeof(MyThreadPoolCop));
	int numThread=2;//one for CPU and one for GPU.
	pool->create(numThread);
	int i=0;
	ws_coinlj** pData=(ws_coinlj**)malloc(sizeof(ws_coinlj*)*numThread);
	int curP=0;
	vector<Record**>* tempResultVec=new vector<Record**>();
	vector<int>* tempSizeVec=new vector<int>();
	int cpuBlockSize=CPU_INLJ_BLOCKSIZE;
	int gpuBlockSize=GPU_INLJ_BLOCKSIZE;
	if(sLen<24*1024*1024)
	{
		cpuBlockSize=(int)((double)sLen*CPU_INLJ_RATIO);
		gpuBlockSize=sLen-cpuBlockSize;
	}
	__DEBUGInt__((int)(CPU_INLJ_RATIO*(double)100.0));
	__DEBUGInt__(cpuBlockSize);
	__DEBUGInt__(gpuBlockSize);
	for( i=0; i<numThread; i++ )
	{
		// Allocate memory for thread data.
		pData[i] = (ws_coinlj*) HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, sizeof(ws_coinlj));

		if( pData[i]  == NULL )
			ExitProcess(2);

		// as far as we know if sorting 16M tuples on the GPU is as fast 
		// as sorting 8M tuples on the GPU.
		if(i==0)//GPU first.
		{
			pData[i]->init(R,rLen,gpuBlockSize,S, sLen, cT, gT,dispatchMutex,mergeMutex,&curP,EXEC_GPU,
				tempResultVec,tempSizeVec);
			/*ON_GPU("tp_inlj2");
			clock_t tt=0;
			Record *tempRout;
			startTimer(tt);
			cout<<"GPU: "<<0<<", "<<gpuBlockSize<<endl;
			int numResult=GPUCopy_inlj(R,rLen,gT,S,gpuBlockSize,&tempRout);
			cout<<"resultSize3,"<<numResult<<endl;
			endTimer("GPU inlj", tt);
			ON_GPU_DONE("tp_inlj2");*/
		}
		else
		{
			pData[i]->init(R,rLen,cpuBlockSize,S, sLen,cT, gT, dispatchMutex,mergeMutex,&curP,EXEC_CPU,
				tempResultVec,tempSizeVec);			
		}

		pool->assignParameter(i, pData[i]);
		pool->assignTask(i, tp_inlj);
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
	//cout<<"resultSize2,"<<resultSize<<endl;
	
	delete tempResultVec;
	delete tempSizeVec;
	return resultSize;
}