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
extern double evalautedQuery;
extern double Query_UpCPUBurden;
extern double Query_LoCPUBurden;
extern double Query_UpGPUBurden;
extern double Query_LoGPUBurden;
extern double Query_CPUBurden;
extern double Query_GPUBurden;
extern double Query_LothresholdForGPUApp;
extern double Query_LothresholdForCPUApp;
extern double Query_SpeedupGPUOverCPU[12];
extern double RunInCPU[12];
extern double RunInGPU[12];

extern CRITICAL_SECTION Query_CPUBurdenCS;
extern CRITICAL_SECTION Query_GPUBurdenCS;
extern CRITICAL_SECTION preEMCS;
int preEM=EXEC_CPU;
extern FILE *Query_ofp;
//#define Greedy
void inline GPUBurdenINC(const double burden){
		EnterCriticalSection(&(Query_GPUBurdenCS));
		Query_GPUBurden+=(burden);
		LeaveCriticalSection(&(Query_GPUBurdenCS));
}
void inline CPUBurdenINC(const double burden){
		EnterCriticalSection(&(Query_CPUBurdenCS));
		Query_CPUBurden+=(burden);
		LeaveCriticalSection(&(Query_CPUBurdenCS));
}
void inline GPUBurdenDEC(const double burden){
		EnterCriticalSection(&(Query_GPUBurdenCS));
		Query_GPUBurden-=(burden);
		LeaveCriticalSection(&(Query_GPUBurdenCS));
}
void inline CPUBurdenDEC(const double burden){
		EnterCriticalSection(&(Query_CPUBurdenCS));
		Query_CPUBurden-=(burden);
		LeaveCriticalSection(&(Query_CPUBurdenCS));
}
double inline getSpeedUP(int qid)
{
	return Query_SpeedupGPUOverCPU[qid];
}
double inline getAddCPUBurden(int qid)
{
	return RunInCPU[qid];
}
double inline getAddGPUBurden(int qid)
{
	return RunInGPU[qid];
}

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

/*QUERY SCHEDULER*/
#ifdef Greedy
		if((Query_CPUBurden+getAddCPUBurden(i))<(Query_GPUBurden+getAddGPUBurden(i))){
			printf("Query_CPUBurden %lf,Query_GPUBurden %lf\n",Query_CPUBurden,Query_GPUBurden);
				gQstat[i].eM=EXEC_CPU;
				CPUBurdenINC(getAddCPUBurden(i));
		}
		else{
				gQstat[i].eM=EXEC_GPU;
				GPUBurdenINC(getAddGPUBurden(i));
		}
#else 
		EnterCriticalSection(&(preEMCS));
		if(preEM==EXEC_CPU){
			gQstat[i].eM=EXEC_GPU;
			GPUBurdenINC(getAddGPUBurden(i));
			preEM=EXEC_GPU;
		}else{
			gQstat[i].eM=EXEC_CPU;
			CPUBurdenINC(getAddCPUBurden(i));
			preEM=EXEC_CPU;
		}
		LeaveCriticalSection(&(preEMCS));
#endif
	return result;
}


DWORD WINAPI tp_QueryThread(LPVOID lpParam)
{
	tp_singleQuery* pData;
	pData = (tp_singleQuery*)lpParam;
	char* query=pData->query;
	EXEC_MODE eM=pData->eM;
	int id=pData->id;
	int tid=pData->postThreadID;
		setHighPriority();
		QueryPlanTree tree;//set GPUONLY_QP always false. Assume CoProcessor can only see host data. so directly cancell it.
		tree.buildTree(query);
		tree.execute(eM);
		int len=tree.planStatus->numResultColumn*tree.planStatus->numResultRow;
deschedule:
		if(eM=EXEC_CPU){
			CPUBurdenDEC(getAddCPUBurden(pData->id));
		}
		else{
			GPUBurdenDEC(getAddGPUBurden(pData->id));
		}
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
		//query schedule happen here./////////////////////////////////////////////////////////////////////////
		curQuery=pickQuerySmart(threadid,gQstat,totalQuery);
		if(curQuery==-1) 
		{
			ReleaseMutex( dispatchMutex );
			break;
		}
		evalautedQuery++;
		ReleaseMutex( dispatchMutex );
		query=sqlQuery[curQuery];//the query pick going to be execute

		//evaluate it.
		MyThreadPoolCop *pool=(MyThreadPoolCop*)malloc(sizeof(MyThreadPoolCop));
		pool->create(1);
		tp_singleQuery* pData = (tp_singleQuery*) HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, sizeof(tp_singleQuery));

		if( pData  == NULL )
			ExitProcess(2);
		pData->init(gQstat[curQuery].eM,query,curQuery,threadid);
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
		gQStat[i].speedupGPUoverCPU=getSpeedUP(gQStat[i].qT);
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
			pData[i]->init(dispatchMutex,&curID,numQuery,sqlQuery,gQStat,i);
			pool->assignParameter(i, pData[i]);
			pool->assignTask(i, tp_naiveQP);
	}
	int timer=genTimer(1);
	getTimer(timer);
	pool->run();
	double t=getTimer(timer);
	char outputFilename[50];
	sprintf(outputFilename, "./Output/Q_level_Time.tony");
	Query_ofp = fopen(outputFilename, "a");
	if(Query_ofp != NULL){
	fprintf(Query_ofp,"<%d> <%d>\n",numQuery,numThread);
	fprintf(Query_ofp,"%lf\n\n",t);
	fclose(Query_ofp);
	}else{
		fprintf(stderr,"\t output file is not created, please check file premission!\n");
	}
	printf("Query Level test finished. Time spend is %lf\n\n",t);
	for(i=0;i<numThread;i++)
	{
		HeapFree(GetProcessHeap(),0, pData[i]);
	}
	free(pData);
	pool->destory();
	CloseHandle( dispatchMutex );
}