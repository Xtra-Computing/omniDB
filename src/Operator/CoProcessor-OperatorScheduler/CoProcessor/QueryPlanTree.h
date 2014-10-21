#ifndef QUERYPLANTREE_H
#define QUERYPLANTREE_H

#include "db.h"
#include "stdlib.h"
#include "QueryPlanNode.h"
#include "ExecStatus.h"
#include "ThreadOp.h"
#include <vector>
using namespace std;


class QueryPlanTree
{
protected:
public:
	void buildTree(char * str);
	ExecStatus* planStatus;
	void execute();
	QueryPlanTree()
	{
		root=NULL;
		planStatus=(ExecStatus*)malloc(sizeof(ExecStatus));
		planStatus->init();
		hasLock=false;
	}
	~QueryPlanTree();
	QueryPlanNode * QueryPlanTree::construct_plan_tree(char * str, int * index);
	QueryPlanNode * root;
	ThreadOp* getNextOp();
	vector<QueryPlanNode *> nodeVec;
	int curActiveNode;//in the vector.
	int totalNumNode;
	void Marshup(QueryPlanNode * node);
	cl_mem q_Rout;
	int q_numResult;
	bool hasLock;
};

#endif

