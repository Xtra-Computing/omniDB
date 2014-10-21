#ifndef U_INTERFACE_H
#define U_INTERFACE_H

#include "db.h"
#include "QueryPlanNode.h"

//selection on <numColumn> columns.
//int uSelectOnBaseTable(char *table, char** columns, int numColumn, PREDICATE_NODE * pRoot, Record ** Rout, bool execMode);
//int uSelectOnInterMediateTable(int *ridArray, int OP_rLen, char *tableName, char ** columns, int numColumn, PREDICATE_NODE * pRoot, Record ** Rout, bool execMode);
//selection based on RIDs


//int rSelect(char* tableName, char* columnName, int *ridArray, int OP_rLen, Record *Rout, bool execMode);

int rSelectBaseTable(char* tableName, char* columnName, Record **Rout, bool execMode);

//dump the column <col> to an integer array.
void getRIDs(Record *Record, int OP_rLen, int col, int *RIDArray, bool execMode);

//int simpleProjection(int *ridArray, int OP_rLen, char *tableName, char ** columns, int numColumn, int **output, bool execMode);

//int uJoin(char* table1, char *column1, Record *R, int OP_rLen, char* table2, char *column2, Record *S, int sLen, PREDICATE_NODE * pRoot, int pType, Record ** Rout, bool execMode);

//int uNLJ(Record **input,int num_col, int rNumColumn, int OP_rLen,int sLen, PREDICATE_NODE * pRoot, Record ** Rout,bool execMode);

//if col==0, newRIDout[i]=oldRIDout[joinResult[i].rid];
//otherwise, newRIDout[i]=oldRIDout[joinResult[i].value];
void genOriginalRIDs(int* oldRIDout,Record * joinResult,int numResult,int col, int * newRIDout, bool execMode);

//int rAggregation(char* table1,char* columns,int* RIDout,int OP_rLen,char *aggType, Record **output, bool execMode);

//order by, return a permutation used for all rids.
int rOrder(char *tableName,char* columnName,int* RIDout,int OP_rLen,int* permutation, bool execMode);

int rPermutation(int *ridData, int OP_rLen, int* permutation, bool execMode);
#endif

