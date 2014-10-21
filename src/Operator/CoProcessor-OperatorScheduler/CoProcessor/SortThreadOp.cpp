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
	OP_rLen=p_rLen;
}

SortThreadOp::~SortThreadOp(void)
{
}


void SortThreadOp::execute()
{
	EXEC_MODE eM=this->execMode;
	numResult=OP_rLen;	
//	ON_GPUONLY("SortThreadOp::execute");
	CL_CREATE(&Rout,sizeof(Record)*OP_rLen);
	CL_BitonicSortOnly(R,OP_rLen,Rout,256,512,eM);
//	ON_GPUONLY_DONE("SortThreadOp::execute");
	numResult=OP_rLen;
}


ThreadOp* SortThreadOp::getNextOp(EXEC_MODE eM)
{
	this->execMode=eM;
	isFinished=true;
	return this;
}
