#pragma once
#include "SortThreadOp.h"

class GroupByThreadOp: public SingularThreadOp
{
public:
	int numGroup;
	int* startPos;
	GroupByThreadOp(OP_MODE opt);
	void init(cl_mem p_R, int p_rLen);
	~GroupByThreadOp(void);
	void execute(EXEC_MODE eM);
	ThreadOp* getNextOp(EXEC_MODE eM);
};


class AggAfterGroupByThreadOp: public SingularThreadOp
{
public:
	int numGroup;
	cl_mem startPos;
	cl_mem RHavingGroupBy;
	int rLenHavingGroupBy;
	AggAfterGroupByThreadOp(OP_MODE opt);
	void init(cl_mem p_R, int p_rLen, cl_mem pRHavingGroupBy, int prLenHavingGroupBy,  int numG, cl_mem pStartPos);
	~AggAfterGroupByThreadOp(void);
	void execute(EXEC_MODE eM);
	ThreadOp* getNextOp(EXEC_MODE eM);
};
