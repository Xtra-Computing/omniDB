#include "GroupByThreadOp.h"
#include "CPU_Dll.h"


GroupByThreadOp::GroupByThreadOp(OP_MODE opt):
SingularThreadOp(opt)
{
	numGroup=-1;
	startPos=NULL;
}

void GroupByThreadOp::init(cl_mem p_R, int p_rLen)
{
	R=p_R;
	OP_rLen=p_rLen;
	numGroup=-1;
	startPos=NULL;
}

GroupByThreadOp::~GroupByThreadOp(void)
{
}



void GroupByThreadOp::execute()
{
/*	//ON_GPU("GroupByThreadOp::execute");
	Rout=new Record[OP_rLen];
	
	numGroup=CL_GroupBy(R,OP_rLen,Rout,&startPos,256,64,eM);
	numResult=OP_rLen;
	//ON_GPU_DONE("GroupByThreadOp::execute");*/
}

ThreadOp* GroupByThreadOp::getNextOp(EXEC_MODE eM)
{
	this->execMode=eM;
	isFinished=true;
	return this;
}


/*
* aggregation after group by.
*/

AggAfterGroupByThreadOp::AggAfterGroupByThreadOp(OP_MODE opt):
SingularThreadOp(opt)
{
	
}


void AggAfterGroupByThreadOp::init(cl_mem p_R, int p_rLen, cl_mem pRHavingGroupBy, 
		  int prLenHavingGroupBy, int numG, cl_mem pStartPos)
{
	R=p_R;
	OP_rLen=p_rLen;
	numGroup=numG;
	startPos=pStartPos;
	RHavingGroupBy=pRHavingGroupBy;
	rLenHavingGroupBy=prLenHavingGroupBy;
}

AggAfterGroupByThreadOp::~AggAfterGroupByThreadOp(void)
{
}


void AggAfterGroupByThreadOp::execute()
{

		ON_GPU("AggAfterGroupByThreadOp::execute");
//!!!!not ready yet!!
	/*	
	//	int* tempResult=new int[numGroup];
		switch(optType)
		{
			
			case AGG_SUM_AFTER_GROUP_BY:
				{
		
			//		CL_agg_sum_afterGroupBy(RHavingGroupBy,rLenHavingGroupBy,startPos,numGroup,R,tempResult,256,eM);					
				}break;
			case AGG_MAX_AFTER_GROUP_BY:
				{
	//				CL_agg_max_afterGroupBy(RHavingGroupBy,rLenHavingGroupBy,startPos,numGroup,R,tempResult,256,eM);
				}break;
			case AGG_AVG_AFTER_GROUP_BY:
				{
//					CL_agg_avg_afterGroupBy(RHavingGroupBy,rLenHavingGroupBy,startPos,numGroup,R,tempResult,256,eM);
				}break;
			case AGG_MIN_AFTER_GROUP_BY:
				{
	//				CL_agg_min_afterGroupBy(RHavingGroupBy,rLenHavingGroupBy,startPos,numGroup,R,tempResult,256,eM);
				}break;
		}
Rout=new Record[numGroup];
		int i=0;
		for(i=0;i<numGroup;i++)
		{
			Rout[i].rid=i;
			Rout[i].value=tempResult[i];
		}
		numResult=numGroup;
		delete tempResult;
		ON_GPU_DONE("AggAfterGroupByThreadOp::execute");
		*/
}

ThreadOp* AggAfterGroupByThreadOp::getNextOp(EXEC_MODE eM)
{
	this->execMode=eM;
	isFinished=true;
	return this;
}