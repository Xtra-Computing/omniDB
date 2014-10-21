#include "common.h"

#define REDUCE_SUM (0)
#define REDUCE_MAX (1)
#define REDUCE_MIN (2)
#define REDUCE_AVERAGE (3)

#define SPLIT (4)
#define PARTITION (5)

void aggAfterGroupByImpl(cl_mem d_Rin, int rLen, cl_mem d_startPos, int numGroups, cl_mem d_Ragg, cl_mem d_aggResults, int OPERATOR, int numThread,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void testAggAfterGroupByImpl( int rLen, int OPERATOR, int numThread, int numBlock);