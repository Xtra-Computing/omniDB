#ifndef DB_H
#define DB_H
#include "stdlib.h"
#include "math.h"



#define EVAL(Rhs, Tmp) { Rhs->value = Tmp->value;Rhs->rid = Tmp->rid;}
//#define SWAP(FROM, TO, TMP) { EVAL(TMP,FROM); EVAL(FROM,TO); EVAL(TO,TMP);}

int compareRecord (const void * a, const void * b);
int compareRID (const void * a, const void * b);

#define MAX_TABLE_NUM 32

#define TOTAL_BITS 32

#define NAME_MAX_LENGTH 256

#define DEFAULT_DB_SIZE (150*1024)

#define SUCCEED 0
#define TABLE_NOT_FOUND 1

#define MAX_PREDICATE_NODE 256

#define MAX_PREDICATE_STRING 1024

typedef int RET_VALUE;

//the operator type.

#define TYPE_SEL_EQUAL	1
#define TYPE_SEL_BAND	2
#define TYPE_SEL_OTHER	3

#define TYPE_JOIN_EQUAL 5
#define TYPE_JOIN_BAND	6
#define TYPE_JOIN_OTHER 7


#define NUM_ENTRY 32



#define MAX_TABLE_PER_QUERY 32




#define OFFSET ((1<<15)-1)
#define TEST_MAX ((1<<30)-1)
#define NON_LINUX_PLAT 1
#ifdef NON_LINUX_PLAT
#define RAND(MAX) ((((rand()& OFFSET)<<15)+(rand()&1))+(rand()<<1)+(rand()&1))%MAX
#else
#define RAND(MAX) (rand())%MAX
#endif

#define SWAP(Lhs, Rhs, Tmp) { \
			 Tmp = *Lhs;\
            *Lhs = *Rhs;\
            *Rhs = Tmp;\
        }

//the string functions are not supported in Linux. so we rewrite it.
/*
reverse the string.
*/
char* linux_strrev(char* szT);
/*
transform <value> into a string with radix <radix>
*/
void linux_itoa(int value, char*  str, int radix);


#define OPT_NUM	0
#define OPT_COL	1
#define OPT_OTHER 2

typedef enum{
CMP_BIGER,
CMP_SMALLER,
CMP_OTHER
}COMP_TYPE;

int getOperandType(char * str);


typedef enum{
	PROJECTION,
	SELECTION,
	TYPE_AGGREGATION,//the default.
	AGG_SUM,
	AGG_MAX,
	AGG_MIN,
	AGG_AVG,
	AGG_COUNT,
	AGG_SUM_AFTER_GROUP_BY,
	AGG_MAX_AFTER_GROUP_BY,
	AGG_AVG_AFTER_GROUP_BY,
	AGG_COUNT_AFTER_GROUP_BY,
	AGG_MIN_AFTER_GROUP_BY,
	GROUP_BY,
	ORDER_BY,
	SORT,
	TYPE_JOIN,//the default.
	JOIN_NINLJ,
	JOIN_INLJ,
	JOIN_SMJ,
	JOIN_HJ,
	DISTINCT,
	TYPE_UNKNOWN
} OP_MODE;

typedef enum{
	STATUS_DONE,
	STATUS_UNDONE
}NODE_STATUS;







//for openMP
//#define FIXED_CORE_TO_GPU 1
#ifdef FIXED_CORE_TO_GPU
#define NUM_CORE_FOR_CPU_PROCESSING 3
#define OMP_SORT_NUM_THREAD 4
#define OMP_JOIN_NUM_THREAD 4
#define OMP_NUM_PARTITION_THREAD 4
#define OMP_NUM_MERGE_THREAD 4
#else
#define NUM_CORE_FOR_CPU_PROCESSING 4
#define OMP_SORT_NUM_THREAD 8
#define OMP_JOIN_NUM_THREAD 4
#define OMP_NUM_PARTITION_THREAD 4
#define OMP_NUM_MERGE_THREAD 4
#endif

#endif

