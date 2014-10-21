#ifndef CPU_DLL_H
#define CPU_DLL_H
#include "common.h"
#include "CC_CSSTree.h"

#define NUM_CORE_PER_CPU 4

//void generateRand(Record *R, int max, int rLen, int seed);
//for testing only
int CPU_Sum(int a, int b);
void testDLL(int argc, char **argv);
//selection
int CPU_PointSelection(Record* Rin, int rLen, int matchingKeyValue, Record **Rout, int numThread);
int CPU_RangeSelection(Record* Rin, int rLen, int rangeSmallKey, int rangeLargeKey, Record **Rout,int numThread);

//projection, the projTable contains the RID list to fetch the tuple from baseTable.
void CPU_Projection(Record* baseTable, int rLen, Record* projTable, int pLen, int numThread);

//aggregation
int CPU_AggAvg(Record *R, int rLen, int numThread);
int CPU_AggMin(Record *R, int rLen, int numThread);
int CPU_AggMax(Record *R, int rLen, int numThread);
int CPU_AggSum(Record *R, int rLen, int numThread);

//groupby
int CPU_GroupBy(Record*R, int rLen, Record* Rout, int** d_startPos, int numThread);
//agg after group by
void CPU_agg_max_afterGroupBy(Record *Rin, int rLen, int* d_startPos, int numGroups,
							  Record * RinAggOrig, int* d_aggResults, int numThread);
void CPU_agg_min_afterGroupBy(Record *Rin, int rLen, int* d_startPos, int numGroups,
							  Record * RinAggOrig, int* d_aggResults, int numThread);
void CPU_agg_sum_afterGroupBy(Record *Rin, int rLen, int* d_startPos, int numGroups,
							  Record * RinAggOrig, int* d_aggResults, int numThread);
void CPU_agg_avg_afterGroupBy(Record *Rin, int rLen, int* d_startPos, int numGroups,
							  Record * RinAggOrig, int* d_aggResults, int numThread);

//data structures
void CPU_BuildHashTable(Record* h_R, int rLen, int intBits, Bound *h_bound);
void CPU_BuildTreeIndex(Record* R, int rLen, CC_CSSTree** tree);
//access methods
int CPU_HashSearch(Record* R, int rLen, Bound *h_bound, int intBits, Record *S, int sLen, Record** Rout, int numThread);
int CPU_TreeSearch(Record *R, int rLen, CC_CSSTree *tree, Record *S, int sLen, Record** Rout, int numThread);

//for joins
void CPU_Sort(Record* Rin, int rLen, Record* Rout, int numThread);
int CPU_ninlj(Record *R, int rLen, Record *S, int sLen, Record** Rout, int numThread);
int CPU_inlj(Record *R, int rLen, CC_CSSTree *tree, Record *S, int sLen, Record** Rout, int numThread);
int CPU_smj(Record *R, int rLen, Record *S, int sLen, Record** Rout, int numThread);
int CPU_MergeJoin(Record *R, int rLen, Record *S, int sLen, Record** Rout, int numThread);
int CPU_hj(Record *R, int rLen, Record *S, int sLen, Record** Rout, int numThread);

void CPU_Partition(Record *Rin, int rLen, int numPart, Record* Rout, int* startHist, int numThread);

void set_thread_affinity (int id, int numThread) ;

void set_CPU_affinity (int id, int numThread) ;

void set_selfCPUID (int id) ;

void set_numCoreForCPU (int numCore) ;



#endif

