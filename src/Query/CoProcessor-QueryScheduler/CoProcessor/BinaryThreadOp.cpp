#include "BinaryThreadOp.h"
#include "CPU_Dll.h"
#include "CoProcessor.h"
#include "OpenCL_DLL.h"
BinaryThreadOp::BinaryThreadOp(OP_MODE opt)
{
	optType=opt;
}


void BinaryThreadOp::init(cl_mem p_R, int p_rLen, cl_mem p_S, int p_sLen)
{
	R=p_R;
	Query_rLen=p_rLen;
	S=p_S;
	sLen=p_sLen;
}

BinaryThreadOp::~BinaryThreadOp(void)
{
}


void BinaryThreadOp::execute(EXEC_MODE eM)
{
		//ON_GPU("BinaryThreadOp::execute");
		switch(optType)
		{
			case JOIN_NINLJ:
				{
					//numResult=CL_ninlj(R,Query_rLen,S,sLen,&Rout,eM);		
					cout<<"you should not be here"<<endl;
				}break;
			case JOIN_INLJ:
				{
					cout<<"you should not be here"<<endl;
				}break;
			case JOIN_SMJ:
				{				
					numResult=CL_smjOnly(R,Query_rLen,S,sLen,&Rout,eM);
				}break;
			case JOIN_HJ:
				{
					
					numResult=CL_hjOnly(R,Query_rLen,S,sLen,&Rout,eM);
				}break;
		}
		//ON_GPU_DONE("BinaryThreadOp::execute");
}


ThreadOp* BinaryThreadOp::getNextOp(EXEC_MODE eM)
{
	this->execMode=eM;
	isFinished=true;
	return this;
}
/*
* for the indexed nlj.
*/
IndexJoinThreadOp::IndexJoinThreadOp(OP_MODE opt):BinaryThreadOp(opt)
{
}


void IndexJoinThreadOp::init(cl_mem p_R, int p_rLen,cl_mem p_S, int p_sLen)
{
	R=p_R;
	Query_rLen=p_rLen;
	S=p_S;
	sLen=p_sLen;
}

IndexJoinThreadOp::~IndexJoinThreadOp(void)
{

}

void IndexJoinThreadOp::execute(EXEC_MODE eM)
{
//	ON_GPU("IndexJoinThreadOp::execute");
	//Kernel_bufferchecking(R,100);
	//Kernel_bufferchecking(S,100);
	numResult=CL_inljOnly(R,Query_rLen,&h_tree,S,sLen,&Rout,eM);
	//Kernel_bufferchecking(Rout,sizeof(Record)*numResult);
//	ON_GPU_DONE("IndexJoinThreadOp::execute");
}

ThreadOp* IndexJoinThreadOp::getNextOp(EXEC_MODE eM)
{
	this->execMode=eM;
	isFinished=true;
	return this;
}
