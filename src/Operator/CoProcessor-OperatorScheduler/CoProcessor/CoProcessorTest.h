#ifndef CO_PROCESSOR_TEST_H
#define CO_PROCESSOR_TEST_H

#include "windows.h"
#include "CoProcessor.h"
#include "CoProcessorTest.h"

typedef enum{
	Q_POINT_SELECTION,
	Q_RANGE_SELECTION,
	Q_AGG,
	Q_ORDERBY,
	Q_INLJ,
	Q_SMJ,
	Q_HJ,
	Q_DBMBENCH1,
	Q_DBMBENCH2,
	Q_DBMBENCH3,
	Q_AGG_GROUPBY,
	Q_NINLJ,
}QUERY_TYPE;

struct Query_stat{
	bool isAssigned;
	EXEC_MODE eM;
	QUERY_TYPE qT;
	float speedupGPUoverCPU;
	float timeInSec;
	//bool needCop;
};



struct tp_batchQuery{
	HANDLE dispatchMutex;
	int* curQueryID;
	int totalQuery;
	char** sqlQuery;
	Query_stat* stat;
	int threadid;
	void init(HANDLE pMutex, int* pcurQueryID, int ptotalQuery,
		char** psqlQuery, Query_stat* pstat, int pthreadid)
	{
		dispatchMutex=pMutex;
		curQueryID=pcurQueryID;
		totalQuery=ptotalQuery;
		sqlQuery=psqlQuery;
		stat=pstat;
		threadid=pthreadid;
	}
};

struct tp_singleQuery{
	char* query;
	int id;
	int postThreadID;
	void init(char* pquery, int pid, int ppostThreadID)
	{
		query=pquery;
		id=pid;
		postThreadID=ppostThreadID;
	}
};

void test_plan(int delta, int isGPUONLY_QP, int isAdaptive, int execMode);
void initDB2(char *confFile,int Uplimit);
void testMicroBenchmark(int scale, int delta, int isGPUONLY_QP, int isAdaptive, int execMode);
void makeTestQuery(QUERY_TYPE qt, char *query);
QUERY_TYPE makeRandomQuery(QUERY_TYPE fromType, QUERY_TYPE toType, char *query);
void execQuery(char *query, bool isGPUONLY_QP, bool isAdaptive, EXEC_MODE eM);
void testQueryProcessor(QUERY_TYPE fromType, QUERY_TYPE toType, int numQuery, int numThread);
int pickQuerySmart(Query_stat *gQstat, int numQuery);
void testMbench(int numQuery, EXEC_MODE eM, int numThread, int scale);

#endif

