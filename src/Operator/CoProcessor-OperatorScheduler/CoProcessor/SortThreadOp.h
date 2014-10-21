#pragma once
#include "singularthreadop.h"

#include <vector>
using namespace std;
/*
if getNextOp is called, we advance the startUnSorted cursor; (require a lock?).
depending on the execution type, we advanced a different number of tuples.

*/

class SortThreadOp :
	public SingularThreadOp
{
public:
	SortThreadOp(OP_MODE opt);
	void init(cl_mem p_R, int p_rLen);
	~SortThreadOp(void);
	void execute();
	ThreadOp* getNextOp(EXEC_MODE eM);

	//for the workload and temporary results.
	int startUnSorted;
	int cpuChunkSize;
	int gpuChunkSize;
	vector<Record**> tempResultVec;
	
};
