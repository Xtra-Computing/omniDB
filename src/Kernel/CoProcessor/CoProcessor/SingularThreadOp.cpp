#include "SingularThreadOp.h"
#include "CPU_Dll.h"


SingularThreadOp::SingularThreadOp(OP_MODE opt)
{
	optType=opt;
	R=NULL;
	Query_rLen=-1;
}

void SingularThreadOp::init(cl_mem p_R, int p_rLen)
{
	R=p_R;
	Query_rLen=p_rLen;
}

SingularThreadOp::~SingularThreadOp(void)
{
}


void SingularThreadOp::execute(EXEC_MODE eM)
{
	int result=0;
	//printf("SingularThreadOp::execute \n");
	switch(optType)
	{
		case AGG_SUM:
		{
			
			result=CL_AggSumOnly(R,Query_rLen,&Rout,256, 512,eM);						
		}break;
		case AGG_MAX:
		{
			
			result=CL_AggMaxOnly(R,Query_rLen,&Rout,256, 512,eM);			
		}break;
		case AGG_AVG:
		{
				if(Query_rLen==0)
					result=0;
				else
					
					result=CL_AggAvgOnly(R,Query_rLen,&Rout,256, 512,eM);			
		}break;
		case AGG_MIN:
		{
			
			result=CL_AggMinOnly(R,Query_rLen,&Rout,256, 512,eM);			
		}break;
		case SELECTION:
		{
				SelectionOp *sop=(SelectionOp*)this;
				sop->execute(eM);
		}break;
	}
	numResult=1;
	//printf("SingularThreadOp::execute done\n");
}

ThreadOp* SingularThreadOp::getNextOp(EXEC_MODE eM)
{
	isFinished=true;
	return this;
}



SelectionOp::SelectionOp(OP_MODE opt):
SingularThreadOp(opt)
{
	
}

void SelectionOp::init(cl_mem p_R, int p_rLen, int p_lowerKey, int p_higherKey)
{
	R=p_R;
	Query_rLen=p_rLen;
	//Kernel_bufferchecking(R,1000);
	lowerKey=p_lowerKey;
	higherKey=p_higherKey;
}

void SelectionOp::execute(EXEC_MODE eM)
{
		//printf("SelectionOp::execute\n");
	if(lowerKey==higherKey)
	{
		////printf("doing point selection, lowerKey is %d, Higher Key is %d",lowerKey,higherKey);
		//Kernel_bufferchecking(R,1000);
		//CL_PointSelectionOnly(cl_mem d_Rin, int rLen, int rangeSmallKey, cl_mem* d_Rout, 
															  //int numThreadPB, int numBlock,int _CPU_GPU );
//int CL_PointSelectionOnly(cl_mem d_Rin, int rLen, int rangeSmallKey, int rangeLargeKey, cl_mem* d_Rout, 
	//														  int numThreadPB, int numBlock,int _CPU_GPU )
		Kernel_bufferchecking(R,100);
		numResult=CL_PointSelectionOnly(R,Query_rLen,lowerKey, &Rout,256,512,eM);//pointselection canonly handle up to 100
		////printf("point selection finished\n");
	}
	else
	{
		////printf("doing range selection, lowerKey is %d, Higher Key is %d",lowerKey,higherKey);
		
			numResult=CL_RangeSelectionOnly(R,Query_rLen,lowerKey, higherKey,&Rout,256,512,eM);
	}
	//printf("SelectionOp::execute done\n");
}


ThreadOp* SelectionOp::getNextOp(EXEC_MODE eM)
{
	isFinished=true;
	return this;
}

/*
* Projection operator
*/
void ProjectionOp::execute(EXEC_MODE eM)
{
	//printf("ProjectionOp::execute\n");
	numResult=RIDLen;
	if(RIDLen>0)
	{
		CL_CREATE(&Rout, sizeof(Record)*RIDLen);
		CL_setRIDList(RIDList,RIDLen,Rout,256,64,eM);
		//Rout[i].rid=RIDList[i];
		CL_ProjectionOnly(R,Query_rLen,Rout,RIDLen,256,64,eM);	
		//GPUDEBUG_Record(Rout, RIDLen);
	}
	//printf("ProjectionOp::execute done\n");
/*	//ON_GPU("ProjectionOp::execute");
	this->Rout=new Record[RIDLen];
	this->numResult=RIDLen;
	int i=0;
	for(i=0;i<RIDLen;i++)
		Rout[i].rid=RIDList[i];
	
	CL_Projection(R,Query_rLen,Rout,RIDLen,256,256,eM);
	numResult=RIDLen;
	//ON_GPU_DONE("ProjectionOp::execute");*/
}
ProjectionOp::ProjectionOp(OP_MODE opt):
SingularThreadOp(opt)
{
}

void ProjectionOp::init(cl_mem p_R, int p_rLen, cl_mem pRIDList, int pRIDLen)
{
	R=p_R;
	//Kernel_bufferchecking(R,sizeof(Record)*10);
	Query_rLen=p_rLen;
	
	RIDList=pRIDList;
	RIDLen=pRIDLen;
}
ThreadOp* ProjectionOp::getNextOp(EXEC_MODE eM)
{
	isFinished=true;
	return this;
}