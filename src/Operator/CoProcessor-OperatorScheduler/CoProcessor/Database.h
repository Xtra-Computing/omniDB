#ifndef GPUQB_DATABASE_H
#define GPUQB_DATABASE_H
//#pragma once
#include "hash.h"
#include "db.h"
#include "CPU_Dll.h"
#include "OpenCL_DLL.h"
extern int OP_rLen;
/*
Since we are using column-based model, if a table TT(a,b,c), so we store them into 
three tables with name, TT.a, TT.b, TT.c. 
*/
struct TableProperty
{
	int OP_rLen;
	int sortField;//0, rid; 1, value.
	int gpu_treeindex;//0, no index; 1, CSS-tree index
	int cpu_treeindex;//0, no index; 1, CSS-tree index
};
#define PY_EVAL(Rhs, Tmp) { Rhs->OP_rLen = Tmp->OP_rLen;Rhs->sortField = Tmp->sortField;Rhs->gpu_treeindex = Tmp->gpu_treeindex;Rhs->cpu_treeindex = Tmp->cpu_treeindex;}
class Database
{
public:
	Database(void);
	~Database(void);
	int check(char *rName);
	int createRandomTable(char* rName, cl_mem R, int OP_rLen);
	int createSortedTable(char* rName, cl_mem R, int OP_rLen);
	int dropTable(char* rName);
	int getTable(char* rName, cl_mem* Rout, int * OP_rLen);

	HashTable* nameIndex; //map the table name to the index.
	int numTable;
	Record** tables;
	TableProperty* tPro;
	static int test(void);
	RET_VALUE getTableProperty(int tableID, TableProperty* py);
	int getTableLength(char* tableName);
	HashTable* cc_indexObjs;
	CUDA_CSSTree ** cpu_treeIndexes;
	CUDA_CSSTree ** gpu_treeIndexes;
	int numIndex;
	int createTreeIndex(char* tableName, char* columnName);
	void removeTreeIndex(char* tableName, char* columnName);
	void getTreeIndex(char* columnName,CUDA_CSSTree** cIndex, CUDA_CSSTree** gIndex)
	{ 
		int id=getTableID(columnName);
		*cIndex=cpu_treeIndexes[id];
		*gIndex=gpu_treeIndexes[id];
	}
	void sortTable(char* tableName, char* columnName);
	int loadDB(char* conFile,int Uplimit);
	int dumpDB(char* conFile, bool toWrite);
	int addTable(char* tableName, Record* data, int OP_rLen);
	char allTableName[MAX_TABLE_NUM][NAME_MAX_LENGTH];
	char allColumnName[MAX_TABLE_NUM][NAME_MAX_LENGTH*4];
	int numColumnInOTable[MAX_TABLE_NUM];
	int numOriginalTable;//the row-storage model table.
	int dbmbench(int scale);
	int defaultLoad(double scale=1.0);
	int getTableID(char* tableName);
	int dbmbenchForMonet(int scale);
	int dbmbenchForMonetLoad(int scale);
};

#endif

