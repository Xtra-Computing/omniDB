#include "CoProcessorTest.h"
#include "QueryPlanTree.h"
#include "Database.h"

Database *easedb;

/*typedef enum{
	Q_POINT_SELECTION,
	Q_RANGE_SELECTION,
	Q_AGG,
	Q_ORDERBY,
	Q_AGG_GROUPBY,
	Q_NINLJ,
	Q_INLJ,
	Q_SMJ,
	Q_HJ,//9
	Q_DBMBENCH1,
	Q_DBMBENCH2,
	Q_DBMBENCH3,
}QUERY_TYPE;*/

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
int qselect=0;
void initDB2(char *confFile,int Uplimit)
{
	if(easedb!=NULL) delete easedb;
	easedb=new Database();
	easedb->loadDB(confFile,Uplimit);
	int i=0;
	int id=easedb->getTableID("R.b00");
#if 0
	if(id!=-1)
	{
		Record* bb=easedb->tables[id];
		int OP_rLen=easedb->tPro[id].OP_rLen;
		for(i=0;i<OP_rLen;i++)
			bb[i].value=bb[i].value%64;
	}
#endif
}

QUERY_TYPE makeRandomQuery(QUERY_TYPE fromType, QUERY_TYPE toType, char *query)
{
	QUERY_TYPE result=fromType;
	int offset=(int)(toType-fromType+1);
	//int randomValue=RAND(offset);
	int randomValue=(qselect)%offset;
	qselect++;
	result=(QUERY_TYPE)(fromType+randomValue);
	cout << "result" << (int)result <<"qselect"<<qselect <<endl;
	makeTestQuery(result,query);
	return result;
}

void makeTestQuery(QUERY_TYPE qt, char *query)
{
	switch(qt)
	{
	case Q_POINT_SELECTION:
		{
			int a=RAND(TEST_MAX);
			//cout<<"point: "<<a<<endl;
			sprintf(query, "PRO;R;$;R.a00,;$;:SEL;R;$;R.a00,;=,R.a00,$,$,%d,$,$,;:$:$:$:", a);
		}break;
	case Q_RANGE_SELECTION:
		{
			int a=RAND(TEST_MAX);
			int b=RAND(TEST_MAX);
			int minmin=min(a,b);
			int maxmax=minmin+20000000;//max(a,b);
			//result: 155943, 10000000
			//result: 311916, 20000000
			//cout<<"range: ["<<minmin<<", "<<maxmax<<"]"<<endl;
			sprintf(query, "PRO;R;$;R.a00,;$;:SEL;R;$;R.a00,;AND,>,R.a00,$,$,%d,$,$,<,R.a00,$,$,%d,$,$,;:$:$:$:", minmin, maxmax);
		}break;
	case Q_AGG:
		{
			sprintf(query, "AGG;R;MAX;R.a00,;$;:$:$:");
		}break;
	case Q_ORDERBY:
		{
			sprintf(query, "AGG;R;MAX;R.a00,;$;:ORD;R;$;R.a00,;$;:$:$:$:");
			//sprintf(query, "PRO;$;$;R.b00,;$;:ORD;R;$;R.a00,;$;:$:$:$:");
			//sprintf(query, "PRO;R;$;R.b00,;$;:ORD;R;$;R.a00,;$;:SEL;R;$;R.b00,;<,R.b00,$,$,8,$,$,;:$:$:$:$:");
		}break;
	case Q_AGG_GROUPBY:
		{
			sprintf(query, "AGG;R;MAX;R.a00,;$;:GRP;R;$;R.b00,;$;:$:$:$:");
		}break;
	case Q_NINLJ:
		{
			//sprintf(query, "JOIN;R;S;R.a00,S.a00,;<,R.a00,$,$,S.a00,$,$,;:$:$:");
			/*int a1=RAND(TEST_MAX);
			int a2=RAND(TEST_MAX);
			int minmin1=a1;
			int maxmax1=minmin1+20000000;//max(a,b);
			int minmin2=a2;
			int maxmax2=minmin2+20000000;//max(a,b);
			sprintf(query, "PRO;R;$;R.a00,;$;:JOIN;R;S;R.a00,S.a00,;<,R.a00,$,$,S.a00,$,$,;:SEL;R;$;R.a00,;AND,>,R.a00,$,$,%d,$,$,<,R.a00,$,$,%d,$,$,;:$:$:SEL;S;$;S.a00,;AND,>,S.a00,$,$,%d,$,$,<,S.a00,$,$,%d,$,$,;:$:$:$:",minmin1,maxmax1,minmin2,maxmax2);*/
			//cout<<"I have changed the NLJ for testing"<<endl;
			int a1=RAND(TEST_MAX);
			int a2=RAND(TEST_MAX);
			int delta=(int)(20*1000000.0);
			int minmin1=a1;
			int maxmax1=minmin1+delta;//max(a,b);
			int minmin2=a2;
			int maxmax2=minmin2+delta;//max(a,b);
			sprintf(query, "PRO;R;$;R.a00,;$;:JOIN;R;S;R.a00,S.a00,;<,R.a00,$,$,S.a00,$,$,;:SEL;R;$;R.a00,;AND,>,R.a00,$,$,%d,$,$,<,R.a00,$,$,%d,$,$,;:$:$:SEL;S;$;S.a00,;AND,>,S.a00,$,$,%d,$,$,<,S.a00,$,$,%d,$,$,;:$:$:$:",minmin1,maxmax1,minmin2,maxmax2);
		}break;
	case Q_INLJ:
		{

			sprintf(query,"PRO;R;$;R.a00,;$;:JOIN;R;S;R.a00,S.a00,;=,R.a00,$,$,S.a00,$,$,;:$:$:$:");//my own invension. =  -> INLJ
		}break;
	case Q_SMJ:
		{
			sprintf(query,"PRO;R;$;R.a00,;$;:JOIN;R;S;R.a00,S.a00,;+,R.a00,$,$,S.a00,$,$,;:$:$:$:");//my own invension. +  -> SMJ
		}break;
	case Q_HJ:
		{
			sprintf(query,"PRO;R;$;R.a00,;$;:JOIN;R;S;R.a00,S.a00,;-,R.a00,$,$,S.a00,$,$,;:$:$:$:");//my own invension. - -> HJ
		}break;
	case Q_DBMBENCH1:
		{
			int a=RAND(20000);
			int b=(a-2000);
			if(b<0) b=a+2000;
			int minmin=min(a,b);
			int maxmax=max(a,b);
			sprintf(query,"PRO;$;$;T1.a3,;$;:SEL;$;T1;T1.a2,;AND,<,%d,$,$,T1.a2,$,$,<,T1.a2,$,$,%d,$,$,;:$:$:$:",minmin,maxmax);
		}break;
	case Q_DBMBENCH2:
		{
			int a=RAND(20000);
			int b=(a-4000);
			if(b<0) b=a+4000;
			int minmin=min(a,b);
			int maxmax=max(a,b);
			sprintf(query,"AGG;T1;AVG;T1.a3,;$;:JOIN;T1;T2;T1.a1,T2.a1,;=,T1.a1,$,$,T2.a1,$,$,;:SEL;$;T1;T1.a2,;AND,<,%d,$,$,T1.a2,$,$,<,T1.a2,$,$,%d,$,$,;:$:$:$:$:",minmin,maxmax);
			//sprintf(query,"PRO;T1;$;T1.a3,;$;:JOIN;T1;T2;T1.a1,T2.a1,;=,T1.a1,$,$,T2.a1,$,$,;:$:$:$:");
		}break;
	case Q_DBMBENCH3:
		{
			int a=RAND(20000);
			int b=(a-100);
			if(b<0) b=a+100;
			int minmin=min(a,b);
			int maxmax=max(a,b);
			sprintf(query,"AGG;T1;AVG;T1.a3,;$;:SEL;$;T1;T1.a2,;AND,<,%d,$,$,T1.a2,$,$,<,T1.a2,$,$,%d,$,$,;:$:$:$:",minmin,maxmax);
		}break;
	}
	__DEBUG__(query);
}


void GPUDEBUG_Int(int* d_RIDList, int RIDLen, bool GPUONLY_QP)
{
#ifdef DEBUG
if( GPUONLY_QP)
{
	int tempSize=sizeof(int)*RIDLen;
	int* temp=NULL;
	CPUAllocateByCUDA((void**)&temp,tempSize);
	CopyGPUToCPU(temp,d_RIDList,tempSize);
	int i=0;
	cout<<"GPUDEBUG_Int"<<endl;
	for(i=0;i<RIDLen;i++)
		cout<<temp[i]<<"; ";
	cout<<endl;
	CPUFreeByCUDA(temp);
}
else
{
	cout<<"GPUDEBUG_Int"<<endl;
	int i=0;
	for(i=0;i<RIDLen;i++)
		cout<<d_RIDList[i]<<"; ";
	cout<<endl;
}
#endif
}
void GPUDEBUG_Record(Record* d_RIDList, int RIDLen, bool GPUONLY_QP)
{
#ifdef DEBUG
if( GPUONLY_QP)
{
	int tempSize=sizeof(Record)*RIDLen;
	Record* temp=NULL;
	CPUAllocateByCUDA((void**)&temp,tempSize);
	CopyGPUToCPU(temp,d_RIDList,tempSize);
	int i=0;
	cout<<"GPUDEBUG_Record: "<<RIDLen<<endl;
	for(i=0;i<RIDLen;i++)
		cout<<temp[i].rid<<","<<temp[i].value<<"; ";
	cout<<endl;
	CPUFreeByCUDA(temp);
}
else
{
	cout<<"GPUDEBUG_Record"<<endl;
	int i=0;
	for(i=0;i<RIDLen;i++)
		cout<<d_RIDList[i].rid<<","<<d_RIDList[i].value<<"; ";
	cout<<endl;
}
#endif
}


char* OpToString(OP_MODE op,EXEC_MODE eM)
{
	switch(eM)
	{
	case EXEC_CPU:
		{
			switch(op)
			{
			case SELECTION:
				return "SELECTION, on the CPU";
			case TYPE_AGGREGATION:
				return "TYPE_AGGREGATION, on the CPU";
			case AGG_SUM:
				return "AGG_SUM, on the CPU";
			case AGG_MAX:
				return "AGG_MAX, on the CPU";
			case AGG_MIN:
				return "AGG_MIN, on the CPU";
			case AGG_AVG:
				return "AGG_AVG, on the CPU";
			case AGG_COUNT:
				return "AGG_COUNT, on the CPU";
			case GROUP_BY:
				return "GROUP_BY, on the CPU";
			case AGG_SUM_AFTER_GROUP_BY:
				return "AGG_SUM_AFTER_GROUP_BY, on the CPU";
			case AGG_MAX_AFTER_GROUP_BY:
				return "AGG_MAX_AFTER_GROUP_BY, on the CPU";
			case AGG_AVG_AFTER_GROUP_BY:
				return "AGG_AVG_AFTER_GROUP_BY, on the CPU";
			case AGG_COUNT_AFTER_GROUP_BY:
				return "AGG_COUNT_AFTER_GROUP_BY, on the CPU";
			case ORDER_BY:
				return "ORDER_BY, on the CPU";
			case SORT:
				return "SORT, on the CPU";
			case PROJECTION:
				return "PROJECTION, on the CPU";
			case TYPE_JOIN:
				return "TYPE_JOIN, on the CPU";
			case JOIN_NINLJ:
				return "JOIN_NINLJ, on the CPU";
			case JOIN_INLJ:
				return "JOIN_INLJ, on the CPU";
			case JOIN_SMJ:
				return "JOIN_SMJ, on the CPU";
			case JOIN_HJ:
				return "JOIN_HJ, on the CPU";
			case DISTINCT:
				return "DISTINCT, on the CPU";
			default:
				return "TYPE_UNKNOWN, on the CPU";
			}
		}break;
	case EXEC_GPU:
		{
			switch(op)
			{
			case SELECTION:
				return "SELECTION, on the GPU";
			case TYPE_AGGREGATION:
				return "TYPE_AGGREGATION, on the GPU";
			case AGG_SUM:
				return "AGG_SUM, on the GPU";
			case AGG_MAX:
				return "AGG_MAX, on the GPU";
			case AGG_MIN:
				return "AGG_MIN, on the GPU";
			case AGG_AVG:
				return "AGG_AVG, on the GPU";
			case AGG_COUNT:
				return "AGG_COUNT, on the GPU";
			case GROUP_BY:
				return "GROUP_BY, on the GPU";
			case AGG_SUM_AFTER_GROUP_BY:
				return "AGG_SUM_AFTER_GROUP_BY, on the GPU";
			case AGG_MAX_AFTER_GROUP_BY:
				return "AGG_MAX_AFTER_GROUP_BY, on the GPU";
			case AGG_AVG_AFTER_GROUP_BY:
				return "AGG_AVG_AFTER_GROUP_BY, on the GPU";
			case AGG_COUNT_AFTER_GROUP_BY:
				return "AGG_COUNT_AFTER_GROUP_BY, on the GPU";
			case ORDER_BY:
				return "ORDER_BY, on the GPU";
			case SORT:
				return "SORT, on the GPU";
			case PROJECTION:
				return "PROJECTION, on the GPU";
			case TYPE_JOIN:
				return "TYPE_JOIN, on the GPU";
			case JOIN_NINLJ:
				return "JOIN_NINLJ, on the GPU";
			case JOIN_INLJ:
				return "JOIN_INLJ, on the GPU";
			case JOIN_SMJ:
				return "JOIN_SMJ, on the GPU";
			case JOIN_HJ:
				return "JOIN_HJ, on the GPU";
			case DISTINCT:
				return "DISTINCT, on the GPU";
			default:
				return "TYPE_UNKNOWN, on the GPU";
			}

		}break;
	}
}


void setHighPriority()
{
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
	//cout<<"setHighPriority"<<endl;
}