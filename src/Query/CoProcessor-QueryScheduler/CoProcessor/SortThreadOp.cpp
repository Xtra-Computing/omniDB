#include "SortThreadOp.h"
#include "CPU_Dll.h"

#include "CoProcessor.h"

SortThreadOp::SortThreadOp(OP_MODE opt)
:SingularThreadOp(opt)
{
	
}

void SortThreadOp::init(cl_mem p_R, int p_rLen)
{
	R=p_R;
	Query_rLen=p_rLen;
}

SortThreadOp::~SortThreadOp(void)
{
}


void SortThreadOp::execute(EXEC_MODE eM)
{
	numResult=Query_rLen;	
//	ON_GPUONLY("SortThreadOp::execute");
	CL_CREATE(&Rout,sizeof(Record)*Query_rLen);
	CL_BitonicSortOnly(R,Query_rLen,Rout,256,512,eM);
//	ON_GPUONLY_DONE("SortThreadOp::execute");
	numResult=Query_rLen;
}


ThreadOp* SortThreadOp::getNextOp(EXEC_MODE eM)
{
	isFinished=true;
	return this;
}
