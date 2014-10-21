#include "CoProcessor.h"
#include "MyThreadPoolCop.h"
#include "CoProcessorTest.h"
#include "QueryPlanTree.h"
#include "Database.h"
#include "Helper.h"
#define CPUCORE_FORGPU 4

#define SMART_PROCESSOR 1
#define WORK_STEALING 1

#define STEALING_CHANCE 3
bool isGPUavailable=true;
double OP_LothresholdForGPUApp;
double OP_LothresholdForCPUApp;
double SpeedupGPUOverCPU_Operator[22];
double AddCPUBurden_OP[22];
double AddGPUBurden_OP[22];
double OP_UpCPUBurden;
double OP_LoCPUBurden;
double OP_UpGPUBurden;
double OP_LoGPUBurden;
double OP_CPUBurden;
double OP_GPUBurden;
extern CRITICAL_SECTION OP_GPUBurdenCS;
extern CRITICAL_SECTION OP_CPUBurdenCS;
extern FILE *OP_ofp;
#define Greedy
//#define Balance
int pickQuerySmart(int threadid, Query_stat *gQstat, int numQuery)
{
	int i=0;
	int result=-1;
	/*pick them up senqutially*/
	for(i=0;i<numQuery;i++)
		if(gQstat[i].isAssigned==false)
		{
			result=i;
			gQstat[i].isAssigned=true;
			break;
		}
	return result;
}
DWORD WINAPI tp_QueryThread(LPVOID lpParam)
{
	tp_singleQuery* pData;
	pData = (tp_singleQuery*)lpParam;
	char* query=pData->query;
	int id=pData->id;
	int tid=pData->postThreadID;
		setHighPriority();
		QueryPlanTree tree;//set GPUONLY_QP always false. Assume CoProcessor can only see host data. so directly cancell it.
		
		tree.buildTree(query);
		
		tree.execute();
		int len=tree.planStatus->numResultColumn*tree.planStatus->numResultRow;
		free(tree.planStatus->finalResult);
		return 0;
}

//the naive query processor, just pick the query random one by one.
DWORD WINAPI tp_naiveQP( LPVOID lpParam ) 
{ 
	tp_batchQuery* pData;
	pData = (tp_batchQuery*)lpParam;
	HANDLE dispatchMutex=pData->dispatchMutex;
	EXEC_MODE eM;
	int totalQuery=pData->totalQuery;
	char** sqlQuery=pData->sqlQuery;
	Query_stat *gQstat=pData->stat;
	int threadid=pData->threadid;
	int curQuery;
	char* query=NULL;
	setHighPriority();
	while(1)
	{
		//get the tuples to sort
		WaitForSingleObject( dispatchMutex, INFINITE );
		curQuery=pickQuerySmart(threadid,gQstat,totalQuery);
		if(curQuery==-1) 
		{
			ReleaseMutex( dispatchMutex );
			break;
		}
		ReleaseMutex( dispatchMutex );
		query=sqlQuery[curQuery];//the query pick going to be execute
		//evaluate it.
		MyThreadPoolCop *pool=(MyThreadPoolCop*)malloc(sizeof(MyThreadPoolCop));
		pool->create(1);
		tp_singleQuery* pData = (tp_singleQuery*) HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, sizeof(tp_singleQuery));
		if( pData  == NULL )
			ExitProcess(2);
		pData->init(query,curQuery,threadid);
		pool->assignParameter(0, pData);
		pool->assignTask(0, tp_QueryThread);//execute this query.
		pool->run();
		HeapFree(GetProcessHeap(),0, pData);
		pool->destory();
		//resetGPU();
	}
	return 0;
} 
void testQueryProcessor(QUERY_TYPE fromType, QUERY_TYPE toType, int numQuery,int numThread)
{
	HANDLE dispatchMutex = CreateMutex( NULL, FALSE, NULL );  
	MyThreadPoolCop *pool=(MyThreadPoolCop*)malloc(sizeof(MyThreadPoolCop));	
	pool->create(numThread);//always one thread to handle query request regardsless how many query are there.
	int i=0;
	tp_batchQuery** pData=(tp_batchQuery**)malloc(sizeof(tp_batchQuery*)*numThread);
	char** sqlQuery=(char**)malloc(sizeof(char*)*numQuery);
	//QUERY_TYPE* queryType=(QUERY_TYPE*)malloc(sizeof(QUERY_TYPE)*numQuery);
	Query_stat* gQStat=new Query_stat[numQuery];
	for(i=0;i<numQuery;i++)
	{
		sqlQuery[i]=new char[512];
		gQStat[i].qT=makeRandomQuery(fromType,toType,sqlQuery[i]);
		gQStat[i].isAssigned=false;
	}
	int curID=0;
	int numActiveThread=numThread;
	for( i=0; i<numThread; i++ )
	{
		// Allocate memory for thread data.
		pData[i] = (tp_batchQuery*) HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, sizeof(tp_batchQuery));
		if( pData[i]  == NULL )
			ExitProcess(2);
			pData[i]->init(dispatchMutex,&curID,numQuery,sqlQuery,gQStat,i);
			pool->assignParameter(i, pData[i]);
			pool->assignTask(i, tp_naiveQP);
	}
	int timer=genTimer(0);
	getTimer(timer);
	pool->run();
	double t=getTimer(timer);
	char outputFilename[50];
	sprintf(outputFilename, "./Output/O_level_Time.tony");
	OP_ofp = fopen(outputFilename, "a");
	if(OP_ofp != NULL){
	fprintf(OP_ofp,"<%d> <%d>\n",numQuery,numThread);
	fprintf(OP_ofp,"%lf\n\n",t);
	fclose(OP_ofp);
	}else{
		fprintf(stderr,"\t output file is not created, please check file premission!\n");
	}
	printf("Op level finished, time spend is %lf\n",t);
	for(i=0;i<numThread;i++)
	{
		HeapFree(GetProcessHeap(),0, pData[i]);
	}
	free(pData);
	pool->destory();
	CloseHandle( dispatchMutex );
}