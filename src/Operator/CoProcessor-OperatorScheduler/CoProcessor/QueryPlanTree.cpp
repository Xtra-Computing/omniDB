#include <string.h>
#include "QueryPlanTree.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "Helper.h"
#define ADA_START_TYPE (GROUP_BY)
#define ADA_END_TYPE (JOIN_HJ)
/////////////////////////////////////////////////////////////
extern double OP_LothresholdForGPUApp;
extern double OP_LothresholdForCPUApp;
extern double SpeedupGPUOverCPU_Operator[22];
extern double AddCPUBurden_OP[22];
extern double AddGPUBurden_OP[22];
extern double OP_UpCPUBurden;
extern double OP_LoCPUBurden;
extern double OP_UpGPUBurden;
extern double OP_LoGPUBurden;
extern double OP_CPUBurden;
extern double OP_GPUBurden;
extern CRITICAL_SECTION OP_CPUBurdenCS;
extern CRITICAL_SECTION OP_GPUBurdenCS;
void inline GPUBurdenDEC(const double burden){
		EnterCriticalSection(&(OP_GPUBurdenCS));
		OP_GPUBurden-=(burden);
		LeaveCriticalSection(&(OP_GPUBurdenCS));
}
void inline CPUBurdenDEC(const double burden){
		EnterCriticalSection(&(OP_CPUBurdenCS));
		OP_CPUBurden-=(burden);
		LeaveCriticalSection(&(OP_CPUBurdenCS));
}

void QueryPlanTree::buildTree(char * str)
{
	int i = 0;
	root = construct_plan_tree(str, &i);
	curActiveNode=0;
	totalNumNode=0;
	Marshup(root);	
}

QueryPlanTree::~QueryPlanTree()
{
	////printf("Some one call to delete tree\n");
	_ASSERTE( _CrtCheckMemory( ) );
	int i=0;
	for(i=0;i<totalNumNode;i++)
	{
		if(nodeVec[i]!=NULL){
			//_ASSERTE( _CrtCheckMemory( ) );
			////printf("delete nodeVec..%d\n",i);
			delete nodeVec[i];// no need to delete nodeVec??
			//_ASSERTE( _CrtCheckMemory( ) );
			////printf("delete nodeVec..%d succeed\n",i);
		}
		else{
			////printf("!!already delete!\n");
		}
	}
	planStatus->destory();
	free(planStatus);
	hasLock=false;
}

QueryPlanNode * QueryPlanTree::construct_plan_tree(char * str, int * index)
{
	int i, j = *index;
	
	 if (str[j] == '$')
	{
		*index = j + 2;
		return NULL;
	}


	QueryPlanNode * node = new QueryPlanNode();

	for (i = j; str[i] != '\0'; i++)
	{
		if (str[i] == ':')
			break;
	}
	node->initialNode((char*)(str + j),this->planStatus);
	i++;

	//recursive
	node->left = construct_plan_tree(str, &i);
	node->right = construct_plan_tree(str, &i);
	*index = i;
	return node;
}
//get an operator and execute it.
//the status should be updated into the query plan.
void QueryPlanTree::execute()
{
	ThreadOp* resultOp=getNextOp();//---->get the first Op.
	ThreadOp* previousOp=resultOp;
	QueryPlanNode* curNode=(QueryPlanNode*)(nodeVec[curActiveNode]);//-->get the first node:>corresponding to first Op
	while(resultOp!=NULL)
	{	
		//first, we get the type, next, we init the op.
		curNode=(QueryPlanNode*)(nodeVec[curActiveNode]);	
		curNode->initOp(resultOp->execMode);
		//int timer=genTimer(0);
		resultOp->execute();	
		//getTimer(timer);
		//double t=getTimer(timer);
		deschedule:
		if(resultOp->execMode){
		GPUBurdenDEC(AddGPUBurden_OP[resultOp->optType]);
		}
		else{
		CPUBurdenDEC(AddCPUBurden_OP[resultOp->optType]);
		}
		curNode->PostExecution(resultOp->execMode);
		previousOp=resultOp;
		resultOp=getNextOp();		
	}
	//store the result;
	q_Rout=previousOp->Rout;
	q_numResult=previousOp->numResult;
}



ThreadOp* QueryPlanTree::getNextOp()
{
	ThreadOp* resultOp=NULL;
	
	if(totalNumNode>curActiveNode)
	{	
		resultOp=((QueryPlanNode*)(nodeVec[curActiveNode]))->getNextOp();
		if(resultOp==NULL)
		{
			if((curActiveNode+1)==totalNumNode) //the last node, output result
			{
				QueryPlanNode* curNode=nodeVec[curActiveNode];
				int numResult=curNode->tOp->numResult;

#ifdef DEBUG
				Record *Rout=curNode->tOp->Rout;
				int i=0;
				cout<<numResult<<endl;
				if(numResult!=0)
				{
					for(i=0;i<numResult;i++)
					{
						cout<<Rout[i].rid<<", "<<Rout[i].value<<"\t";
					}
				}
#endif
			}
			else
			{
				//update the plan status here.
				curActiveNode++;
				if(totalNumNode>curActiveNode)
				resultOp=((QueryPlanNode*)(nodeVec[curActiveNode]))->getNextOp();
			}
		}
	}
	return resultOp;
}

void QueryPlanTree::Marshup(QueryPlanNode * node)
{
	QueryPlanNode * left=node->left;
	QueryPlanNode * right=node->right;
	if (left != NULL)
	{
		Marshup(left);
	}
	if (right != NULL)
	{
		Marshup(right);
	}
	nodeVec.push_back(node);
	totalNumNode++;
}

