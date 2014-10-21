#include "common.h"
void projectionImpl(cl_mem d_Rin, int rLen, cl_mem d_projTable, int pLen, 
					int numThread, int numBlock,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void testProjection( int rLen, int pLen, int numThread = 32, int numBlock = 64 ) ;