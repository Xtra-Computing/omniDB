#ifndef EXEC_STATUS
#define EXEC_STATUS

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "db.h"
#include "QP_Utility.h"
#include "Database.h"
#include "CoProcessor.h"

extern Database *easedb;

typedef enum{
	DATA_ON_CPU,
	DATA_ON_GPU,
	DATA_ON_UNKNOWN
}DATA_RESIDENCE;


struct ExecStatus
{
//attributes,
	//one RID list corresponds to one base table.
	char RIDTableNames [MAX_TABLE_PER_QUERY][NAME_MAX_LENGTH];
	//int* RID_baseTable [MAX_TABLE_PER_QUERY];
	cl_mem RID_baseTable [MAX_TABLE_PER_QUERY];
	int RIDLen[MAX_TABLE_PER_QUERY];
	int numTables;
	//one columnNames corresponds to one table, one base table, 	
	int* finalResult;
	int numResultColumn;
	int numResultRow;
	bool hasGroupBy;
	int* groupByStartPos;
	int groupByNumGroup;
	cl_mem groupByRelation;
	DATA_RESIDENCE grpRes;
	int groupByRlen;
	
//methods,
	int getTableID(char* tableName, char *columnName)
	{
		int id=-1;
		//find the exact column.
		for(int i=0;i<numTables;i++)
		{
			if(strcmp(tableName,RIDTableNames[i])==0)
			{
				id=i;
				break;
			}
		}
		//not found in the plan status, we fetch it from easedb.
		if(id==-1)
		{
			//otherwise, we find the base table.
			strcpy(RIDTableNames[numTables],tableName);
			id=numTables;
			RID_baseTable[id]=NULL;			
			numTables++;
		}
		return id;	
	}
 
	int getDataTable(int id, char* columnName, cl_mem* Rout,EXEC_MODE eM)
	{
		assert(id>=0 && id<numTables);
		int resultLen;
		if(RID_baseTable[id]==NULL)
		{
			easedb->getTable(columnName,Rout,&resultLen);
			//OP_bufferchecking(*Rout,1);
			RIDLen[id]=resultLen;
		}
		else
		{
			resultLen=RIDLen[id];
			//*Rout=new Record[resultLen];
			CL_CREATE(Rout,sizeof(Record)*resultLen);			
			//int i=0;
			//for(i=0;i<resultLen;i++)
			//	(*Rout)[i].rid=RID_baseTable[id][i];
			CL_setRIDList(RID_baseTable[id],resultLen,*Rout,256,64,eM);
			//OP_bufferchecking(*Rout,1);
			cl_mem baseTable=NULL;
			int OP_rLen;
			easedb->getTable(columnName,&baseTable,&OP_rLen);
			CL_ProjectionOnly(baseTable,RIDLen[id],*Rout,resultLen,256,256,eM);	
			CL_DESTORY(&baseTable);
		}
		return resultLen;
	}
	//always get the original table. we don't need to adapt.
	int getBaseTable(int id, char* columnName, cl_mem* Rout)
	{
		assert(id>=0 && id<numTables);
		int OP_rLen;
		easedb->getTable(columnName,Rout,&OP_rLen);
		return OP_rLen;
	}
	//this one needs also assert the GPUONLY_QP and dataRes.
	int getRIDList(int id, char* columnName,cl_mem* RIDList,EXEC_MODE eM)
	{
		assert(id>=0 && id<numTables);
//		assert((dataRes[id]==DATA_ON_GPU && GPUONLY_QP) || (dataRes[id]==DATA_ON_CPU && (!GPUONLY_QP)));
		if(RID_baseTable[id]==NULL)
		{	
			cl_mem Rout;
			int resultLen=0;
			CL_getRIDList(Rout,resultLen,&(RID_baseTable[id]),256,64,eM);	
			/*Record *Rout;
			int resultLen=0, i=0;
			easedb->getTable(columnName,&Rout,&resultLen);
			RID_baseTable[id]=new int[resultLen];
			for(i=0;i<resultLen;i++)
			RID_baseTable[id][i]=Rout[i].rid;*/			
			RIDLen[id]=resultLen;
		}	
		*RIDList=RID_baseTable[id];
		return RIDLen[id];
	}

	void addJoinTable(int ID1, int ID2, cl_mem dt,int OP_rLen,EXEC_MODE eM)
	{
		assert(ID1>=0 && ID1<numTables);
		assert(ID2>=0 && ID2<numTables);
//		assert((dataRes[ID1]==DATA_ON_GPU && GPUONLY_QP) || (dataRes[ID1]==DATA_ON_CPU && (!GPUONLY_QP)));
//		assert((dataRes[ID2]==DATA_ON_GPU && GPUONLY_QP) || (dataRes[ID2]==DATA_ON_CPU && (!GPUONLY_QP)));
		if(RID_baseTable[ID1]!=NULL)
			CL_DESTORY(&RID_baseTable[ID1]);
		if(RID_baseTable[ID2]!=NULL)
			CL_DESTORY(&RID_baseTable[ID2]);
		if(OP_rLen<=0)
		{
			CL_CREATE(&(RID_baseTable[ID1]),sizeof(int));
			CL_CREATE(&(RID_baseTable[ID2]),sizeof(int));
		}
		else 
		{
			CL_getRIDList(dt,OP_rLen,&(RID_baseTable[ID1]),256,64,eM);
			CL_getValueList(dt,OP_rLen,&(RID_baseTable[ID2]),256,64,eM);
		}
/*		if(RID_baseTable[ID1]!=NULL)
			delete RID_baseTable[ID1];
		if(RID_baseTable[ID2]!=NULL)
			delete RID_baseTable[ID2];
		if(OP_rLen==0)
		{
			RID_baseTable[ID1]=new int[1];
			RID_baseTable[ID2]=new int[1];
		}
		else 
		{
			RID_baseTable[ID1]=new int[OP_rLen];
			RID_baseTable[ID2]=new int[OP_rLen];
		}
		int i=0;
		for(i=0;i<OP_rLen;i++)
		{
			RID_baseTable[ID1][i]=dt[i].rid;
			RID_baseTable[ID2][i]=dt[i].value;
		}
		*/
		RIDLen[ID1]=OP_rLen;
		RIDLen[ID2]=OP_rLen;
	}
//add table??? we don't know whether it is from gpu or cpu.
	void addDataTable(int id, cl_mem dt,int OP_rLen, DATA_RESIDENCE dataStorePlace,EXEC_MODE eM)
	{
		assert(id>=0 && id<numTables);
		if(RID_baseTable[id]!=NULL)
		{
				CL_DESTORY(&RID_baseTable[id]);
		}
		if(OP_rLen<=0)
			CL_CREATE(&(RID_baseTable[id]),sizeof(int));
		else 
		{
			cl_mem tempDT=dt;
			/*if(dataStorePlace==DATA_ON_CPU)
			{
				GPUAllocate((void**)&tempDT,rLen*sizeof(Record));
				CopyCPUToGPU(tempDT,dt,rLen*sizeof(Record));	
				DATA_TO_GPU(rLen*sizeof(Record));
			}*/
			CL_getRIDList(tempDT,OP_rLen,&(RID_baseTable[id]),256,64,eM);
		}
/*		if(RID_baseTable[id]!=NULL)
		{
			delete RID_baseTable[id];
		}
		if(OP_rLen==0)
			RID_baseTable[id]=new int[1];
		else 
			RID_baseTable[id]=new int[OP_rLen];
		Record *tempDT=dt;
		int i=0;
		for(i=0;i<OP_rLen;i++)
			RID_baseTable[id][i]=tempDT[i].rid;
*/
		RIDLen[id]=OP_rLen;
	}
	void addRIDList(int id, cl_mem dt,int OP_rLen, DATA_RESIDENCE dataStorePlace)
	{
		assert(id>=0 && id<numTables);
		//free(RID_baseTable[id]);
		CL_DESTORY(&RID_baseTable[id]);
		//even in adaptive mode, we change the execution mode and then addRIDList.
		//it assert: (dataStorePlace==DATA_ON_GPU && GPUONLY_QP) || (dataStorePlace==DATA_ON_CPU && (!GPUONLY_QP))
//		assert((dataStorePlace==DATA_ON_GPU && GPUONLY_QP) || (dataStorePlace==DATA_ON_CPU && (!GPUONLY_QP)));
		RID_baseTable[id]=dt;
		RIDLen[id]=OP_rLen;
	}
	void init()
	{
		numTables=0;
		groupByStartPos=NULL;
		for(int i=0;i<MAX_TABLE_PER_QUERY;i++)
		{
			RID_baseTable[i]=NULL;
		}
		finalResult=NULL;
		numResultColumn=-1;
		numResultRow=-1;
		hasGroupBy=false;
	}
	void destory()
	{
		cl_int status;
		cl_uint refCount;
		for(int i=0;i<MAX_TABLE_PER_QUERY;i++)
		{
				CL_DESTORY(&RID_baseTable[i]);
			
		}
	}
};
#endif


