#include "ThreadOp.h"
#include "CPU_Dll.h"
#include <iostream>
#include "OpenCL_DLL.h"
using namespace std;

ThreadOp::ThreadOp()
{
	isFinished=false;
	numResult=OP_rLen;
	Rout=NULL;
}

ThreadOp::~ThreadOp()
{
	clReleaseMemObject(this->R);
}


