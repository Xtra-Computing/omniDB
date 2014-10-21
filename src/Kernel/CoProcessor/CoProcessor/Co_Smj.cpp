#include "CoProcessor.h"
#include "MyThreadPoolCop.h"
#include "CPU_Dll.h"
#include <vector>
using namespace std;

int CO_Smj(Record *R, int rLen, Record *S, int sLen, Record** Rout)
{
	int result=0;
	Record *RTempOut=new Record[rLen];
	Record *STempOut=new Record[sLen];
	CO_Sort(R,rLen,RTempOut);
	CO_Sort(S,sLen,STempOut);
	result=CPU_MergeJoin(RTempOut,rLen, STempOut,sLen,Rout,OMP_NUM_MERGE_THREAD);
	delete RTempOut;
	delete STempOut;
	return result;
}