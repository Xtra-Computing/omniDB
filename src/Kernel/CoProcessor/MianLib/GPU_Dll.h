#ifndef _GPU_DLL_H_
#define _GPU_DLL_H_

#include "cuCSSTree.h"
#include "hashTable.h"

typedef struct
{
	IDataNode* data;
	unsigned int nDataNodes;

	IDirectoryNode* dir;
	unsigned int nDirNodes;

	int search(int key, Record** Rout)
	{
		return 0;
	}
	void print()
	{
		
	}
} CUDA_CSSTree;



///////////////////////////////////////////////////////////////////////////////////
//CL_
////////////////////////////////////////////////////////////////////////////////////

//selection
int CL_PointSelection( Record* h_Rin, int rLen, int matchingKeyValue, Record** h_Rout, 
															  int numThreadPB = 32, int numBlock = 256,int CPU_GPU=0);

int CL_RangeSelection( Record* h_Rin, int rLen, int rangeSmallKey, int rangeLargeKey, Record** h_Rout, 
															  int numThreadPB = 64, int numBlock = 512,int CPU_GPU=0);

//projection
void CL_Projection( Record* h_Rin, int rLen, Record* h_projTable, int pLen, 
														   int numThread = 256, int numBlock = 256 ,int CPU_GPU=0);


//aggregation
int CL_AggMax( Record* h_Rin, int rLen, Record** d_Rout,
													  int numThread = 256, int numBlock = 1024 ,int CPU_GPU=0);

int CL_AggMin( Record* h_Rin, int rLen, Record** d_Rout, 
													  int numThread = 256, int numBlock = 1024 ,int CPU_GPU=0);

int CL_AggSum( Record* h_Rin, int rLen, Record** d_Rout, 
													  int numThread = 256, int numBlock = 1024 ,int CPU_GPU=0);

int CL_AggAvg( Record* h_Rin, int rLen, Record** d_Rout, 
													  int numThread = 256, int numBlock = 1024,int CPU_GPU=0 );

//group by
int	CL_GroupBy( Record* h_Rin, int rLen, Record* h_Rout, int** h_startPos, 
					int numThread = 64, int numBlock = 1024,int CPU_GPU=0 );

//agg after group by
void CL_agg_max_afterGroupBy( Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, 
								  int numThread = 256,int CPU_GPU=0 ); 

void CL_agg_min_afterGroupBy( Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, 
								  int numThread = 256,int CPU_GPU=0 ); 

void CL_agg_sum_afterGroupBy( Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, 
								  int numThread = 256,int CPU_GPU=0 ); 

void CL_agg_avg_afterGroupBy( Record* h_Rin, int rLen, int* h_startPos, int numGroups, Record* h_Ragg, int* h_aggResults, 
								  int numThread = 256,int CPU_GPU=0 ); 

//data structure
void CL_BuildHashTable( Record* h_R, int rLen, int intBits, Bound* h_bound );

int CL_BuildTreeIndex( Record* h_Rin, int rLen, CUDA_CSSTree** tree );

int CL_HashSearch( Record* h_R, int rLen, Bound* h_bound, int inBits, Record* h_S, int sLen, Record** h_Rout, 
														  int numThread = 512 ,int CPU_GPU=0);

int CL_TreeSearch( Record* h_Rin, int rLen, CUDA_CSSTree* tree, Record* h_Sin, int sLen, Record** h_Rout,int CPU_GPU=0 );

//for joins

//sort
void CL_bitonicSort( Record* h_Rin, int rLen, Record* h_Rout, 
															int numThreadPB = 128, int numBlock = 1024 ,int CPU_GPU=0);

void CL_QuickSort( Record* h_Rin, int rLen, Record* h_Rout ,int CPU_GPU=0);

void CL_RadixSort( Record* h_Rin, int rLen, Record* h_Rout ,int CPU_GPU=0);

//join

int CL_ninlj( Record* h_R, int rLen, Record* h_S, int sLen, Record** h_Rout ,int CPU_GPU=0);

int	CL_smj( Record* h_R, int rLen, Record* h_S, int sLen, Record** h_Joinout ,int CPU_GPU=0);

int CL_hj( Record* h_Rin, int rLen, Record* d_Sin, int sLen, Record** h_Rout,int CPU_GPU=0 );

int CL_inlj( Record* h_Rin, int rLen, CUDA_CSSTree* h_tree, Record* h_Sin, int sLen, Record** h_Rout ,int CPU_GPU=0);

int CL_mj( Record* h_Rin, int rLen, Record* h_Sin, int sLen, Record** h_Joinout,int CPU_GPU=0 );

//partition
void CL_Partition( Record* h_Rin, int rLen, int numPart, Record* d_Rout, int* d_startHist, 
														  int numThreadPB = -1, int numBlock = -1 ,int CPU_GPU=0);

/*
//Interface 1: get all RIDs into an array. You need to allocate d_RIDList. 
void GPUOnly_getRIDList(Record* d_Rin, int rLen, int** d_RIDList, int numThreadPerBlock = 512, int numBlock = 256);

// Interface 2: copy a relation to another relation. You need to allocate d_destRin.
void GPUOnly_copyRelation(Record* d_srcRin, int rLen, Record** d_destRin, int numThreadPerBlock = 512, int numBlock = 256);

// Interface3: set the RID according to the RID list.
void GPUOnly_setRIDList(int* d_RIDList, int rLen, Record* d_destRin, int numThreadPerBlock = 512, int numBlock = 256);

void GPUOnly_setValueList(int* d_ValueList, int rLen, Record* d_destRin, int numThreadPerBlock = 512, int numBlock = 256);

void GPUOnly_getValueList(Record* d_Rin, int rLen, int** d_ValueList, int numThreadPerBlock = 512, int numBlock = 256);
*/
void resetGPU();
 
#endif

