#include "SingularThreadOp.h"
#include "CPU_Dll.h"


SingularThreadOp::SingularThreadOp(OP_MODE opt)
{
	optType=opt;
	R=NULL;
	OP_rLen=-1;
}

void SingularThreadOp::init(cl_mem p_R, int p_rLen)
{
	R=p_R;
	OP_rLen=p_rLen;
}

SingularThreadOp::~SingularThreadOp(void)
{
}


void SingularThreadOp::execute()
{
	EXEC_MODE eM=this->execMode;
	int result=0;
	//ON_GPU("SingularThreadOp::execute");
	switch(optType)
	{
		case AGG_SUM:
		{
			//OP_bufferchecking(R,100);
			result=CL_AggSumOnly(R,OP_rLen,&Rout,256, 512,eM);						
		}break;
		case AGG_MAX:
		{		
			//OP_bufferchecking(R,100);
			result=CL_AggMaxOnly(R,OP_rLen,&Rout,256, 512,eM);			
		}break;
		case AGG_AVG:
		{
				if(OP_rLen==0)
					result=0;
				else
					//OP_bufferchecking(R,100);
					result=CL_AggAvgOnly(R,OP_rLen,&Rout,256, 512,eM);			
		}break;
		case AGG_MIN:
		{
			//OP_bufferchecking(R,100);
			result=CL_AggMinOnly(R,OP_rLen,&Rout,256, 512,eM);			
		}break;
		case SELECTION:
		{
			//OP_bufferchecking(R,100);
				SelectionOp *sop=(SelectionOp*)this;
				sop->execute();
		}break;
	}
	numResult=1;
	//ON_GPU_DONE("SingularThreadOp::execute");
}

ThreadOp* SingularThreadOp::getNextOp(EXEC_MODE eM)
{
	this->execMode=eM;
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
	OP_rLen=p_rLen;
	//OP_bufferchecking(R,1000);
	lowerKey=p_lowerKey;
	higherKey=p_higherKey;
}

void SelectionOp::execute()
{
	EXEC_MODE eM=this->execMode;
	//	ON_GPU("SelectionOp::execute");
	if(lowerKey==higherKey)
	{
		//printf("doing point selection, lowerKey is %d, Higher Key is %d",lowerKey,higherKey);
		//OP_bufferchecking(R,1000);
		//CL_PointSelectionOnly(cl_mem d_Rin, int rLen, int rangeSmallKey, cl_mem* d_Rout, 
															  //int numThreadPB, int numBlock,int _CPU_GPU );
//int CL_PointSelectionOnly(cl_mem d_Rin, int rLen, int rangeSmallKey, int rangeLargeKey, cl_mem* d_Rout, 
	//														  int numThreadPB, int numBlock,int _CPU_GPU )
		numResult=CL_PointSelectionOnly(R,OP_rLen,lowerKey, &Rout,256,512,eM);//pointselection canonly handle up to 100
		////printf("point selection finished\n");
	}
	else
	{
		//printf("doing range selection, lowerKey is %d, Higher Key is %d",lowerKey,higherKey);
		//OP_bufferchecking(R,1000);
		numResult=CL_RangeSelectionOnly(R,OP_rLen,lowerKey, higherKey,&Rout,256,512,eM);
	}
	//ON_GPU_DONE("SelectionOp::execute");
}


ThreadOp* SelectionOp::getNextOp(EXEC_MODE eM)
{
	this->execMode=eM;
	isFinished=true;
	return this;
}

/*
* Projection operator
*/
void ProjectionOp::execute()
{
	EXEC_MODE eM=this->execMode;
	//ON_GPUONLY("ProjectionOp::execute");
	numResult=RIDLen;
	if(RIDLen>0)
	{
		CL_CREATE(&Rout, sizeof(Record)*RIDLen);
		CL_setRIDList(RIDList,RIDLen,Rout,256,64,eM);
		//Rout[i].rid=RIDList[i];
		CL_ProjectionOnly(R,OP_rLen,Rout,RIDLen,256,64,eM);	
		//GPUDEBUG_Record(Rout, RIDLen);
	}
	//ON_GPUONLY_DONE("ProjectionOp::execute");
/*	//ON_GPU("ProjectionOp::execute");
	this->Rout=new Record[RIDLen];
	this->numResult=RIDLen;
	int i=0;
	for(i=0;i<RIDLen;i++)
		Rout[i].rid=RIDList[i];
	
	CL_Projection(R,OP_rLen,Rout,RIDLen,256,256,eM);
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
	//OP_bufferchecking(R,sizeof(Record)*10);
	OP_rLen=p_rLen;
	
	RIDList=pRIDList;
	RIDLen=pRIDLen;
}
ThreadOp* ProjectionOp::getNextOp(EXEC_MODE eM)
{
	this->execMode=eM;
	isFinished=true;
	return this;
}