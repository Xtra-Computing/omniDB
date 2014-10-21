#pragma once

//
//#include <QP_Utility.cu>

typedef int IKeyType;

#ifdef __cplusplus
extern "C" {
#endif

	//#region Tunable parameters
	#define WARPS_PER_NODE		1	//saven: 2 ray:1	// Warps per node
	#define BLCK_PER_MP_create	256	// blocks per multiprocessor during tree creation
	#define BLCK_PER_MP_search	512	// blocks per multiprocessor during tree searching
	#define WAPRS_PER_BLCK_join	8//16	// blocks per multiprocessor during tree creation
	#define BLCK_PER_MP_join	512//256	// blocks per multiprocessor during tree searching
	//#endregion

	//#region definition of card specific parameters
	#define WARP_SIZE 32
	#define N_MULTI_P 16
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

	//#region Indexing Tree Types
	typedef struct {
		IKeyType keys[TREE_NODE_SIZE];
	} IDirectoryNode;	
	typedef struct {
		Record records[TREE_NODE_SIZE];
	} IDataNode;
	//#endregion

	//#region Declaration of functions
	void cuda_create_index(IDataNode g_data[], unsigned int nDataNodes, IDirectoryNode** g_ptrDir, unsigned int* ptrDirSize, unsigned int nNodesPerBlock);
	int cuda_join(Record R[], unsigned int Query_rLen, unsigned int nDataNodes, IDirectoryNode g_dir[], unsigned int nDirNodes, Record S[], unsigned int sLen);
	void cuda_search_index(IDataNode g_data[], unsigned int nDataNodes, IDirectoryNode g_dir[], unsigned int nDirNodes, Record g_keys[], int g_locations[], unsigned int nSearchKeys);
	void cuda_search_index_usingKeys(IDataNode g_data[], unsigned int nDataNodes, IDirectoryNode g_dir[], unsigned int nDirNodes, int g_keys[], int g_locations[], unsigned int nSearchKeys);	
	int cuda_join_after_search(IDataNode g_data[], unsigned int nDataNodes, IDirectoryNode g_dir[], unsigned int nDirNodes, Record g_keys[], int g_locations[], unsigned int nSearchKeys);
	//#endregion

#ifdef __cplusplus
}
#endif
