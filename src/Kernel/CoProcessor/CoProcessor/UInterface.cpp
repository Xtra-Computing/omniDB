#include "UInterface.h"
#include "LinkedList.h"
#include "Database.h"

extern Database *easedb;

/*int uSelectOnBaseTable(char *table, char** columns, int numColumn, PREDICATE_NODE * pRoot, Record ** Rout, bool execMode)
{
	__DEBUG__("uSelectOnBaseTable");
	Record** tables=(Record**)malloc(sizeof(Record*)*numColumn);
	int Query_rLen;
	int result=0;
	int i=0, j=0;
	char tempTableName[NAME_MAX_LENGTH];
	for(i=0;i<numColumn;i++)
	{
		//sprintf(tempTableName,"%s.%s",table,columns[i]);
		sprintf(tempTableName,"%s",columns[i]);
		easedb->getTable(tempTableName, &(tables[i]), &Query_rLen);
	}
	result=select(tables,numColumn, Query_rLen,pRoot, Rout, execMode);
	delete []tables;
	return result;
}

int uSelectOnInterMediateTable(int *ridArray, int Query_rLen, char *tableName, char ** columns, int numColumn, PREDICATE_NODE * pRoot, Record ** Rout, bool execMode)
{
	__DEBUG__("uSelectOnInterMediateTable");
	int result=0;
	int i=0, j=0;
	Record** tables=(Record**)malloc(sizeof(Record*)*numColumn);
	for(i=0;i<numColumn;i++)
	{
		tables[i]=(Record *)malloc(sizeof(Record)*Query_rLen);
		rSelect(tableName, columns[i], ridArray, Query_rLen, tables[i], execMode);
	}
	result=select(tables,numColumn, Query_rLen,pRoot, Rout, execMode);
	for(i=0;i<numColumn;i++)
		delete tables[i];
	delete []tables;
	return result;
}*/


//generate a new table with the value from the old table, but the rid are new, consecutive. 
/*int rSelect(char* tableName, char* columnName, int *ridArray, int Query_rLen, Record *Rout, bool execMode)
{
	char tempTableName[NAME_MAX_LENGTH];
	//sprintf(tempTableName,"%s.%s",tableName,columnName);
	sprintf(tempTableName,"%s",columnName);
	Record* originalTable;
	int originalLen;
	int tableID=easedb->getTable(tempTableName, &(originalTable), &originalLen);
	TableProperty py;
	if(! easedb->getTableProperty(tableID, &py))
	{
		int i=0;
		if(py.sortField==0)//sorted on RID
		{
			for(i=0; i<Query_rLen; i++)
			{
				//EVAL((Rout+i),(originalTable+ridArray[i]));
				Rout[i].value=originalTable[ridArray[i]].value;
				Rout[i].rid=i;
			}
		}
		else
		{
			Record *tmpR=new Record [originalLen];
			//printf("error?");
			sort(originalTable, originalLen, compareRID, tmpR, 1);
			for(i=0; i<Query_rLen; i++)
			{
				//EVAL((Rout+i),(tmpR+ridArray[i]));
				Rout[i].value=tmpR[ridArray[i]].value;
				Rout[i].rid=i;
			}
			delete []tmpR;
		}
	}
	return Query_rLen;

}*/


/*int rSelectBaseTable(char* tableName, char* columnName, Record **Rout,bool execMode)
{
	char tempTableName[NAME_MAX_LENGTH];
	//sprintf(tempTableName,"%s.%s",tableName,columnName);
	sprintf(tempTableName,"%s",columnName);
	cl_mem originalTable;
	int originalLen;
	int tableID=easedb->getTable(tempTableName, &(originalTable), &originalLen);
	int Query_rLen=originalLen;
	*Rout=(Record *)malloc(sizeof(Record)*Query_rLen);
	int i=0;
	for(i=0; i<Query_rLen; i++)
	{
		EVAL((*Rout+i),(originalTable+i));
	}
	return Query_rLen;

}*/



//projection, for only one table, no join.
/*int simpleProjection(int *ridArray, int Query_rLen, char *tableName, char ** columns, int numColumn, int **output, bool execMode)
{
	*output=(int *)malloc(sizeof(int)*Query_rLen*numColumn);
	Record *Rout=(Record*)malloc(sizeof(Record)*Query_rLen);
	int col=0;
	int i=0;
	for(col=0; col<numColumn; col++)
	{
		rSelect(tableName, columns[col], ridArray, Query_rLen, Rout,execMode);
		for(i=0;i<Query_rLen;i++)
			(*output)[i*numColumn+col]=Rout[i].value;
	}
	delete Rout;
	return Query_rLen;
}

int uJoin(char* table1, char *column1, Record *R, int Query_rLen, char* table2, char *column2, Record *S, int sLen, PREDICATE_NODE * pRoot, int pType, Record ** Rout, bool execMode)
{
	int res;
	if(pType==TYPE_JOIN_OTHER)
		res=ninlj(R,Query_rLen,S,sLen,pRoot,Rout,execMode);
	else
		if(pType==TYPE_JOIN_EQUAL)
		{
			TableProperty py1;
			TableProperty py2;
			
			int tid1=easedb->getTableID(column1);
			int tid2=easedb->getTableID(column2);
			easedb->getTableProperty(tid1,&py1);
			easedb->getTableProperty(tid2,&py2);

			void *tree=NULL;
			bool RIndexable=true;
			if(execMode==CC_EXEC)
			{
				if(py1.cpu_treeindex==1) 
				{
					RIndexable=true;
					tree=easedb->cpu_treeIndexes[tid1];
				}
				else if(py2.cpu_treeindex==1) 
				{
					RIndexable=false;
					tree=easedb->cpu_treeIndexes[tid2];
				}
			}
			else//co
			{
				if(py1.gpu_treeindex==1) 
				{
					RIndexable=true;
					tree=easedb->co_treeIndexes[tid1];
				}
				else if(py2.gpu_treeindex==1) 
				{
					RIndexable=false;
					tree=easedb->co_treeIndexes[tid2];
				}
			}

			if(tree!=NULL)
			{	
				if(RIndexable)
					res=inlj(R,Query_rLen,tree,S,sLen,pRoot,Rout,execMode);
				else
				{
					__DEBUG__("the R and S oprand exchanged!")
					res=inlj(S,sLen,tree,R,Query_rLen,pRoot,Rout,execMode);
				}
			}
			else 
				res=hj(R,Query_rLen,S,sLen,pRoot,Rout,execMode);
		}
	return res;
}

int uNLJ(Record **input,int num_col, int rNumColumn, int Query_rLen,int sLen, PREDICATE_NODE * pRoot, Record ** Rout,bool execMode)
{
	return multiNLJ(input,num_col,rNumColumn,Query_rLen,sLen,pRoot,Rout,	execMode);
}*/

void getRIDs(Record * R, int Query_rLen, int col, int *RIDArray,bool execMode)
{
	int i=0;
	if(col==0)//col=0, RID
	for(i=0;i<Query_rLen;i++)
		RIDArray[i]=R[i].rid;
	else //col!=0, value
	for(i=0;i<Query_rLen;i++)
		RIDArray[i]=R[i].value;
}

//if col==0, newRIDout[i]=oldRIDout[joinResult[i].rid];
//otherwise, newRIDout[i]=oldRIDout[joinResult[i].value];
void genOriginalRIDs(int* oldRIDout,Record * joinResult,int numResult,int col, int * newRIDout,bool execMode)
{
	if(col==0)
	{
		int i=0;
		for(i=0;i<numResult;i++)
		{
			newRIDout[i]=oldRIDout[joinResult[i].rid];
		}
	}
	else
	{
		int i=0;
		for(i=0;i<numResult;i++)
		{
			newRIDout[i]=oldRIDout[joinResult[i].value];
		}
	}
}

//the aggregation
/*int rAggregation(char* tableName,char* columnName,int* ridArray,int Query_rLen,char *aggType, Record **output, bool execMode)
{
	int numResult;
	Record *data=(Record *)malloc(sizeof(Record)*Query_rLen);
	//printf("modified");
	//rSelect(tableName, columnName, ridArray, Query_rLen, data, execMode);
	if(strcmp(aggType, "SUM")==0)//sum
	{
		numResult=1;
		*output=(Record*)malloc(sizeof(Record)*numResult);
		(*output)->rid=0;
		(*output)->value=sum(data,Query_rLen,execMode);
	}
	else if(strcmp(aggType, "COUNT")==0)//count
	{
		numResult=1;
		*output=(Record*)malloc(sizeof(Record)*numResult);
		(*output)->rid=0;
		(*output)->value=count(data,Query_rLen,execMode);
	}
	else if(strcmp(aggType, "MIN")==0)//min
	{
		numResult=1;
		*output=(Record*)malloc(sizeof(Record)*numResult);
		(*output)->rid=0;
		(*output)->value=min(data,Query_rLen,execMode);
	}
	else if(strcmp(aggType, "MAX")==0)//max
	{
		numResult=1;
		*output=(Record*)malloc(sizeof(Record)*numResult);
		(*output)->rid=0;
		(*output)->value=max(data,Query_rLen,execMode);
	}
	else if(strcmp(aggType, "DISTINCT")==0)//max
	{
		numResult=distinct(data,Query_rLen,1,output,execMode);
	}
	free(data);
	return numResult;   
}

int rOrder(char *tableName,char* columnName,int* ridArray,int Query_rLen,int* permutation,bool execMode)
{
	int numResult=Query_rLen;
	Record *data=(Record *)malloc(sizeof(Record)*Query_rLen);
	rSelect(tableName, columnName, ridArray, Query_rLen, data,execMode);
	Record *Rout=(Record *)malloc(sizeof(Record)*Query_rLen);
	sort(data,Query_rLen,compareRecord,Rout,execMode);
	int i=0;
	for(i=0;i<Query_rLen;i++)
		permutation[i]=(Rout)[i].rid;
	free(data);
	free(Rout);
	free(Rout);
	return numResult;   

}*/

int rPermutation(int *ridData, int Query_rLen, int* permutation, bool execMode)
{
	int* temp=(int *)malloc(sizeof(int)*Query_rLen);
	int i=0;
	for(i=0;i<Query_rLen;i++)
	{
		temp[i]=ridData[permutation[i]];
	}
	memcpy(ridData,temp,sizeof(int)*Query_rLen);
	free(temp);
	return Query_rLen;
}