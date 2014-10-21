#ifndef PRIMITIVE_COMMON_H
#define PRIMITIVE_COMMON_H
#include "common.h"
//scan.
typedef cl_uint uint;
const uint WORKGROUP_SIZE=256;
const uint MAX_BATCH_ELEMENTS = 64 * 1048576;
const uint MIN_LARGE_ARRAY_SIZE = 8 * WORKGROUP_SIZE;
const uint MAX_LARGE_ARRAY_SIZE = 4 * WORKGROUP_SIZE * WORKGROUP_SIZE;
uint iSnapUp(uint dividend, uint divisor);
uint factorRadix2(uint& log2L, uint L);
bool is2n(unsigned int i);
int get2N( int rLen );

unsigned int uintCeilingDiv(unsigned int dividend, unsigned int divisor);


unsigned int uintFloorLog(unsigned int base, unsigned int num);

unsigned int uintCeilingLog(unsigned int base, unsigned int num);

unsigned int uintPower(unsigned int base, unsigned int pow);

//new added
void validateGroupBy( Record* h_Rin, int rLen, Record* h_Rout, int* h_startPos, int numGroup );
void validateAggAfterGroupBy( Record *Rin, int rLen, int* startPos, int numGroups, Record* Ragg, int* aggResults, int OPERATOR );
void validateFilter( Record* h_Rin, int beginPos, int rLen, 
					Record* h_Rout, int outSize, int smallKey, int largeKey);
void validateReduce( Record* R, int rLen, unsigned int gpuResult, int OPERATOR );
void validateScan( int* input, int rLen, int* output );
void validateProjection( Record* h_Rin, int rLen, Record* originalProjTable, Record* h_projTable, int pLen );

void validateSplit(Record *R, int rLen, int numPart);
void validateSort(Record *R, int rLen);



#define divRoundDown(n,s)  ((n) / (s))
#define divRoundUp(n,s)    (((n) / (s)) + ((((n) % (s)) > 0) ? 1 : 0))

#endif

