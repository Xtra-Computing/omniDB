#ifndef _GPU_DLL_H_
#define _GPU_DLL_H_
#include "common.h"
#include "CSSTree.h"
//#region Indexing Tree Types
typedef struct {
	IKeyType keys[TREE_NODE_SIZE];
} IDirectoryNode;	
typedef struct {
	Record records[TREE_NODE_SIZE];
} IDataNode;
typedef struct
{
	//IDataNode* data;
	cl_mem data;
	unsigned int nDataNodes;

	//IDirectoryNode* dir;
	cl_mem dir;
	unsigned int nDirNodes;
} CUDA_CSSTree;
void __declspec( dllexport  )  CL_CREATE(cl_mem *mem,cl_int size);
void __declspec( dllexport  )  CL_DESTORY(cl_mem *mem);
void __declspec( dllexport  ) CopyCPUToGPU(cl_mem to, void* from, size_t size);
void __declspec( dllexport  ) CopyGPUToCPU(cl_mem from, void* to, size_t size);
void __declspec( dllexport  ) CopyGPUToGPU(cl_mem from, cl_mem to, size_t size);
extern "C" void __declspec( dllexport  )  CL_setRIDList(cl_mem h_RIDList, int rLen, cl_mem h_destRin, int numThreadPB, int numBlock,int _CPU_GPU);
extern "C" void __declspec( dllexport  )  CL_getRIDList( cl_mem h_Rin, int rLen, cl_mem* h_RIDList,int numThreadPB, int numBlock,int _CPU_GPU);
extern "C" void __declspec( dllexport  )  CL_setValueList(cl_mem h_ValueList, int rLen, cl_mem h_destRin, int numThreadPB, int numBlock,int _CPU_GPU);
extern "C" void __declspec( dllexport  )  CL_RadixSortOnly(cl_mem d_Rin, int rLen,int numThread, int numBlock, int _CPU_GPU);
extern "C" void __declspec( dllexport  )  CL_BitonicSortOnly(cl_mem d_Rin, int rLen,cl_mem d_Rout,int numThread, int numBlock, int _CPU_GPU);
extern "C" void __declspec( dllexport  )  CL_getValueList( cl_mem h_Rin, int rLen, cl_mem* h_ValueList,int numThreadPB, int numBlock,int _CPU_GPU);
int __declspec( dllexport  )  CL_AggMaxOnly( cl_mem d_Rin, int rLen, cl_mem* d_Rout,
	int numThread, int numBlock , int _CPU_GPU);
int __declspec( dllexport  )  CL_AggSumOnly( cl_mem d_Rin, int rLen, cl_mem* d_Rout,
	int numThread, int numBlock , int _CPU_GPU);
int __declspec( dllexport  )  CL_AggAvgOnly( cl_mem d_Rin, int rLen, cl_mem* d_Rout,
	int numThread, int numBlock , int _CPU_GPU);
int __declspec( dllexport  )  CL_AggMinOnly( cl_mem d_Rin, int rLen, cl_mem* d_Rout,
	int numThread, int numBlock , int _CPU_GPU);

extern "C" int __declspec( dllexport  )  CL_smjOnly(cl_mem d_R, int rLen, cl_mem d_S, int sLen, cl_mem*  h_Joinout,int _CPU_GPU);
extern "C" int __declspec( dllexport  )  CL_PointSelectionOnly(cl_mem d_Rin, int rLen, int matchingKeyValue, cl_mem* d_Rout, 
	int numThreadPB, int numBlock,int _CPU_GPU );

extern "C" int __declspec( dllexport  )  CL_RangeSelectionOnly(cl_mem d_Rin, int rLen, int rangeSmallKey, int rangeLargeKey, cl_mem* d_Rout, 
	int numThreadPB, int numBlock,int _CPU_GPU );

extern "C" void __declspec( dllexport  )  CL_ProjectionOnly(cl_mem d_Rin,int rLen, cl_mem d_projTable, int pLen, 
	int numThread, int numBlock , int _CPU_GPU);
extern "C" int __declspec( dllexport  )  CL_hjOnly(cl_mem d_R, int rLen, cl_mem d_S, int sLen, cl_mem* h_Rout ,int _CPU_GPU);

extern "C" int __declspec( dllexport  )  CL_inljOnly( cl_mem h_Rin, int rLen, CUDA_CSSTree** h_tree,cl_mem h_Sin, int sLen, cl_mem* h_Rout, int _CPU_GPU );

void cuda_search_index_usingKeys(cl_mem g_data, unsigned int nDataNodes, cl_mem g_dir, 
	unsigned int nDirNodes, cl_mem g_keys, cl_mem g_locations, 
	unsigned int nSearchKeys,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void cuda_search_indexImpl(cl_mem d_data, unsigned int nDataNodes, cl_mem d_dir, 
	unsigned int nDirNodes,cl_mem d_keys, cl_mem d_locations, unsigned int nSearchKeys,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
int gpu_constructCSSTreeImpl(cl_mem d_Rin, int rLen, CUDA_CSSTree **h_tree,int *index,cl_event *eventList,cl_kernel *Kernel, int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
int cuda_join_after_search(cl_mem d_R, int rLen, cl_mem d_S, cl_mem d_locations, 
	unsigned int sLen, 	cl_mem* pd_Results,int *index,cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);

///////////////////////////////////////////////////////////////////////////////////
//_
////////////////////////////////////////////////////////////////////////////////////

//selection
extern "C" int __declspec( dllexport  )  CL_PointSelectionOnly(cl_mem d_Rin, int rLen, int matchingKeyValue, cl_mem* d_Rout, 
	int numThreadPB, int numBlock,int _CPU_GPU );
extern "C" int __declspec( dllexport  )  CL_PointSelection( Record* h_Rin, int rLen, int matchingKeyValue, Record** h_Rout, 
	int numThreadPB, int numBlock, int _CPU_GPU);
extern "C"  int __declspec( dllexport  )  CL_RangeSelection(Record* h_Rin, int rLen, int rangeSmallKey, int rangeLargeKey, Record** h_Rout, 
	int numThreadPB , int numBlock, int _CPU_GPU);

//projection
extern "C"  void __declspec( dllexport  )  CL_Projection( Record * h_Rin, int rLen, Record* h_projTable, int pLen, 
	int numThread, int numBlock , int _CPU_GPU);


//Singular
extern "C"  int __declspec( dllexport  )  CL_AggMax( Record * h_Rin, int rLen, Record** h_Rout,
	int numThread, int numBlock, int _CPU_GPU);

extern "C"  int __declspec( dllexport  )  CL_AggMin(Record * h_Rin, int rLen, Record** h_Rout,
	int numThread, int numBlock, int _CPU_GPU);


extern "C"  int __declspec( dllexport  )  CL_AggSum( Record * h_Rin, int rLen, Record** h_Rout,
	int numThread, int numBlock, int _CPU_GPU);

extern "C"  int __declspec( dllexport  )  CL_AggAvg( Record * h_Rin, int rLen, Record** h_Rout,
	int numThread, int numBlock, int _CPU_GPU);

//group by
extern "C"  int __declspec( dllexport  )  CL_GroupBy(Record * h_Rin, int rLen, Record* h_Rout, int** h_startPos, 
	int numThread , int numBlock, int _CPU_GPU);

//agg after group by
extern "C"  void __declspec( dllexport  )  CL_agg_max_afterGroupBy( Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, int numThread,int _CPU_GPU);

extern "C"  void __declspec( dllexport  )  CL_agg_min_afterGroupBy( Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, int numThread,int _CPU_GPU);

extern "C"  void __declspec( dllexport  )  CL_agg_sum_afterGroupBy(Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, int numThread,int _CPU_GPU);

extern "C" void __declspec( dllexport  )  CL_agg_avg_afterGroupBy(Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, int numThread,int _CPU_GPU);
//for joins

//sort
extern "C"   void __declspec( dllexport  )  CL_RadixSort(Record* h_Rin, int rLen, Record* h_Rout,int numThread,int numBlock, int _CPU_GPU);

//join

extern "C"  int __declspec( dllexport  )  CL_ninlj(Record* h_R, int rLen, Record* h_S, int sLen, Record** h_Rout, int _CPU_GPU );

extern "C"  int	__declspec( dllexport  )  CL_smj( Record* h_R, int rLen, Record* h_S, int sLen, Record** h_Joinout, int _CPU_GPU );

extern "C" int __declspec( dllexport  )  CL_hj( Record* h_R, int rLen, Record* h_S, int sLen, Record** h_Rout, int _CPU_GPU );

extern "C" int __declspec( dllexport  )  CL_inlj( Record* h_Rin, int rLen, CUDA_CSSTree** h_tree, Record* h_Sin, int sLen, Record** h_Rout, int _CPU_GPU );

extern "C"  int __declspec( dllexport  )  CL_mj( void * h_Rin, int rLen, Record* h_Sin, int sLen, Record** h_Joinout, int _CPU_GPU );
extern "C" __declspec( dllexport  ) void EngineStart(bool handShake,int _KernelSchedule);
extern "C" __declspec( dllexport  )  void EngineStop();
#endif

