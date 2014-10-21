#ifndef CPU_HASH_TABLE
#define CPU_HASH_TABLE

#include "LinkedList.h"

struct Bound{
	int start;
	int end;
};

void buildHashTable(Record* h_R, int OP_rLen, int intBits, Bound *h_bound);
int partHashSearch(Record* h_R, int OP_rLen, Bound *h_bound, int intBits, Record *S, int startS, int endS, LinkedList *ll);
int HashSearch_omp(Record* R, int OP_rLen, Bound *h_bound, int intBits, Record *S, int sLen, Record** Rout, int numThread);

#endif

