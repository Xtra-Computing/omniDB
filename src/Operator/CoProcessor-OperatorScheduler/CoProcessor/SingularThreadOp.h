#pragma once
#include "threadop.h"

class SingularThreadOp :
	public ThreadOp
{
public:
	void init(cl_mem p_R, int p_rLen);
	SingularThreadOp(OP_MODE opt);
	~SingularThreadOp(void);
	void execute();
	ThreadOp* getNextOp(EXEC_MODE eM);
};

class SelectionOp: public SingularThreadOp
{
public:
	int lowerKey;
	int higherKey;
	void execute();
	EXEC_MODE eM;
	void init(cl_mem p_R, int p_rLen, int lowerKey, int higherKey);
	SelectionOp(OP_MODE opt);
	ThreadOp* getNextOp(EXEC_MODE eM);
};

class ProjectionOp:public SingularThreadOp
{
public:
	cl_mem RIDList;
	int RIDLen;
	void execute();
	void init(cl_mem p_R, int p_rLen, cl_mem RIDList, int RIDLen);
	ProjectionOp(OP_MODE opt);
	ThreadOp* getNextOp(EXEC_MODE eM);
};
