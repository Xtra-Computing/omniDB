#pragma once
#include "threadop.h"
#include "CPU_Dll.h"

class BinaryThreadOp :
	public ThreadOp
{
public:
	cl_mem S;
	int sLen;
	BinaryThreadOp(OP_MODE opt);
	void init(cl_mem p_R, int p_rLen, cl_mem p_S, int p_sLen);
	~BinaryThreadOp(void);
	void execute();
	ThreadOp* getNextOp(EXEC_MODE eM);
};

class IndexJoinThreadOp: public BinaryThreadOp
{
public:
	CUDA_CSSTree *h_tree;
	IndexJoinThreadOp(OP_MODE opt);
	void init(cl_mem p_R, int p_rLen, cl_mem p_S, int p_sLen);
	~IndexJoinThreadOp(void);
	void execute();
	ThreadOp* getNextOp(EXEC_MODE eM);

};
