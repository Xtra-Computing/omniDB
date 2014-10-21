#ifndef CSS_TREE_H_
#define CSS_TREE_H_
#include "PrimitiveCommon.h"
#include "common.h"
typedef int IKeyType;

#define WARPS_PER_NODE		1	//saven: 2 ray:1	// Warps per node
#define BLCK_PER_MP_create	64	// blocks per multiprocessor during tree creation
#define BLCK_PER_MP_search	64	// blocks per multiprocessor during tree searching
#define WAPRS_PER_BLCK_join	16	// blocks per multiprocessor during tree creation
#define BLCK_PER_MP_join	64//256	// blocks per multiprocessor during tree searching
//#endregion

//#region definition of card specific parameters
#define WARP_SIZE 32
#define N_MULTI_P 8
//#endregion

//#region definition of derived parameters
// Algorithmic params
#define TREE_NODE_SIZE			(WARP_SIZE * WARPS_PER_NODE)
#define TREE_FANOUT				(TREE_NODE_SIZE + 1)

// Configurational params
// create
#define THRD_PER_BLCK_create	TREE_NODE_SIZE
#define BLCK_PER_GRID_create	(N_MULTI_P * BLCK_PER_MP_create)
#define THRD_PER_GRID_create	(THRD_PER_BLCK_create * BLCK_PER_GRID_create)

// search
#define THRD_PER_BLCK_search	TREE_NODE_SIZE
#define BLCK_PER_GRID_search	(N_MULTI_P * BLCK_PER_MP_search)
#define THRD_PER_GRID_search	(THRD_PER_BLCK_search * BLCK_PER_GRID_search)

// join
#define THRD_PER_BLCK_join		(WARP_SIZE * WAPRS_PER_BLCK_join)
#define BLCK_PER_GRID_join		(N_MULTI_P * BLCK_PER_MP_join)
#define THRD_PER_GRID_join		(THRD_PER_BLCK_join * BLCK_PER_GRID_join)
//#endregion
#endif