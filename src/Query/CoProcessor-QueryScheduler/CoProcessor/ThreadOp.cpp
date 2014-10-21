#include "ThreadOp.h"
#include "CPU_Dll.h"
#include <iostream>
#include "OpenCL_DLL.h"
using namespace std;

ThreadOp::ThreadOp()
{
	isFinished=false;
	numResult=Query_rLen;
	Rout=NULL;
}

ThreadOp::~ThreadOp()
{
	clReleaseMemObject(this->R);
}


