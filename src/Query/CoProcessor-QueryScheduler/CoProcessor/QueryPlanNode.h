#ifndef QUERYPLANNODE_H
#define QUERYPLANNODE_H

#include "db.h"
#include "ExecStatus.h"
#include "PredicateTree.h"
#include "ThreadOp.h"





class QueryPlanNode
{
public:
//attributes,
	QueryPlanNode * left;
	QueryPlanNode * right;
//	PREDICATE_NODE * nodeArray;
	PredicateTree * predicateRoot;
	ExecStatus *planStatus;
	OP_MODE optType;
	char * table1;
	char * table2;
	int ID0;
	int ID1;
	char ** columns;
	int num_col;
	int predicate_num;
	ThreadOp* tOp;
	NODE_STATUS nodeStatus;


	QueryPlanNode();
	~QueryPlanNode();


	void initialNode(char * str, ExecStatus *status);
	void execute(int nodeLevel);

//methods,

	void getSelOprand(int* lowerKey, int* higherKey);
	OP_MODE getJoinType(void);
	ThreadOp* getNextOp(EXEC_MODE eM);
	void createOp();
	void initOp(EXEC_MODE eM);
	void PostExecution(EXEC_MODE eM);
};

#endif

