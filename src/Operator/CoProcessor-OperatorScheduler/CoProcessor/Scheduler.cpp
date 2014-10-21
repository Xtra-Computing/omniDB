#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "QueryPlanNode.h"
#include "QueryPlanTree.h"
#include "QP_Utility.h"
#include "time.h"
#include "assert.h"
#include "Database.h"
#include "SingularThreadOp.h"
#include "SortThreadOp.h"
#include "BinaryThreadOp.h"
#include "GroupByThreadOp.h"
#include <iostream>
#include "CoProcessorTest.h"
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
extern CRITICAL_SECTION preEMCS;

//#define Greedy
static int preEm=EXEC_CPU;
void inline GPUBurdenINC(const double burden){
		EnterCriticalSection(&(OP_GPUBurdenCS));
		OP_GPUBurden+=(burden);
		LeaveCriticalSection(&(OP_GPUBurdenCS));
}
void inline CPUBurdenINC(const double burden){
		EnterCriticalSection(&(OP_CPUBurdenCS));
		OP_CPUBurden+=(burden);
		LeaveCriticalSection(&(OP_CPUBurdenCS));
}

EXEC_MODE OPScheduler(OP_MODE _optType)
{
/*OPERATOR SCHEDULER*/
	EXEC_MODE eM;
#ifdef Greedy
	if((OP_CPUBurden+AddCPUBurden_OP[_optType])<(OP_GPUBurden+AddGPUBurden_OP[_optType]))
	{
		eM=EXEC_CPU;
		CPUBurdenINC(AddCPUBurden_OP[_optType]);
	}else{
		eM=EXEC_GPU;
		GPUBurdenINC(AddGPUBurden_OP[_optType]);
	}
#else
		EnterCriticalSection(&(preEMCS));
	 	if(preEm==EXEC_CPU){
			eM=EXEC_GPU;
			GPUBurdenINC(AddGPUBurden_OP[_optType]);
			preEm=EXEC_GPU;
		}
		else{
			eM=EXEC_CPU;
			CPUBurdenINC(AddCPUBurden_OP[_optType]);
			preEm=EXEC_CPU;
		}
		LeaveCriticalSection(&(preEMCS));
#endif
	return eM;
}
