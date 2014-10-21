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
extern FILE *K_ofp;
CRITICAL_SECTION PoolLock;
int CurentID;
extern int numQueries;//->corresponding to numOfThread for K_schedule
extern int numThread;//this is fixed to 1 for Q and O schedule
extern int Query_rLen;
void tp_QueryThread(tp_singleQuery* _pData)
{
	char* query=_pData->query;
	EXEC_MODE eM=_pData->eM;
	int id=_pData->id;
	int tid=_pData->postThreadID;
	setHighPriority();
	QueryPlanTree tree;//set GPUONLY_QP always false. Assume CoProcessor can only see host data. so directly cancell it.
	tree.buildTree(query);
	tree.execute(eM);
	int len=tree.planStatus->numResultColumn*tree.planStatus->numResultRow;
	free(tree.planStatus->finalResult);
}
//the naive query processor, just pick the query random one by one.
DWORD WINAPI tp_naiveQP( LPVOID lpParam ) 
{ 
	tp_batchQuery* pData;
	pData = (tp_batchQuery*)lpParam;
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
		EnterCriticalSection(&(PoolLock));
		curQuery=CurentID++;
		LeaveCriticalSection(&(PoolLock));
		if(curQuery>=numQueries) 
		{
			break;
		}
		query=sqlQuery[curQuery];//the query pick going to be execute

		tp_singleQuery* pData = (tp_singleQuery*) HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, sizeof(tp_singleQuery));
		pData->init(gQstat[curQuery].eM,query,curQuery,threadid);
		tp_QueryThread(pData);
		HeapFree(GetProcessHeap(),0, pData);
		//resetGPU();
	}
	return 0;
} 
void testQueryProcessor(QUERY_TYPE fromType, QUERY_TYPE toType, int numQuery,int numThread)
{
//	HANDLE dispatchMutex = CreateMutex( NULL, FALSE, NULL );  
	InitializeCriticalSection(&(PoolLock));
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
		//cout<<gQStat[i].speedupGPUoverCPU<<endl;
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
			pData[i]->init(&curID,numQuery,sqlQuery,gQStat,i);
			pool->assignParameter(i, pData[i]);
			pool->assignTask(i, tp_naiveQP);
	}
	int timer=genTimer(2);
	getTimer(timer);
	pool->run();
	double t=getTimer(timer);

	char outputFilename[50];
	sprintf(outputFilename, "./Output/K_level_Time.tony");
	K_ofp = fopen(outputFilename, "a");
	if(K_ofp !=NULL){
		fprintf(K_ofp,"<%d> <%d>\n",numQueries,numThread);
		fprintf(K_ofp,"%lf\n",t);	
		fclose(K_ofp);
	}else{
		fprintf(stderr,"\t output file is not created, please check file premission!\n");
	}
	printf("------------Kernel level Query finished in %lfd---------------\n\n",t);
	for(i=0;i<numThread;i++)
	{
		HeapFree(GetProcessHeap(),0, pData[i]);
	}
	free(pData);
	pool->destory();	
}