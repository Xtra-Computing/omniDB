#include <string.h>
#include "QueryPlanTree.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "Helper.h"
#define ADA_START_TYPE (GROUP_BY)
#define ADA_END_TYPE (JOIN_HJ)
extern bool isGPUavailable;
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
void QueryPlanTree::execute(EXEC_MODE eM)
{
	ThreadOp* resultOp=getNextOp(eM);
	ThreadOp* previousOp=resultOp;
	QueryPlanNode* curNode=(QueryPlanNode*)(nodeVec[curActiveNode]);
	EXEC_MODE tempEM=eM;
	while(resultOp!=NULL)
	{	
		//first, we get the type, next, we init the op.
		curNode=(QueryPlanNode*)(nodeVec[curActiveNode]);	
		//int timer=genTimer(curActiveNode);
		//genTimer(timer);
		curNode->initOp(eM);
		//double t=genTimer(timer);
		//printf("time spend in initOp is %lf\n",t);
		resultOp->execute(eM);	
		curNode->PostExecution(eM);
		previousOp=resultOp;
		resultOp=getNextOp(eM);		
	}
	//store the result;
	q_Rout=previousOp->Rout;
	q_numResult=previousOp->numResult;
}



ThreadOp* QueryPlanTree::getNextOp(EXEC_MODE eM)
{
	ThreadOp* resultOp=NULL;
	
	if(totalNumNode>curActiveNode)
	{	
		resultOp=((QueryPlanNode*)(nodeVec[curActiveNode]))->getNextOp(eM);
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
				resultOp=((QueryPlanNode*)(nodeVec[curActiveNode]))->getNextOp(eM);
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

