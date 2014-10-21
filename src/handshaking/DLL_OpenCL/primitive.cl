#define NV 1
#define COALESCED
//#define MAC 1
typedef uint2 Record;
#define SHARED_MEMORY_PER_PROCESSOR (8*1024)

#define NLJ_NUM_PROCESSOR 16//for GTX
#define NLJ_SHARED_MEM_PER_PROCESSOR (SHARED_MEMORY_PER_PROCESSOR)
#define NLJ_SHARED_MEM (NLJ_SHARED_MEM_PER_PROCESSOR*NLJ_NUM_PROCESSOR)
#define NLJ_MAX_NUM_BLOCK_PER_DIM (1024)
#define NLJ_NUM_THREADS_PER_BLOCK 256
#define NLJ_NUM_TUPLE_PER_THREAD 2
#define NLJ_S_BLOCK_SIZE (NLJ_NUM_THREADS_PER_BLOCK*NLJ_NUM_TUPLE_PER_THREAD)
#define NLJ_R_BLOCK_SIZE NLJ_NUM_THREADS_PER_BLOCK
#define PRED_EQUAL2(DATA) (DATA[0]==DATA[1])
#define PRED_EQUAL(V1,V2) (V1==V2)

//for CSS tree
typedef int IKeyType;

#define WARPS_PER_NODE		1	//saven: 2 ray:1	// Warps per node
#define BLCK_PER_MP_create	64	// blocks per multiprocessor during tree creation
#define BLCK_PER_MP_search	64	// blocks per multiprocessor during tree searching
#define WAPRS_PER_BLCK_join	8	// blocks per multiprocessor during tree creation
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

#define NUM_RECORDS_R (512*512*4*16)

#define NUM_THREADS_SORT (256)
#define NUM_BLOCKS_X_SORT (NUM_RECORDS_R/NUM_THREADS_SORT)
#define NUM_BLOCKS_Y_SORT (1)

#define REDUCE_SUM (0)
#define REDUCE_MAX (1)
#define REDUCE_MIN (2)
#define REDUCE_AVERAGE (3)

#define SPLIT (4)
#define PARTITION (5)
#define NUM_BANKS 16
#define LOG_NUM_BANKS 4
typedef struct {
	IKeyType keys[TREE_NODE_SIZE];
} IDirectoryNode;	
typedef struct {
	Record records[TREE_NODE_SIZE];
} IDataNode;

//for smj
#define NUM_DELTA_PER_BLOCK 8 
#define SMJ_NUM_THREADS_PER_BLOCK 256

#define TEST_MAX (1<<30)
#define TEST_MIN (0)
int ceilf(float a)
{
	int v=(int)a;
	if(a-(float)v>0)
		v++;
	return v;
}
inline
unsigned int RSHash(int value, int mask)
{
    unsigned int b=378551;
    unsigned int a=63689;
    unsigned int hash=0;

    int i=0;

	for(i=0;i<4;i++)
    {

        hash=hash*a+(value>>(24-(i<<3)));
        a*=b;
    }

    return (hash & mask);
}
	//with shared memory, with coalesced
inline
int CONFLICT_FREE_OFFSET_REDUCE( int index )
{
		//return ((index) >> LOG_NUM_BANKS + (index) >> (2*LOG_NUM_BANKS));

		return ((index) >> LOG_NUM_BANKS);
}	

//hash join
#define _maxPartLen (512)	//max partition length. Limited by shared memory size (4k to be safe): sizeof(Rec) * maxPartLen <= 4k
#define HASH(v) RSHash(v, ((1<<30)-1)) //(((unsigned int)( (v >> 7) ^ (v >> 13) ^ (v >>21) ^ (v) )) )

//new added part.
__kernel//kid=0
void projection_map_kernel(__global Record* d_projTable, int pLen,__global int* d_loc,__global int* d_temp )
{
	int bx = get_group_id(0);
	int tx = get_local_id(0);
	int numThread = get_local_size(0);
	const int gridDimX=get_num_groups(0);
	int gridSize = numThread*gridDimX;

	for( int idx = bx*numThread + tx; idx < pLen; idx += gridSize )
	{
		d_loc[idx] = d_projTable[idx].x;
		d_temp[idx] = d_projTable[idx].y;
	}
}
//the best, with shared memory, with coalesced access
__kernel//kid=1
void getResult_kernel(__global int* d_Result, __global Record* d_Rout, int rLen, int OPERATOR )
{
	d_Rout[0].x = 0;
	d_Rout[0].y = d_Result[0];

	if( OPERATOR == REDUCE_AVERAGE )
	{
		d_Rout[0].y = d_Result[0]/rLen;
	}
}

__kernel void //kid=2
groupByImpl_outSize_kernel(__global  int* d_outSize,__global  int* d_mark,__global  int* d_markOutput, int rLen )
{
	*d_outSize = d_mark[rLen-1] + d_markOutput[rLen-1];
}
__kernel//kid=3
void mapBeforeGather_kernel(__global  Record* d_Rin, int rLen,__global  int* d_loc,__global  int* d_temp )
{
	int bx = get_group_id(0);
	int tx = get_local_id(0);
	int numThread = get_local_size(0);
	const int gridDimX=get_num_groups(0);
	int gridSize = numThread*gridDimX;

	for( int idx = bx*numThread + tx; idx < rLen; idx += gridSize )
	{
		d_loc[idx] = d_Rin[idx].x;
		d_temp[idx] = d_Rin[idx].y;
	}
}

__kernel//kid=4
void parallelAggregate_kernel(__global  Record* d_S,__global  int* d_startPos,__global  int* d_aggResults, int OPERATOR, int blockOffset, int numGroups, int rLen ,__local int* s_data  )
{
	const int blockDimX=get_local_size(0);
	int bx = get_group_id(0);
	int tx = get_local_id(0);
	int numThread =blockDimX;
	int idx = blockOffset + get_group_id(0);
	int start = d_startPos[idx];
	int end = (idx == (numGroups - 1))?(rLen):(d_startPos[idx + 1]);
//	int totalTx = bx*numThread + tx;
	int currentVal;

	if( OPERATOR == REDUCE_MAX )
	{
		//initialization the shared memory
		s_data[tx] = 0;

		for( int i = start + tx; i < end; i = i + numThread )
		{
			currentVal = d_S[i].y;
			if( currentVal > s_data[tx] )
			{
				s_data[tx] = currentVal;
			}
		}

		 barrier(CLK_LOCAL_MEM_FENCE);

		int delta = 2;
		while( delta <= numThread )
		{
			int offset = delta*tx;
			if( offset < numThread )
			{
				s_data[offset] = (s_data[offset] > s_data[offset + delta/2])?(s_data[offset]):(s_data[offset + delta/2]);
			}

			delta = delta*2;
			 barrier(CLK_LOCAL_MEM_FENCE);
		}
	}
	else if( OPERATOR == REDUCE_MIN )
	{
		//initialization the shared memory
		s_data[tx] = TEST_MAX;

		for( int i = start + tx; i < end; i = i + numThread )
		{
			currentVal = d_S[i].y;
			if( currentVal < s_data[tx] )
			{
				s_data[tx] = currentVal;
			}
		}

		 barrier(CLK_LOCAL_MEM_FENCE);

		int delta = 2;
		while( delta <= numThread )
		{
			int offset = delta*tx;
			if( offset < numThread )
			{
				s_data[offset] = (s_data[offset] < s_data[offset + delta/2])?(s_data[offset]):(s_data[offset + delta/2]);
			}

			delta = delta*2;
			 barrier(CLK_LOCAL_MEM_FENCE);
		}
	}
	else if( (OPERATOR == REDUCE_SUM) || (OPERATOR == REDUCE_AVERAGE) )
	{
		//initialization the shared memory
		s_data[tx] = 0;

		for( int i = start + tx; i < end; i = i + numThread )
		{
			s_data[tx] += d_S[i].y;
		}

		 barrier(CLK_LOCAL_MEM_FENCE);

		int delta = 2;
		while( delta <= numThread )
		{
			int offset = delta*tx;
			if( offset < numThread )
			{
				s_data[offset] = (s_data[offset]) + (s_data[offset + delta/2]);
			}

			delta = delta*2;
			 barrier(CLK_LOCAL_MEM_FENCE);
		}
	}


	 barrier(CLK_LOCAL_MEM_FENCE);

	if( tx == 0 )
	{
		if( OPERATOR ==	REDUCE_AVERAGE )
		{
			d_aggResults[idx] = s_data[0]/(end - start);
		}
		else
		{
			d_aggResults[idx] = s_data[0];
		}
	}

}
__kernel//kid=5
void copyLastElement_kernel(__global  int* d_odata,__global Record* d_Rin, int base, int offset)
{
	d_odata[offset] = d_Rin[base].y;
}
//with shared memory, with coalesced
__kernel//kid=6
void perscanFirstPass_kernel(__global int* temp, __global int* d_temp,__global int* d_odata,__global Record* d_idata, int numElementsPerBlock,  int isFull, int base, int d_odataOffset, int OPERATOR,int sharedMemSize )
{
{		
		int thid = get_local_id(0);
		int offset = 1;
		int numThread = get_local_size(0);
		int baseIdx = get_group_id(0)*(numThread*2) + base; 

		int mem_ai = baseIdx + thid;
		int mem_bi = mem_ai + numThread;

		int ai = thid;
		int bi = thid + numThread;

		int bankOffsetA = CONFLICT_FREE_OFFSET_REDUCE( ai );
		int bankOffsetB = CONFLICT_FREE_OFFSET_REDUCE( bi );

		d_temp[mem_ai] = d_idata[mem_ai].x;
		temp[ai + bankOffsetA] = d_idata[mem_ai].y;
		
		if( OPERATOR == REDUCE_SUM || OPERATOR == REDUCE_AVERAGE )
		{
			if( isFull )
			{
				d_temp[mem_bi] = d_idata[mem_bi].x;
				temp[bi + bankOffsetB] = d_idata[mem_bi].y;
			}
			else
			{
				temp[bi + bankOffsetB] = (bi < numElementsPerBlock) ? (d_idata[mem_bi].y) : (0);
			}
		}
		else if( OPERATOR == REDUCE_MAX )
		{
			if( isFull )
			{
				d_temp[mem_bi] = d_idata[mem_bi].x;
				temp[bi + bankOffsetB] = d_idata[mem_bi].y;
			}
			else
			{
				temp[bi + bankOffsetB] = (bi < numElementsPerBlock) ? (d_idata[mem_bi].y) : ( TEST_MIN );
			}
		}
		else if( OPERATOR == REDUCE_MIN )
		{
			if( isFull )
			{
				d_temp[mem_bi] = d_idata[mem_bi].x;
				temp[bi + bankOffsetB] = d_idata[mem_bi].y;
			}
			else
			{
				temp[bi + bankOffsetB] = (bi < numElementsPerBlock) ? (d_idata[mem_bi].y) : ( TEST_MAX );
			}
		}


		barrier(CLK_LOCAL_MEM_FENCE);

		//build sum in place up the tree
		if( OPERATOR == REDUCE_SUM || OPERATOR == REDUCE_AVERAGE )
		{
			for( int d = (numThread*2)>>1; d > 0; d >>= 1 )
			{
				barrier(CLK_LOCAL_MEM_FENCE);

				if( thid < d )
				{
					int ai = offset*( 2*thid + 1 ) - 1;
					int bi = offset*( 2*thid + 2 ) - 1;
					ai += CONFLICT_FREE_OFFSET_REDUCE( ai );
					bi += CONFLICT_FREE_OFFSET_REDUCE( bi );

					temp[bi] += temp[ai];
				}

				offset *= 2;
			}
		}
		else if( OPERATOR == REDUCE_MAX )
		{
			for( int d = (numThread*2)>>1; d > 0; d >>= 1 )
			{
				barrier(CLK_LOCAL_MEM_FENCE);

				if( thid < d )
				{
					int ai = offset*( 2*thid + 1 ) - 1;
					int bi = offset*( 2*thid + 2 ) - 1;
					ai += CONFLICT_FREE_OFFSET_REDUCE( ai );
					bi += CONFLICT_FREE_OFFSET_REDUCE( bi );

					temp[bi] = (temp[bi] > temp[ai])?(temp[bi]):(temp[ai]);				
				}

				offset *= 2;
			}
		}
		else if( OPERATOR == REDUCE_MIN )
		{
			for( int d = (numThread*2)>>1; d > 0; d >>= 1 )
			{
				barrier(CLK_LOCAL_MEM_FENCE);

				if( thid < d )
				{
					int ai = offset*( 2*thid + 1 ) - 1;
					int bi = offset*( 2*thid + 2 ) - 1;
					ai += CONFLICT_FREE_OFFSET_REDUCE( ai );
					bi += CONFLICT_FREE_OFFSET_REDUCE( bi );

					temp[bi] = (temp[bi] > temp[ai])?(temp[ai]):(temp[bi]);				
				}

				offset *= 2;
			}
		}


		barrier(CLK_LOCAL_MEM_FENCE);

		//write out the reduced block sums to d_odata
		if( thid == (numThread - 1)  )
		{
			d_odata[get_group_id(0) + d_odataOffset] = temp[bi+bankOffsetB];
		}
	}
}
__kernel//kid=7
void perscan_kernel(__global int* d_odata,__global int* temp,__global int* d_idata, int numElementsPerBlock, int isFull, int base, int d_odataOffset, int OPERATOR, unsigned int sharedMemSize )
{
		
		int thid = get_local_id(0);
		int offset = 1;
		int numThread = get_local_size(0);
		int baseIdx = get_group_id(0)*(numThread*2) + base; 

		int mem_ai = baseIdx + thid;
		int mem_bi = mem_ai + numThread;

		int ai = thid;
		int bi = thid + numThread;

		int bankOffsetA = CONFLICT_FREE_OFFSET_REDUCE( ai );
		int bankOffsetB = CONFLICT_FREE_OFFSET_REDUCE( bi );

		temp[ai + bankOffsetA] = d_idata[mem_ai];

		if( OPERATOR == REDUCE_SUM || OPERATOR == REDUCE_AVERAGE )
		{
			if( isFull )
			{
				temp[bi + bankOffsetB] = d_idata[mem_bi];
			}
			else
			{
				temp[bi + bankOffsetB] = (bi < numElementsPerBlock) ? (d_idata[mem_bi]) : (0);
			}
		}
		else if( OPERATOR == REDUCE_MAX )
		{
			if( isFull )
			{
				temp[bi + bankOffsetB] = d_idata[mem_bi];
			}
			else
			{
				temp[bi + bankOffsetB] = (bi < numElementsPerBlock) ? (d_idata[mem_bi]) : (TEST_MIN);
			}
		}
		else if( OPERATOR == REDUCE_MIN )
		{
			if( isFull )
			{
				temp[bi + bankOffsetB] = d_idata[mem_bi];
			}
			else
			{
				temp[bi + bankOffsetB] = (bi < numElementsPerBlock) ? (d_idata[mem_bi]) : (TEST_MAX);
			}
		}


		 barrier(CLK_LOCAL_MEM_FENCE);

		//build sum in place up the tree
		if( OPERATOR == REDUCE_SUM || OPERATOR == REDUCE_AVERAGE )
		{
			for( int d = (numThread*2)>>1; d > 0; d >>= 1 )
			{
				 barrier(CLK_LOCAL_MEM_FENCE);

				if( thid < d )
				{
					int ai = offset*( 2*thid + 1 ) - 1;
					int bi = offset*( 2*thid + 2 ) - 1;
					ai += CONFLICT_FREE_OFFSET_REDUCE( ai );
					bi += CONFLICT_FREE_OFFSET_REDUCE( bi );

					temp[bi] += temp[ai];
				}

				offset *= 2;
			}
		}
		else if( OPERATOR == REDUCE_MAX )
		{
			for( int d = (numThread*2)>>1; d > 0; d >>= 1 )
			{
				 barrier(CLK_LOCAL_MEM_FENCE);

				if( thid < d )
				{
					int ai = offset*( 2*thid + 1 ) - 1;
					int bi = offset*( 2*thid + 2 ) - 1;
					ai += CONFLICT_FREE_OFFSET_REDUCE( ai );
					bi += CONFLICT_FREE_OFFSET_REDUCE( bi );

					temp[bi] = (temp[bi] > temp[ai])?(temp[bi]):(temp[ai]);
				}

				offset *= 2;
			}
		}
		else if( OPERATOR == REDUCE_MIN )
		{
			for( int d = (numThread*2)>>1; d > 0; d >>= 1 )
			{
				 barrier(CLK_LOCAL_MEM_FENCE);

				if( thid < d )
				{
					int ai = offset*( 2*thid + 1 ) - 1;
					int bi = offset*( 2*thid + 2 ) - 1;
					ai += CONFLICT_FREE_OFFSET_REDUCE( ai );
					bi += CONFLICT_FREE_OFFSET_REDUCE( bi );

					temp[bi] = (temp[bi] > temp[ai])?(temp[ai]):(temp[bi]);
				}

				offset *= 2;
			}
		}

		 barrier(CLK_LOCAL_MEM_FENCE);

		//write out the reduced block sums to d_odata
		if( thid == (numThread - 1)  )
		{
			d_odata[get_group_id(0) + d_odataOffset] = temp[bi+bankOffsetB];
		}	
}
__kernel  //kid=8
void groupByImpl_write_kernel(__global int* d_startPos,__global int* d_groupLabel,__global int* d_writePos, int rLen )
	{
		int bx = get_group_id(0);
		int tx = get_local_id(0);
		const int blockDimX=get_local_size(0);
		const int gridDimX=get_num_groups(0);
		int gridSize = blockDimX*gridDimX;

		for( int idx = bx*blockDimX + tx; idx < rLen; idx += gridSize )
		{
			if( d_groupLabel[idx] == 1 )
			{
				d_startPos[d_writePos[idx]] = idx;
			}
		}	
	}
__kernel //kid=9
void scanGroupLabel_kernel(__global Record* d_Rin, int rLen,__global int* d_groupLabel )
	{
		int bx = get_group_id(0);
		int tx = get_local_id(0);
		const int blockDimX=get_local_size(0);
		const int gridDimX=get_num_groups(0);
		int gridSize = blockDimX*gridDimX;
		int currentValue;
		int nextValue;

		for( int idx = bx*blockDimX + tx; idx < rLen - 1; idx += gridSize )
		{
			currentValue = d_Rin[idx].y;
			nextValue = d_Rin[idx + 1].y;

			if( currentValue != nextValue )
			{
				d_groupLabel[idx + 1] = 1;
			}
		}

		//write the first position
		if( (bx == 0) && (tx == 0) )
		{
			d_groupLabel[0] = 1;
		}
	}

__kernel void //kid=10
setRIDList_kernel(__global int *d_RIDList, int delta, __global int *d_intput, int rLen,  __global Record *d_R)  
{
	const int by = get_group_id(1);
	const int bx = get_group_id(0);
	const int tx = get_local_id(0);
	const int ty = get_local_id(1);	
	const int tid=tx+ty*get_local_size(0);
	const int bid=bx+by*get_num_groups(0);
	const int numThread=get_local_size(0);
	const int resultID=(bid)*numThread+tid;
	for(int pos=resultID;pos<rLen;pos+=delta)
	{
		//value=d_R[pos];
		d_R[pos].x=d_RIDList[pos];
		d_R[pos].y=d_intput[pos];
	}		
}

__kernel void //kid=11
getRIDList_kernel(__global int *d_RIDList,int delta,__global int *d_output, int rLen,__global Record *d_Rin) 
{
	const int by = get_group_id(1);
	const int bx = get_group_id(0);
	const int tx = get_local_id(0);
	const int ty = get_local_id(1);	
	const int tid=tx+ty*get_local_size(0);
	const int bid=bx+by*get_num_groups(0);
	const int numThread=get_local_size(0);
	const int resultID=(bid)*numThread+tid;
	get_local_size(0);
	for(int pos=resultID;pos<rLen;pos+=delta)
	{
		//value=d_R[pos];
		d_RIDList[pos]=d_Rin[pos].x;
		d_output[pos]=d_Rin[pos].y;
	}	
}
__kernel void //kid=12
setValueList_kernel(__global int *d_ValueList, int delta, int rLen,__global Record *d_R)  
{
	const int by = get_group_id(1);
	const int bx = get_group_id(0);
	const int tx = get_local_id(0);
	const int ty = get_local_id(1);	
	const int tid=tx+ty*get_local_size(0);
	const int bid=bx+by*get_num_groups(0);
	const int numThread=get_local_size(0);
	const int resultID=(bid)*numThread+tid;
	//Record value;
	for(int pos=resultID;pos<rLen;pos+=delta)
	{
		//value=d_R[pos];
		d_R[pos].x=pos;
		d_R[pos].y=d_ValueList[pos];
	}	
}
__kernel void //kid=13
getValueList_kernel(__global int *d_ValueList,int delta,__global int *d_output, int rLen,__global Record *d_Rin) 
{
	const int by = get_group_id(1);
	const int bx = get_group_id(0);
	const int tx = get_local_id(0);
	const int ty = get_local_id(1);	
	const int tid=tx+ty*get_local_size(0);
	const int bid=bx+by*get_num_groups(0);
	const int numThread=get_local_size(0);
	const int resultID=(bid)*numThread+tid;
	get_local_size(0);
	for(int pos=resultID;pos<rLen;pos+=delta)
	{
		//value=d_R[pos];
		d_ValueList[pos]=d_Rin[pos].x;
		d_output[pos]=d_Rin[pos].y;
	}	
}

//origianl part
__kernel void //kid=14
mapImpl_kernel(__global Record *d_R, int rLen,__global int *d_output1, __global int *d_output2)  
{
	int iGID = get_global_id(0);
	Record value;
	int delta=get_global_size(0);
	for(int pos=iGID;pos<rLen;pos+=delta)
	{
		value=d_R[pos];
		d_output1[pos]=value.x;
		d_output2[pos]=value.y;
	}	
}
__kernel void //kid=15
mapImplVec_kernel(__global uint4 *d_R, int rLen,__global uint2 *d_output1, __global uint2 *d_output2)  
{
	int iGID = get_global_id(0);
	uint4 value;
	int delta=get_global_size(0);
	uint2 first;
	uint2 second; 
	int len=rLen>>1;
	for(int pos=iGID;pos<len;pos+=delta)
	{
		value=d_R[pos];		
		first.x=value.x; first.y= value.z;
		second.x=value.y; second.y= value.w;
		d_output1[pos]=first;
		d_output2[pos]=second;
	}	
}
__kernel void //kid=16
memset_int_kernel(__global int *d_R, int rLen, int value)  
{
	int iGID = get_global_id(0);	
	int delta=get_global_size(0);
	for(int pos=iGID;pos<rLen;pos+=delta)
	{
		d_R[pos]=value;
	}
}


/*
 * ScanLargeArrays_kernel : Scan is done for each block and the sum of each
 * block is stored in separate array (sumBuffer). SumBuffer is scanned
 * and results are added to every value of next corresponding block to
 * compute the scan of a large array.(not limited to 2*MAX_GROUP_SIZE)
 * Scan uses a balanced tree algorithm. See Belloch, 1990 "Prefix Sums
 * and Their Applications"
 * @param output output data 
 * @param input  input data
 * @param block  local memory used in the kernel
 * @param sumBuffer  sum of blocks
 * @param length length of the input data
 */

__kernel//kid=17
void blockAddition_kernel(__global int* input, __global int* output)
{	
	int globalId = get_global_id(0);
	int groupId = get_group_id(0);
	int localId = get_local_id(0);

	__local int value[1];

	/* Only 1 thread of a group will read from global buffer */
	if(localId == 0)
	{
		value[0] = input[groupId];
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	output[globalId] += value[0];
}


__kernel //kid=18
void prefixSum_kernel(__global int *output, __global int *input, __local  int *block, const uint length)
{
	int tid = get_local_id(0);
	
	int offset = 1;

    /* Cache the computational window in shared memory */
	block[2*tid]     = input[2*tid];
	block[2*tid + 1] = input[2*tid + 1];	

    /* build the sum in place up the tree */
	for(int d = length>>1; d > 0; d >>=1)
	{
		barrier(CLK_LOCAL_MEM_FENCE);
		
		if(tid<d)
		{
			int ai = offset*(2*tid + 1) - 1;
			int bi = offset*(2*tid + 2) - 1;
			
			block[bi] += block[ai];
		}
		offset *= 2;
	}

    /* scan back down the tree */

    /* clear the last element */
	if(tid == 0)
	{
		block[length - 1] = 0;
	}

    /* traverse down the tree building the scan in the place */
	for(int d = 1; d < length ; d *= 2)
	{
		offset >>=1;
		barrier(CLK_LOCAL_MEM_FENCE);
		
		if(tid < d)
		{
			int ai = offset*(2*tid + 1) - 1;
			int bi = offset*(2*tid + 2) - 1;
			
			int t = block[ai];
			block[ai] = block[bi];
			block[bi] += t;
		}
	}
	
	barrier(CLK_LOCAL_MEM_FENCE);

    /*write the results back to global memory */
	output[2*tid]     = block[2*tid];
	output[2*tid + 1] = block[2*tid + 1];
}

__kernel //kid=19
void ScanLargeArrays_kernel(__global int *output,
               		__global int *input,
              		 __local  int *block,	 // Size : block_size
					const uint block_size,	 // size of block				
              		const uint length,	 	 // no of elements 
					 __global int *sumBuffer)  // sum of blocks
			
{
	int tid = get_local_id(0);
	int gid = get_global_id(0);
	int bid = get_group_id(0);
	
	int offset = 1;

    /* Cache the computational window in shared memory */
	block[2*tid]     = input[2*gid];
	block[2*tid + 1] = input[2*gid + 1];	

    /* build the sum in place up the tree */
	for(int d = block_size>>1; d > 0; d >>=1)
	{
		barrier(CLK_LOCAL_MEM_FENCE);
		
		if(tid<d)
		{
			int ai = offset*(2*tid + 1) - 1;
			int bi = offset*(2*tid + 2) - 1;
			
			block[bi] += block[ai];
		}
		offset *= 2;
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	int group_id = get_group_id(0);
		
    /* store the value in sum buffer before making it to 0 */ 	
	sumBuffer[bid] = block[block_size - 1];

	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	
    /* scan back down the tree */

    /* clear the last element */
	block[block_size - 1] = 0;	

    /* traverse down the tree building the scan in the place */
	for(int d = 1; d < block_size ; d *= 2)
	{
		offset >>=1;
		barrier(CLK_LOCAL_MEM_FENCE);
		
		if(tid < d)
		{
			int ai = offset*(2*tid + 1) - 1;
			int bi = offset*(2*tid + 2) - 1;
			
			int t = block[ai];
			block[ai] = block[bi];
			block[bi] += t;
		}
	}
	
	barrier(CLK_LOCAL_MEM_FENCE);	

    /*write the results back to global memory */

	if(group_id == 0)
	{	
		output[2*gid]     = block[2*tid];
		output[2*gid + 1] = block[2*tid + 1];
	}
	else
	{
		output[2*gid]     = block[2*tid];
		output[2*gid + 1] = block[2*tid + 1];
	}
	

}
__kernel void//kid=20
filterImpl_map_kernel(__global Record* d_Rin, int beginPos, int rLen, __global int* d_mark, 
								  int smallKey, int largeKey, __global int* d_temp )
{
	int iGID = get_global_id(0);
	Record value;
	int delta=get_global_size(0);
	int flag=0;
	for(int pos=iGID;pos<rLen;pos+=delta)
	{
		value = d_Rin[pos];
		d_temp[pos] = value.x;
		int key = value.y;
		//the filter condition		
		if( ( key >= smallKey ) && ( key <= largeKey ) )
		{
			flag = 1;
		}
		else
		{
			flag=0;
		}
		d_mark[pos]=flag;
	}	
}

/*__kernel void //kid=21
filterImpl_map_noCoalesced_kernel(__global Record* d_Rin, int beginPos, int rLen, __global int* d_mark, 
								  int smallKey, int largeKey, __global int* d_temp )
{
	int delta=get_global_size(0);
	unsigned int chunkSize=rLen/delta;
	const int resultID=get_global_id(0);
	Record value;
	int startPos=resultID*chunkSize;
	int endPos=(resultID+1)*chunkSize;
	endPos=(endPos>rLen)?rLen:endPos;
	int flag=0;
	for(int pos=startPos;pos<endPos;pos++)
	{
		value = d_Rin[pos];		
		int key = value.y;
		//the filter condition		
		if( ( key >= smallKey ) && ( key <= largeKey ) )
		{
			flag = 1;
		}
		else
		{
			flag=0;
		}
		d_mark[pos]=flag;
	}
}*/


__kernel void //kid 21
filterImpl_outSize_kernel(__global int* d_outSize,__global int* d_mark,__global int* d_markOutput, int rLen )
{
	*d_outSize = d_mark[rLen-1] + d_markOutput[rLen-1];
}


__kernel void//kid=22
filterImpl_write_noCoalesced_kernel(__global Record* d_Rout,__global Record* d_Rin, __global int* d_mark, __global int* d_markOutput, int beginPos, int rLen )
{
	int delta=get_global_size(0);
	unsigned int chunkSize=rLen/delta;
	const int resultID=get_global_id(0);
	Record value;
	int startPos=resultID*chunkSize;
	int endPos=(resultID+1)*chunkSize;
	endPos=(endPos>rLen)?rLen:endPos;
	int flag=0;
	int writePos=0;
	for(int pos=startPos;pos<endPos;pos++)
	{
		flag=d_mark[pos];
		if(flag)
		{
			writePos=d_markOutput[pos];
			d_Rout[writePos]=d_Rin[pos];
		}
	}
}

__kernel void//kid=23
filterImpl_write_kernel(__global Record* d_Rout,__global Record* d_Rin, __global int* d_mark, __global int* d_markOutput, int beginPos, int rLen )
{
	int iGID = get_global_id(0);
	Record value;
	int delta=get_global_size(0);
	int flag=0;
	int writePos=0;
	for(int pos=iGID;pos<rLen;pos+=delta)
	{
		flag=d_mark[pos];
		writePos=d_markOutput[pos];
		if(flag)
		{
			d_Rout[writePos]=d_Rin[pos];
		}
	}	
}



//scatter and gather

__kernel void//kid=24
optScatter_kernel(__global Record *d_R, int rLen,__global int *loc, int from, int to, __global Record *d_S)
{
	int iGID = get_global_id(0);	
	int delta=get_global_size(0);
	int targetLoc=0;
	for(int pos=iGID;pos<rLen;pos+=delta)
	{
		targetLoc=loc[pos];
		if(targetLoc>=from && targetLoc<to)
		d_S[targetLoc]=d_R[pos];
	}	
}
__kernel void//kid=25
optGather_kernel( __global Record *d_R, int rLen, __global int *loc, int from, int to, 
		  __global Record *d_S, int sLen)
{
	int iGID = get_global_id(0);	
	int delta=get_global_size(0);
	int targetLoc=0;
	for(int pos=iGID;pos<sLen;pos+=delta)
	{
		targetLoc=loc[pos];
		if(targetLoc>=from && targetLoc<to)
		d_S[pos]=d_R[targetLoc];
	}
}

inline int getPartID(int key, int numPart)
{
	return (key%numPart);
}

//for split
__kernel //kid=26
void partition_kernel(__global Record *d_R, int rLen, int numPart, __global int *d_output1, __global int *d_output2)  
{
	int iGID = get_global_id(0);	
	int delta=get_global_size(0);
	int targetLoc=0;
	for(int pos=iGID;pos<rLen;pos+=delta)
	{
		d_output1[pos] = d_R[pos].x;
		d_output2[pos] = RSHash(d_R[pos].y, numPart - 1);
	}	
}

__kernel void //kid=27
mapPart_kernel(__global Record *d_R, int rLen, int numPart, __global int *d_output1, __global int *d_output2)  
{
	int iGID = get_global_id(0);	
	int delta=get_global_size(0);
	int targetLoc=0;
	for(int pos=iGID;pos<rLen;pos+=delta)
	{
		d_output1[pos]=d_R[pos].x;
		d_output2[pos]=getPartID(d_R[pos].y, numPart);
	}	
}
//use the shared memory.
	//compute the histogram.
	//d_hist layout: d_hist[thread global ID + total number threads*partition ID] is the count.
__kernel //kid=28
void countHist_kernel(__global int *d_pidArray, int rLen, int numPart, 
	__global int *d_hist, __local int* shared_hist)
	{
		//extern __shared__ int shared_hist[];
		const int tid=get_local_id(0);
		const int bid=get_group_id(0);		
		const int resultID=get_global_id(0);
		int delta=get_global_size(0);

		int pid=0;
		int offset=tid*numPart;
		
		for(pid=0;pid<numPart;pid++)
		{
			shared_hist[pid+offset]=0;
		}
		barrier(CLK_LOCAL_MEM_FENCE);	

		for(int pos=resultID;pos<rLen;pos+=delta)
		{
			pid=d_pidArray[pos];
			shared_hist[pid+offset]++;
		}
		barrier(CLK_LOCAL_MEM_FENCE);

		for(pid=0;pid<numPart;pid++)
		{
			d_hist[resultID+delta*pid]=shared_hist[pid+offset];
		}
		
	}
	//having the prefix sum, compute the write location.
__kernel //kid=29
void writeHist_kernel(__global int *d_pidArray, int rLen, int numPart, __global int *d_psSum, 
	__global int* d_loc,  __local int* shared_hist)
	{
		//extern __shared__ int shared_hist[];
		const int tid=get_local_id(0);
		const int bid=get_group_id(0);		
		const int resultID=get_global_id(0);
		int delta=get_global_size(0);

		int pid=0;
		int offset=tid*numPart;
		for(pid=0;pid<numPart;pid++)
			shared_hist[pid+offset]=d_psSum[resultID+delta*pid];
		barrier(CLK_LOCAL_MEM_FENCE);
		for(int pos=resultID;pos<rLen;pos+=delta)
		{
			pid=d_pidArray[pos];
			d_loc[pos]=shared_hist[pid+offset];
			shared_hist[pid+offset]++;
		}
	}
__kernel void //kid=30
getBound_kernel(__global int *d_psSum, int interval, int rLen, int numPart, __global Record* d_bound)
{
	const int tid=get_local_id(0);
	const int bid=get_group_id(0);		
	const int resultID=get_global_id(0);
	if(resultID<numPart)
	{
		int start=0;
		if(resultID==0)
			start=0;
		else
			start=d_psSum[interval*resultID];
		int end=0;
		if((resultID+1)==numPart)
			end=rLen;
		else
			end=d_psSum[interval*(resultID+1)];			
		d_bound[resultID].x=start;
		d_bound[resultID].y=end;
	}	
}
__kernel //kid 31
	void BitonicSort_kernel(__global Record * theArray,
                          const uint stage, 
						  const uint passOfStage,
						  const uint width,
						  const uint direction)
{
   uint sortIncreasing = direction;
   uint threadId = get_global_id(0);
    
    uint pairDistance = 1 << (stage - passOfStage);
    uint blockWidth   = 2 * pairDistance;

    uint leftId = (threadId % pairDistance) 
                   + (threadId / pairDistance) * blockWidth;

    uint rightId = leftId + pairDistance;
    
    Record leftElement = theArray[leftId];
    Record rightElement = theArray[rightId];
    
    uint sameDirectionBlockWidth = 1 << stage;
    
    if((threadId/sameDirectionBlockWidth) % 2 == 1)
        sortIncreasing = 1 - sortIncreasing;

    Record greater;
    Record lesser;
    if(leftElement.y > rightElement.y)
    {
        greater = leftElement;
        lesser  = rightElement;
    }
    else
    {
        greater = rightElement;
        lesser  = leftElement;
    }
    
    if(sortIncreasing)
    {
        theArray[leftId]  = lesser;
        theArray[rightId] = greater;
    }
    else
    {
        theArray[leftId]  = greater;
        theArray[rightId] = lesser;
    }
}

//NINLJ

//best with shared memory , with coaesced
__kernel //kid=38
void gpuNLJ_kernel(__global int* d_temp, __global Record *d_R, __global Record *d_S, 
			  int sStart, int rLen, int sLen, __global int *d_n) 
{
	__local Record shared_s[NLJ_S_BLOCK_SIZE];
	int bid=get_group_id(0)+get_group_id(1)*get_num_groups(0);
	int tid=get_local_id(0)+get_local_id(1)*get_local_size(0);
	int resultID=(bid)*NLJ_NUM_THREADS_PER_BLOCK+tid;
	int j=0;
	int i=0;
	int numResult=0;
	Record rTmp;
	sStart+=bid*NLJ_S_BLOCK_SIZE;
	int curPosInShared=0;
	for(i=0;i<NLJ_NUM_TUPLE_PER_THREAD;i++)
	{
		curPosInShared=tid+NLJ_NUM_THREADS_PER_BLOCK*i;
		if((curPosInShared+sStart)<sLen)
			shared_s[curPosInShared]=d_S[(curPosInShared+sStart)];
		else
			shared_s[curPosInShared].y=-1;
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	for(i = tid; (i) < rLen; i=i+NLJ_R_BLOCK_SIZE)
	{
		rTmp=d_R[i];
		d_temp[i] = d_R[i].x;
		for(j=0;j<NLJ_S_BLOCK_SIZE;j++)
		{
			if(PRED_EQUAL(rTmp.y, shared_s[j].y))
			{
				numResult++;
			}
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	d_n[resultID]=numResult;
}


__kernel void //kid=39
nlj_write_kernel(__global Record *d_R, __global Record *d_S,  int sStart, int rLen, int sLen, 
	  __global int *d_sum, __global Record *output)
{
	__local Record shared_s[NLJ_S_BLOCK_SIZE];
	int bid=get_group_id(0)+get_group_id(1)*get_num_groups(0);
	int tid=get_local_id(0)+get_local_id(1)*get_local_size(0);
	int resultID=(bid)*NLJ_NUM_THREADS_PER_BLOCK+tid;
	int j=0;
	int i=0;
	Record rTmp;
	
	int base=d_sum[resultID];
	//if(d_sum[bstartSum]!=d_sum[bendSum])
	//for(int sg=0;sg<NLJ_NUM_GRID_S;sg++)	
	{
		sStart+=bid*NLJ_S_BLOCK_SIZE;
		int curPosInShared=0;
		for(i=0;i<NLJ_NUM_TUPLE_PER_THREAD;i++)
		{
			curPosInShared=tid+NLJ_NUM_THREADS_PER_BLOCK*i;
			if((curPosInShared+sStart)<sLen)
				shared_s[curPosInShared]=d_S[(curPosInShared+sStart)];
			else
				shared_s[curPosInShared].y=-1;
		}		
		barrier(CLK_LOCAL_MEM_FENCE);
		
		for(i = tid; i < rLen; i=i+NLJ_R_BLOCK_SIZE)
		{
			rTmp=d_R[i];
			for(j=0;j<NLJ_S_BLOCK_SIZE;j++)
			{
				if(PRED_EQUAL(rTmp.y, shared_s[j].y))
				{
					output[base].x=rTmp.x;
					output[base].y=shared_s[j].x;
					base++;
				}
			}
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}
}
//cSS tree

unsigned int uintCeilingDiv(unsigned int dividend, unsigned int divisor)
{
	return (dividend + divisor - 1) / divisor;
}
unsigned int uintFloorLog(unsigned int base, unsigned int num)
{
	//unsigned int result = 0;

	//for(unsigned int temp = 0; temp <= num; temp *= base)
	//	result++;

	//return result;

	return (int)(log((float)(base))/log((float)(num)));
}



unsigned int uintCeilingLog(unsigned int base, unsigned int num)
{
	unsigned int result = 0;

	for(unsigned int temp = 1; temp < num; temp *= base)
		result++;

	return result;
}


unsigned int uintPower(unsigned int base, unsigned int pow)
{
	unsigned int result = 1;

	for(; pow; pow--)
		result *= base;

	return result;
}

int getRightMostDescIdx(int tree_size, int nodeIdx)
{
	int tmp = nodeIdx * TREE_NODE_SIZE + TREE_FANOUT;
	int n = uintCeilingLog(TREE_FANOUT, uintCeilingDiv(TREE_NODE_SIZE * tree_size + TREE_FANOUT, tmp)) - 1;

	int result = (tmp * uintPower(TREE_FANOUT, n) - TREE_FANOUT) / TREE_NODE_SIZE;
    return result; 
}

int firstMatchingKeyInDirNodeSM(__local IKeyType* keys, int key)
{
	int min = 0;
	int max = TREE_NODE_SIZE;
	int mid;
	int cut;
	while(max - min > 1) {
		mid = (min + max) / 2;
		cut = keys[mid];

		if(key > cut)
			min = mid;
		else
			max = mid;
	}

	if(keys[min] >= key)
		return min;

	return max;

}

int firstMatchingKeyInDirNode1(__global IKeyType* keys, int key)
{
	int min = 0;
	int max = TREE_NODE_SIZE;
	int mid;
	int cut;
	while(max - min > 1) {
		mid = (min + max) / 2;
		cut = keys[mid];

		if(key > cut)
			min = mid;
		else
			max = mid;
	}

	if(keys[min] >= key)
		return min;

	return max;

}



int getDataArrayIdx(int dirSize, int tree_size, int bottom_start, int treeIdx)
{
	int idx;
	if(treeIdx < dirSize) {
		idx = tree_size - bottom_start - 1;
	}
	else if( treeIdx < bottom_start ) {
		idx = tree_size - bottom_start + treeIdx - dirSize;
	}
	else {
		idx = treeIdx - bottom_start;
	}
	return idx;
}


int firstMatchingKeyInDataNode2(__global Record* records, IKeyType key)
{
	int min = 0;
	int max = TREE_NODE_SIZE;
	int mid;
	int cut;
	while(max - min > 1) {
		mid = (min + max) / 2;
		cut = records[mid].y;

		if(key > cut)
			min = mid;
		else
			max = mid;
	}

	if(records[min].y == key)
		return min;

	if(max < TREE_NODE_SIZE && records[max].y == key)
		return max;

	return -1;
}

__kernel //kid=40
void gSearchTree_kernel(__global IDataNode* data, int nDataNodes, 
				 __global IDirectoryNode* dir, int nDirNodes, int lvlDir, 
				 __global Record* arr, __global int* locations, int nSearchKeys, 
				 int nKeysPerThread, int tree_size, int bottom_start)
{
	// Bringing the root node (visited by every tuple) to the faster shared memory
	__local IKeyType RootNodeKeys[TREE_NODE_SIZE];
	int bid=get_group_id(0);
	int tid=get_local_id(0);
	RootNodeKeys[tid] = dir->keys[tid];

	barrier(CLK_LOCAL_MEM_FENCE);

	int OverallThreadIdx = bid * THRD_PER_BLCK_search + tid;

	for(int keyIdx = OverallThreadIdx; keyIdx < nSearchKeys; keyIdx += THRD_PER_GRID_search)
	{
		IKeyType val = arr[keyIdx].y;
		int loc = firstMatchingKeyInDirNodeSM(RootNodeKeys, val) + 1;
		for(int i = 1; i < lvlDir && loc < nDirNodes; i++) {
			int kid = firstMatchingKeyInDirNode1(dir[loc].keys, val);
			loc = loc * TREE_FANOUT + kid + 1;
		}

		if(loc >= tree_size)
			loc = nDataNodes - 1;
		else
			loc = getDataArrayIdx(nDirNodes, tree_size, bottom_start, loc);

		int offset = firstMatchingKeyInDataNode2(data[loc].records, val);
		locations[keyIdx] = (offset <0)?-1:(loc * TREE_NODE_SIZE + offset);
	}
}



__kernel//kid=41
void gIndexJoin_kernel(__global Record* R, int rLen, __global Record* S, __global int* g_locations, 
				int sLen, __global int* g_ResNums, int clusterSize)
{
	int bid=get_group_id(0);
	int tid=get_local_id(0);
	int cluster_id = bid * THRD_PER_BLCK_join + tid;

	if(cluster_id >= sLen) {
		g_ResNums[cluster_id] = 0;
		return;
	}

	int count = 0;

	int s_cur = cluster_id;
	int r_cur;
	int s_key;

	// Outputing to result
	while(s_cur < sLen) {
		r_cur = g_locations[s_cur];
		if(r_cur >= 0 && r_cur < rLen) {
			s_key = S[s_cur].y;
			while(s_key == R[r_cur].y) {
				count++;
				r_cur++;
			}
		}

		s_cur += THRD_PER_GRID_join;
	}

	g_ResNums[cluster_id] = count;
}


__kernel//kid=42
void gJoinWithWrite_kernel(__global Record* R, int rLen, __global Record* S, __global int* g_locations, 
					int sLen, __global int* g_PrefixSums, __global Record* g_joinResultBuffers, 
					int clusterSize)
{
	int bid=get_group_id(0);
	int tid=get_local_id(0);
	int cluster_id = bid * THRD_PER_BLCK_join + tid;

	if(cluster_id >= sLen) {
		return;
	}

	int s_cur = cluster_id;
	int r_cur;
	int s_key;

	//Record* pen = g_joinResultBuffers + (g_PrefixSums[cluster_id]);
	int curOffset=g_PrefixSums[cluster_id];
	// Outputing to result
	while(s_cur < sLen) {
		r_cur = g_locations[s_cur];
		if(r_cur >= 0 && r_cur < rLen) {
			s_key = S[s_cur].y;
			while(s_key == R[r_cur].y) {
				Record resultTuple;
				resultTuple.x=R[r_cur].x;
				resultTuple.y=S[s_cur].x;
				/*pen ->x = R[r_cur].x;
				pen ->y = S[s_cur].x;
				pen++;*/
				g_joinResultBuffers[curOffset]=resultTuple;
				curOffset++;
				r_cur++;
			}
		}

		s_cur += THRD_PER_GRID_join;
	}
}
__kernel//kid=43
void gCreateIndex_kernel(__global IDataNode* data, __global IDirectoryNode* dir, int dirSize, 
				  int tree_size, int bottom_start, int nNodesPerBlock)
{
		//int startIdx = blockIdx.x * nNodesPerBlock;
  //      int endIdx = startIdx + nNodesPerBlock;
  //      if(endIdx > dirSize)
  //              endIdx = dirSize;
  //      int keyIdx = threadIdx.x;
		//
		//dir[0].keys[0] = startIdx + keyIdx;
	int bid=get_group_id(0);
	int tid=get_local_id(0);

        int startIdx = bid * nNodesPerBlock;
        int endIdx = startIdx + nNodesPerBlock;
        if(endIdx > dirSize)
                endIdx = dirSize;
        int keyIdx = tid;

        // Proceed only when in internal nodes
        for(int nodeIdx = startIdx; nodeIdx < endIdx; nodeIdx++)
        {
                int childIdx = nodeIdx * TREE_FANOUT + keyIdx + 1;        // One step down to the left
                // Then look for the right most descendent
                int rightMostDesIdx;
                // Common cases
                if(childIdx < tree_size) {
                        rightMostDesIdx = getRightMostDescIdx(tree_size, childIdx);
                }
                // versus the unusual case when the tree is incomplete and the node does not have the full set of children
                else {
                        // pick the last node in the tree (largest element of the array)
                        rightMostDesIdx = tree_size - 1;
                }

                int dataArrayIdx = getDataArrayIdx(dirSize, tree_size, bottom_start, rightMostDesIdx);

                dir[nodeIdx].keys[keyIdx] = data[dataArrayIdx].records[TREE_NODE_SIZE - 1].y;
        }
}
__kernel //kid=44
void quanMap_kernel(__global Record *d_R, int interval, int rLen,
			   __global int *d_output, __local Record* tempBuf)  
{
	//extern __shared__ Record tempBuf[];
	int bid=get_group_id(0)+get_group_id(1)*get_num_groups(0);
	int tid=get_local_id(0)+get_local_id(1)*get_local_size(0);	
	const int numThread=get_local_size(0);
	const int resultID=(bid)*numThread+tid;
	if(resultID<rLen)
		tempBuf[tid]=d_R[resultID];
	barrier(CLK_LOCAL_MEM_FENCE);
	if(tid==0)
	{
		Record value;
		value.x=tempBuf[0].y;
		int numElement=0;
		if(resultID+interval>rLen)
			numElement=rLen-resultID;
		else
			numElement=interval;
		value.y=tempBuf[numElement-1].y+1;
		int curQuanPos=(resultID/interval)<<1;
		d_output[curQuanPos]=value.x;			
		d_output[curQuanPos+1]=value.y;			
	}

}


int firstMatchingKeyInDataNode_saven(__global Record* records, IKeyType key)
{
	int min = 0;
	int max = TREE_NODE_SIZE;
	int mid;
	int cut;
	while(max - min > 1) {
		mid = (min + max) / 2;
		cut = records[mid].y;

		if(key > cut)
			min = mid;
		else
			max = mid;
	}

	if(records[min].y >= key)
		return min;

	return max;
}


__kernel//kid=45
void gSearchTree_usingKeys_kernel(__global IDataNode* data, int nDataNodes,__global IDirectoryNode* dir, 
						   int nDirNodes, int lvlDir, __global int* arr, 
						   __global int* locations, int nSearchKeys, int nKeysPerThread, 
						   int tree_size, int bottom_start)
{
	int bid=get_group_id(0);
	int tid=get_local_id(0);
	// Bringing the root node (visited by every tuple) to the faster shared memory
	__local IKeyType RootNodeKeys[TREE_NODE_SIZE];
	RootNodeKeys[tid] = dir->keys[tid];

	barrier(CLK_LOCAL_MEM_FENCE);

	int OverallThreadIdx = bid * THRD_PER_BLCK_search + tid;

	for(int keyIdx = OverallThreadIdx; keyIdx < nSearchKeys; keyIdx += THRD_PER_GRID_search)
	{
		IKeyType val = arr[keyIdx];
		int loc = firstMatchingKeyInDirNodeSM(RootNodeKeys, val) + 1;
		for(int i = 1; i < lvlDir && loc < nDirNodes; i++) {
			int kid = firstMatchingKeyInDirNode1(dir[loc].keys, val);
			loc = loc * TREE_FANOUT + kid + 1;
		}

		if(loc >= tree_size)
			loc = nDataNodes - 1;
		else
			loc = getDataArrayIdx(nDirNodes, tree_size, bottom_start, loc);

		int offset = firstMatchingKeyInDataNode_saven(data[loc].records, val);
		locations[keyIdx] = loc * TREE_NODE_SIZE + offset;
	}
}



//sort merge join.

int findNumResultInChunk(__local Record* records, int key)
{
	int min = 0;
	int max = SMJ_NUM_THREADS_PER_BLOCK;
	int mid;
	int cut;
	while(max - min > 1) {
		mid = (min + max) / 2;
		cut = records[mid].y;

		if(key > cut)
			min = mid;
		else
			max = mid;
	}

	if(records[min].y >= key)
		return min;
	else
		return max;
}

__kernel void //kid 46
joinMBCount_kernel(__global Record *d_R, int rLen, __global Record* d_S, 
				   int sLen, __global int *d_quanLocS, int numQuan, 
				   __global int *d_n)  
{
	__local Record tempBuf_R[SMJ_NUM_THREADS_PER_BLOCK];
	__local int sStart;
	__local int sEnd;
	int bid=get_group_id(0);
	int tid=get_local_id(0);
	const int resultID=get_global_id(0);
	int numResult=0;
	if(bid<numQuan)
	{
		if(resultID<rLen)
			tempBuf_R[tid]=d_R[resultID];
		else
			tempBuf_R[tid].y=TEST_MAX;
		if(tid==0)
		{
			sStart=d_quanLocS[bid<<1];
			sEnd=d_quanLocS[(bid<<1)+1];
		}
		barrier(CLK_LOCAL_MEM_FENCE);
		int pos=0;
		Record tempValue;
		int i=0;
		int startPos=0;
		for(pos=sStart;(pos+tid)<sEnd;pos+=SMJ_NUM_THREADS_PER_BLOCK)
		{
			tempValue=d_S[pos+tid];
			//for(i=0;i<SMJ_NUM_THREADS_PER_BLOCK;i++)
			//	if(tempValue.y==tempBuf_R[i].y)
			//		numResult++;
			//numResult+=findNumResultInChunk_seq(tempBuf_R,tempValue.y);
			startPos=findNumResultInChunk(tempBuf_R,tempValue.y);
			for(i=startPos;i<SMJ_NUM_THREADS_PER_BLOCK;i++)
				if(tempBuf_R[i].y==tempValue.y)
					numResult++;
				else
					if(tempBuf_R[i].y>tempValue.y)
						break;
		}
	}
	else
		numResult=0;
	d_n[resultID]=numResult;
}

//best, with shared memory, with coalesced
__kernel void //kid=47
joinMBWrite_kernel(__global Record *d_R, int rLen, __global Record* d_S, 
				   int sLen, __global int *d_quanLocS, int numQuan, 
				   __global int *d_sum, __global Record *d_output)  
{
	__local Record tempBuf_R[SMJ_NUM_THREADS_PER_BLOCK];
	__local int sStart;
	__local int sEnd;
	int bid=get_group_id(0);
	int tid=get_local_id(0);	
	const int resultID=get_global_id(0);
	//int numResult=0;
	if(bid<numQuan)
	{
		if(resultID<rLen)
			tempBuf_R[tid]=d_R[resultID];
		else
			tempBuf_R[tid].y=TEST_MAX;
		if(tid==0)
		{
			sStart=d_quanLocS[bid<<1];
			sEnd=d_quanLocS[(bid<<1)+1];
		}
		barrier(CLK_LOCAL_MEM_FENCE);
		int pos=0;
		Record tempValue;
		int i=0;
		int startPos=0;
		int base=d_sum[resultID];
		for(pos=sStart;(pos+tid)<sEnd;pos+=SMJ_NUM_THREADS_PER_BLOCK)
		{
			tempValue=d_S[pos+tid];
			//for(i=0;i<SMJ_NUM_THREADS_PER_BLOCK;i++)
			//	if(tempValue.y==tempBuf_R[i].y)
			//		numResult++;
			//numResult+=findNumResultInChunk_seq(tempBuf_R,tempValue.y);
			startPos=findNumResultInChunk(tempBuf_R,tempValue.y);
			for(i=startPos;i<SMJ_NUM_THREADS_PER_BLOCK;i++)
				if(tempBuf_R[i].y==tempValue.y)
				{
					d_output[base].x=tempBuf_R[i].x;
					d_output[base].y=tempValue.x;
					base++;
				}
				else
					if(tempBuf_R[i].y>tempValue.y)
						break;
		}
	}
}


//hash join



//	Histo: scan R and get d_PidHisto[pn] of each thread
//		p=hash(r); histo[p]++; sync; d_PidHisto[snakewise p]=histo;
__kernel //kid=48
 void Histo_kernel(__global int* d_HistoMat, __global Record* d_R, const int nR, 
					const int pn, const int shift, __local int* s_histo)
{
	const int gridDimX=get_num_groups(0);
	const int blockDimX=get_local_size(0);
	const int bidX=get_group_id(0);
	const int tidX=get_local_id(0);
	const int gridLen = gridDimX * blockDimX;
	int offset = bidX * blockDimX + tidX;
	int pid;
	//extern __local int s_histo[];
	for(int pi = 0; pi < pn; ++pi)
	{
		s_histo[tidX * pn + pi] = 0;
	}
	while(offset < nR)
	{
		pid = (HASH(d_R[offset].y) >> shift) & (pn - 1);	
		++s_histo[tidX * pn + pid];
		offset = offset + gridLen;
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	offset = bidX * blockDimX + tidX;
	for(int pi = 0; pi < pn; ++pi)
	{
		d_HistoMat[pi * gridLen + offset] = s_histo[tidX * pn + pi];
	}
}


__kernel//kid=49
 void Reorder_kernel(__global Record* d_R1, __global int* d_PBound, __global Record* d_R, 
					  __global int* d_WriteLoc, const int nR, const int pn, 
					  const int shift, __local int* s_writeLoc)
{
	const int gridDimX=get_num_groups(0);
	const int blockDimX=get_local_size(0);
	const int bidX=get_group_id(0);
	const int tidX=get_local_id(0);
	int gridLen = gridDimX * blockDimX;
	int pid;
	Record recR;
	//extern __local int s_writeLoc[];
	int offset = bidX * blockDimX + tidX;
	for(int pi = 0; pi < pn; ++pi)
	{
		s_writeLoc[tidX * pn + pi] = d_WriteLoc[pi * gridLen + offset];
	}
    if(offset == 0)//this thread's corresponding HistMatPre contains d_partbound2
	{
		for(int pi = 0; pi < pn; ++pi)
		{
			d_PBound[pi] = s_writeLoc[tidX * pn + pi];
		}
		d_PBound[pn] = nR;
	}

	//v0: single pass
	while(offset < nR)
	{
		recR = d_R[offset];
		pid = tidX * pn + ((HASH(recR.y) >> shift) & (pn - 1));
		//pid = tidX * pn + d_Pid[offset];
		d_R1[s_writeLoc[pid]] = recR;
		++s_writeLoc[pid];
		offset = offset + gridLen;
	}
	
}


// histo all parents in 1 kernel. Parent i corresponds to HistM[i*Bp]
//	for each parent, scan R and get d_PID and d_PidHisto of each thread
//		p=d_PID[r]=hash(r); histo[p]++; sync; d_PidHisto[snakewise p]=histo;
__kernel//kid=50
void Histo3_kernel(__global int* d_HistoMat, __global Record* d_R, __global int* d_PBound, 
					 const int nParent, const int pn, const int shift,
					 __local int* s_histo)
{
	const int gridDimX=get_num_groups(0);
	const int blockDimX=get_local_size(0);
	const int bidX=get_group_id(0);
	const int tidX=get_local_id(0);
	const int bp = gridDimX / nParent;			//num blocks per part
	const int partIdx = bidX / bp;		//part#
	const int partStart = d_PBound[partIdx];	//start rec idx
	const int partEnd =  d_PBound[partIdx + 1];	//end rec idx
	const int bx = bidX;					//blockIdx
	const int bx0 = partIdx * bp ;				//start block idx
	const int tn = blockDimX;					//num threads in this block
	const int tx = tidX;

	//extern __shared__ int s_histo[];
	//reset histo of this block
	for(int pi = 0; pi < pn; ++pi)
		s_histo[tx * pn + pi] = 0;

	//cumu
	int offset = partStart + (bx - bx0) * tn + tx;	//recIdx
	int pid;
	while(offset < partEnd)
	{
		pid = (HASH(d_R[offset].y) >> shift) & (pn - 1);	
		++s_histo[tx * pn + pid];
		offset += bp * tn;
	}
	
	//write
	offset = bx0 * tn * pn + (bx - bx0) * tn + tx;
	for(int pi = 0; pi < pn; ++pi)
	{
		d_HistoMat[pi * (bp * tn) + offset] = s_histo[tx * pn + pi];
	}
}

//Permute all parents in 1 kernel. Parent i corresponds to HistM[i*Bp]
//	for each parent, scan R and get d_PID and d_PidHisto of each thread
//		p=d_PID[r]=hash(r); histo[p]++; sync; d_PidHisto[snakewise p]=histo;

//write d_R to d_R1 according to d_Loc; write d_RDir to d_RDir1 using d_Loc
__kernel //kid=51
void Reorder3_kernel(__global Record* d_R1, __global int* d_RDir1, __global Record* d_R, 
					   __global int* d_RDir, __global int* d_Loc, const int nParent, 
					   const int pn, const int shift, __local int* s_writeLoc)
{
	const int gridDimX=get_num_groups(0);
	const int blockDimX=get_local_size(0);
	const int bidX=get_group_id(0);
	const int tidX=get_local_id(0);
	const int bp = gridDimX / nParent;			//num blocks per part
	const int partIdx = bidX / bp;		//part#
	const int partStart = d_RDir[partIdx];	//start rec idx
	const int partEnd =  d_RDir[partIdx + 1];	//end rec idx
	const int bx = bidX;					//blockIdx
	const int bx0 = partIdx * bp ;				//start block idx
	const int tn = blockDimX;					//num threads in this block
	const int tx = tidX;
	
//	extern __local int s_writeLoc[];
	int offset = bx0 * tn * pn + (bx - bx0) * tn + tx;
	for(int pi = 0; pi < pn; ++pi)
		s_writeLoc[tx * pn + pi] = partStart + d_Loc[pi * (bp * tn) + offset];
	
    if((bx == bx0) && (tx == 0))//this writeLoc is also the child partition's start
	{
		for(int pi = 0; pi < pn; ++pi)
		{
			d_RDir1[partIdx * pn + pi] = s_writeLoc[tx * pn + pi];
		}
		d_RDir1[partIdx * pn + pn] = partEnd;
	}

	//scatter
	offset = partStart + (bx - bx0) * tn + tx;	//recIdx
	int pid;
	Record rec;
	while(offset < partEnd)
	{		
		rec = d_R[offset];
		pid = ((HASH(rec.y) >> shift) & (pn - 1));
		d_R1[s_writeLoc[tx * pn + pid]] = rec;
		++s_writeLoc[tx * pn + pid];
		offset += bp * tn;
	}
}

void swapRec(__local Record* a, __local Record* b)
{
	// Alternative swap doesn't use a temporary register:
	 //a ^= b;
	 //b ^= a;
	 //a ^= b;
	
    Record tmp = a[0];
    a[0] = b[0];
    b[0] = tmp;
}

__kernel//kid=52
 void ProbePreSort_kernel(__global Record* d_R, __global int* d_RDir, int nR, 
						   __global Record* d_S, __global int* d_SDir, int nS, 
						   int PnG, __local Record* shared)
{
	const int gridDimX=get_num_groups(0);
	const int blockDimX=get_local_size(0);
	const int bidX=get_group_id(0);
	const int tidX=get_local_id(0);
	const int partId = get_group_id(1) * gridDimX + bidX;
	if(partId >= PnG)
		return;

	const int r1 = d_RDir[partId];
	const int r2 = d_RDir[partId + 1];
	const int s1 = d_SDir[partId];
	const int s2 = d_SDir[partId + 1];

	if((r2 - r1 > _maxPartLen) && (s2 - s1 > _maxPartLen))	//overflow shared memory
		return;

	const bool bLoadR = ((r2 - r1) <= (s2 - s1));	//load r or s into SM
	//extern __shared__ Record shared[];
    const int tid = tidX;
	
	shared[tid].y = INT_MAX;
	if(bLoadR)
	{
		if(tid < r2 - r1)
			shared[tid] = d_R[r1 + tid];
	}
	else
	{
		if(tid < s2 - s1) 
			shared[tid] = d_S[s1 + tid];
	}

    barrier(CLK_LOCAL_MEM_FENCE);

    // Parallel bitonic sort.

	for (int k = 2; k <= blockDimX; k *= 2)
	{
		// Bitonic merge:
		for (int j = k / 2; j>0; j /= 2)
		{
			int ixj = tid ^ j;
 			if (ixj > tid)	//should carry the INTMAX together, o/w will err!:  if ((ixj > tid) && (tid < partLen))
			{
				if ((tid & k) == 0)
				{
					if (shared[tid].y > shared[ixj].y)
					{
						//swapRec(shared[tid], shared[ixj]);
						swapRec(shared+tid, shared+ixj);
					}
				}
				else
				{
					if (shared[tid].y < shared[ixj].y)
					{
						//swapRec(shared[tid], shared[ixj]);
						swapRec(shared+tid, shared+ixj);
					}
				}
			}
            
			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}

	// Write result.
	if(bLoadR)
	{
		if(tid < r2 - r1)
			d_R[r1 + tid] = shared[tid];
	}
	else
	{
		if(tid < s2 - s1) 
			d_S[s1 + tid] = shared[tid];
	}
}


int FirstHit(__local Record* A, int N, int val)
{
	//v1: 300ms
	int low = 0;
	int high = N - 1;
	int p = low + (high - low) / 2;	//Initial probe position
    while(low <= high)
	{
		if(A[p].y > val)
			high = p -1;
		else if(A[p].y < val)
			low = p + 1;
		else
			return p;
		p = low + (high - low) / 2;
	}
	return -1;
}


//find cnt for each threads
__kernel //kid=53
void Probe_Cnt_kernel(__global int* d_ThreadCnts, __global int* d_skewPid, __global Record* d_R, 
						__global  int* d_PBoundR, __global Record* d_S,__global  int* d_PBoundS, 
						const int PnG, __local Record* s_Table)
{
	//extern __shared__ Record s_Table[];
	const int gridDimX=get_num_groups(0);
	const int blockDimX=get_local_size(0);
	const int bidX=get_group_id(0);
	const int tidX=get_local_id(0);
	int count = 0;	//important
	int px = bidX;	//part idx
	Record tmpRec;
	int r1, r2, s1, s2;
	int offset/*, firstHit*/;
	while(px < PnG)
	{
		r1 = d_PBoundR[px];
		r2 = d_PBoundR[px + 1];
		s1 = d_PBoundS[px];
		s2 = d_PBoundS[px + 1];
		if((r2 - r1 > _maxPartLen) && (s2 - s1 > _maxPartLen))
		{
			d_skewPid[px] = 1;
			barrier(CLK_LOCAL_MEM_FENCE);	//avoid next s_Table[] be dirtied
			px = px + gridDimX;	//skip, leave to later
		}
		else if((r2 - r1) <= (s2 - s1))	//read R, scan S
		{
			offset = r1 + tidX;
			for(int i = 0; offset < r2; ++i)
			{
				s_Table[tidX + i * blockDimX] = d_R[offset];
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			//read S
			offset = s1 + tidX;
			for(int i = 0; offset < s2; ++i)
			{
				tmpRec = d_S[offset];
/*//#ifdef _bSortPart
				firstHit = FirstHit(s_Table, r2 - r1, tmpRec.y);
				if(firstHit >= 0)
				{
					do{
						++count;
						++firstHit;
					}while(s_Table[firstHit].y == tmpRec.y);
				}
#else*/
				for(int j = 0; j < (r2 - r1); ++j)
				{
					if(s_Table[j].y == tmpRec.y)
					{
						++count; //lazy: tune: to use break, only find unique results!
					}
				}
//#endif
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);	//avoid next s_Table[] be dirtied
			px = px + gridDimX;
		}
		else//((r2 - r1) >= (s2 - s1))	//read S, scan R
		{
			offset = s1 + tidX;
			for(int i = 0; offset < s2; ++i)
			{
				s_Table[tidX + i * blockDimX] = d_S[offset];
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			//read R
			offset = r1 + tidX;
			for(int i = 0; offset < r2; ++i)
			{
				tmpRec = d_R[offset];
/*//#ifdef _bSortPart
				firstHit = FirstHit(s_Table, s2 - s1, tmpRec.y);
				if(firstHit >= 0)
				{
					do{
						++count;
						++firstHit;
					}while(s_Table[firstHit].y == tmpRec.y);
				}
#else*/
				for(int j = 0; j < (s2 - s1); ++j)
				{
					if(s_Table[j].y == tmpRec.y)
					{
						++count; //lazy: tune: to use break, only find unique results!
					}
				}
//#endif
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);	//avoid next s_Table[] be dirtied
			px = px + gridDimX;
		}
	}
	d_ThreadCnts[bidX * blockDimX + tidX] = count;
}


//use s to probe r, write to global positions that d_ThreadCntsPresum indicates
__kernel //kid=54
void Probe_Write_kernel(__global Record* d_RS, __global int* d_ThreadCntsPresum, __global Record* d_R, 
						  __global int* d_PBoundR,__global  Record* d_S, __global  int* d_PBoundS, 
						  const int PnG, __local Record* s_Table)
{
	//extern __shared__ Record s_Table[];
	const int gridDimX=get_num_groups(0);
	const int blockDimX=get_local_size(0);
	const int bidX=get_group_id(0);
	const int tidX=get_local_id(0);
	int outputPos = d_ThreadCntsPresum[bidX * blockDimX + tidX];	//output's starting offset
	int px = bidX;	//part idx
	Record tmpRec;
	Record tmpRS;
	int r1, r2, s1, s2;
	int offset/*, firstHit*/;
	while(px < PnG)
	{
		r1 = d_PBoundR[px];
		r2 = d_PBoundR[px + 1];
		s1 = d_PBoundS[px];
		s2 = d_PBoundS[px + 1];
		if((r2 - r1 > _maxPartLen) && (s2 - s1 > _maxPartLen))
		{
			barrier(CLK_LOCAL_MEM_FENCE);	//avoid next s_Table[] be dirtied
			px = px + gridDimX;	//skip, leave to later
		}
		else if((r2 - r1) <= (s2 - s1))	//read R, scan S
		{
			offset = r1 + tidX;

			for(int i = 0; offset < r2; ++i)
			{
				s_Table[tidX + i * blockDimX] = d_R[offset];
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			//scan S
			offset = s1 + tidX;

			for(int i = 0; offset < s2; ++i)
			{
				tmpRec = d_S[offset];
/*//#ifdef _bSortPart
				firstHit = FirstHit(s_Table, r2 - r1, tmpRec.y);
				if(firstHit >= 0)
				{
					do{
						tmpRS.x = s_Table[firstHit].x;	//x
						tmpRS.y = tmpRec.x;	//sid
						d_RS[outputPos] = tmpRS;
						++outputPos;
						++firstHit;
					}
					while(s_Table[firstHit].y == tmpRec.y);
				}
#else*/
				for(int j = 0; j < (r2 - r1); ++j)
				{
					if(s_Table[j].y == tmpRec.y)
					{
						tmpRS.x = s_Table[j].x;	//x
						tmpRS.y = tmpRec.x;	//sid
						d_RS[outputPos] = tmpRS;
						++outputPos;
					}
				}
//#endif
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);	//avoid next s_Table[] be dirtied
			px = px + gridDimX;
		}
		else//((r2 - r1) >= (s2 - s1))	//read S, scan R
		{
			offset = s1 + tidX;
			for(int i = 0; offset < s2; ++i)
			{
				s_Table[tidX + i * blockDimX] = d_S[offset];
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			//scan R
			offset = r1 + tidX;
			for(int i = 0; offset < r2; ++i)
			{
				tmpRec = d_R[offset];
/*#ifdef _bSortPart
				firstHit = FirstHit(s_Table, s2 - s1, tmpRec.y);
				if(firstHit >= 0)
				{
					do{
						tmpRS.x = tmpRec.x;	//x
						tmpRS.y = s_Table[firstHit].x;	//sid
						d_RS[outputPos] = tmpRS;
						++outputPos;
						++firstHit;
					}
					while(s_Table[firstHit].y == tmpRec.y);
				}

#else*/
				for(int j = 0; j < (s2 - s1); ++j)
				{
					if(s_Table[j].y == tmpRec.y)
					{
						tmpRS.x = tmpRec.x;			//x
						tmpRS.y = s_Table[j].x;	//sid
						d_RS[outputPos] = tmpRS;
						++outputPos;
					}
				}
//#endif
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);	//avoid next s_Table[] be dirtied
			px = px + gridDimX;
		}
	}
}

//the overflowed partition falls naturally into some fragments, each length parLen/shareMemsize. then each block handles one frag in strip-mining way.
__kernel //kid=55
void Probe_CntOverflow_kernel(__global int* d_ThreadCnts, const int skewPid, __global Record* d_R, 
								__global  int* d_PBoundR,__global  Record* d_S, __global  int* d_PBoundS,
								__local Record* s_Table)
{
	//extern __shared__ Record s_Table[];
	const int gridDimX=get_num_groups(0);
	const int blockDimX=get_local_size(0);
	const int bidX=get_group_id(0);
	const int tidX=get_local_id(0);
	const int rStart = d_PBoundR[skewPid];
	const int rEnd = d_PBoundR[skewPid + 1];
	const int sStart = d_PBoundS[skewPid];
	const int sEnd = d_PBoundS[skewPid + 1];
	int count = 0;
	Record tmpRec;
	if((rEnd - rStart) <= (sEnd - sStart))
	{
		const int nFrag = (int)ceilf((float)(rEnd - rStart) / _maxPartLen);
		int FIdx = bidX;
		while(FIdx < nFrag)
		{
			//read r fragment
			int fragStart = rStart + FIdx * _maxPartLen;
			int fragEnd = (fragStart + _maxPartLen < rEnd)? (fragStart + _maxPartLen) : rEnd;
			int offset = fragStart + tidX;	//global
			for(int i = 0; offset < fragEnd; ++i)
			{
				s_Table[tidX + i * blockDimX] = d_R[offset];
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			//read whole S partition
			offset = sStart + tidX;
			for(int i = 0; offset < sEnd; ++i)
			{
				tmpRec = d_S[offset];
				for(int j = 0; j < fragEnd - fragStart; ++j)
				{
					if(s_Table[j].y == tmpRec.y)
						++count;
				}
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			FIdx = FIdx + gridDimX;
		}
	}
	else //(rEnd - rStart) >= (sEnd - sStart)), use s as inner table
	{
		int nFrag = (int)ceilf((float)(sEnd - sStart) / _maxPartLen);
		int FIdx = bidX;
		while(FIdx < nFrag)
		{
			//read s fragment
			int fragStart = sStart + FIdx * _maxPartLen;
			int fragEnd = (fragStart + _maxPartLen < sEnd)? (fragStart + _maxPartLen) : sEnd;
			int offset = fragStart + tidX;	//global
			for(int i = 0; offset < fragEnd; ++i)
			{
				s_Table[tidX + i * blockDimX] = d_S[offset];
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			//read whole R partition
			offset = rStart + tidX;
			for(int i = 0; offset < rEnd; ++i)
			{
				tmpRec = d_R[offset];
				for(int j = 0; j < fragEnd - fragStart; ++j)
				{
					if(s_Table[j].y == tmpRec.y)
						++count;
				}
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			FIdx = FIdx + gridDimX;
		}
	}
	d_ThreadCnts[bidX * blockDimX + tidX] = count;
}

//the overflowed partition falls naturally into some fragments, each length parLen/shareMemsize. then each block handles one frag in strip-mining way.
__kernel//kid=56
void Probe_WriteOverflow_kernel(__global Record* d_RS, __global int* d_WriteLoc, const int skewPid, 
								  __global Record* d_R, __global  int* d_PBoundR, __global Record* d_S, 
								__global  int* d_PBoundS, __local Record* s_Table)
{
	//extern __shared__ Record s_Table[];
	const int gridDimX=get_num_groups(0);
	const int blockDimX=get_local_size(0);
	const int bidX=get_group_id(0);
	const int tidX=get_local_id(0);
	const int rStart = d_PBoundR[skewPid];
	const int rEnd = d_PBoundR[skewPid + 1];
	const int sStart = d_PBoundS[skewPid];
	const int sEnd = d_PBoundS[skewPid + 1];
	int outputPos = d_WriteLoc[bidX * blockDimX + tidX];
		
	Record tmpRec, tmpRS;
	if((rEnd - rStart) <= (sEnd - sStart))
	{
		const int nFrag = (int)ceilf((float)(rEnd - rStart) / _maxPartLen);
		int FIdx = bidX;
		while(FIdx < nFrag)
		{
			//read r fragment
			int fragStart = rStart + FIdx * _maxPartLen;
			int fragEnd = min(fragStart + _maxPartLen, rEnd);
			int offset = fragStart + tidX;	//global
			for(int i = 0; offset < fragEnd; ++i)
			{
				s_Table[tidX + i * blockDimX] = d_R[offset];
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			//read whole S partition
			offset = sStart + tidX;
			for(int i = 0; offset < sEnd; ++i)
			{
				tmpRec = d_S[offset];
				for(int j = 0; j < fragEnd - fragStart; ++j)
				{
					if(s_Table[j].y == tmpRec.y)
					{
						tmpRS.x = s_Table[j].x;	//x
						tmpRS.y = tmpRec.x;	//sid
						d_RS[outputPos] = tmpRS;
						++outputPos;
					}
				}
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			FIdx = FIdx + gridDimX;
		}
	}
	else //(rEnd - rStart) >= (sEnd - sStart)), use s as inner table
	{
		int nFrag = (int)ceilf((float)(sEnd - sStart) / _maxPartLen);
		int FIdx = bidX;
		while(FIdx < nFrag)
		{
			//read s fragment
			int fragStart = sStart + FIdx * _maxPartLen;
			int fragEnd = (fragStart + _maxPartLen < sEnd)? (fragStart + _maxPartLen) : sEnd;
			int offset = fragStart + tidX;	//global
			for(int i = 0; offset < fragEnd; ++i)
			{
				s_Table[tidX + i * blockDimX] = d_S[offset];
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			//read whole R partition
			offset = rStart + tidX;
			for(int i = 0; offset < rEnd; ++i)
			{
				tmpRec = d_R[offset];
				for(int j = 0; j < fragEnd - fragStart; ++j)
				{
					if(s_Table[j].y == tmpRec.y)
					{
						tmpRS.x = tmpRec.x;			//x
						tmpRS.y = s_Table[j].x;	//sid
						d_RS[outputPos] = tmpRS;
						++outputPos;
					}
				}
				offset = offset + blockDimX;
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			FIdx = FIdx + gridDimX;
		}
	}
}



//segmented prefix scan

__kernel //kid=57
void segPS_kernel(__global int* d_input, int rLen, int segSize, 
					__global int* d_output, __local int* shared)
{
	int bid=get_group_id(0)+get_num_groups(0)*get_group_id(1);
	int tid=get_local_id(0);
	int delta=get_local_size(0);
	int startPos=bid*segSize;
	int endPos=(bid+1)*segSize;
	endPos=(endPos>rLen)?rLen:endPos;
	int curSum=0;
	for(int pos=startPos+tid;pos<endPos;pos+=delta)
	{
		shared[tid]=d_input[pos];
		barrier(CLK_LOCAL_MEM_FENCE);
		if(tid==0)
		{
			for(int i=0;(i<delta);i++)
			{
				int v=shared[i];
				shared[i]=curSum;
				curSum+=v;				
			}
		}
		barrier(CLK_LOCAL_MEM_FENCE);
		//output
		d_output[pos]=shared[tid];
	}
}

//djb2_hash hash function
uint djb2_hash(uint  key, uint mod){
	unsigned long hash = 5381;
	uint tempKey[4];
	tempKey[0] = (0x000000ff & key) >> 0;
	tempKey[1] = (0x0000ff00 & key) >> 8;
	tempKey[2] = (0x00ff0000 & key) >> 16;
	tempKey[3] = (0xff000000 & key) >> 24;
    for(uint i = 0; i < 4; i++)
        hash = ((hash << 5) + hash) + tempKey[i];
    return hash % mod;
}

//murmurhash2 hash function
unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed )
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;
	unsigned int h = seed ^ len;

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;
		k *= m; 
		k ^= k >> r; 
		k *= m; 

		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch(len)
	{
		case 3: h ^= data[2] << 16;
		case 2: h ^= data[1] << 8;
		case 1: h ^= data[0];
			h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}


uint sim_hash(uint key, uint mod)
{
	return key % mod;
}

__kernel //kid 49
void build_kernel(__global uint * rTableOnDevice,
	       __global uint * rHashTable,
		   const uint      rTupleNum,
		   const uint      sTupleNum,
		   const uint      rHashTableBucketNum)
{
	uint numWorkItems = get_global_size(0);
    uint tid          = get_global_id(0);
	uint key, val, hash, count;
	uint hashBucketSize = rTupleNum/rHashTableBucketNum; //number of tuples inserted into each bucket

	while (tid < rTupleNum)
	{
		key = rTableOnDevice[tid * 2 + 0]; //get the key of one tuple
		val = rTableOnDevice[tid * 2 + 1]; //get the value of one tuple

		hash = djb2_hash(key,rHashTableBucketNum);
	
		//check to find the free memory address
		if((count = atomic_inc(&rHashTable[hash * (1 + hashBucketSize * 2)])) < hashBucketSize) //if this bucket is not full, add it
		{
			rHashTable[1 + hash * (1 + hashBucketSize * 2) + count * 2 + 0] = key;
			rHashTable[1 + hash * (1 + hashBucketSize * 2) + count * 2 + 1] = val;
		}

		tid += numWorkItems;
	}
}

__kernel //kid 50
void probe_kernel(__global uint * rHashTable, 
           __global uint * sTableOnDevice, 
		   __global uint * matchedTable,
		   const uint      rTupleNum, 
		   const uint      sTupleNum, 
		   const uint      rHashTableBucketNum, 
		   const uint      matchedTupleNum)
{
	uint numWorkItems = get_global_size(0);
    uint tid          = get_global_id(0);

	uint key, val, hash, count, matchedNum;
	uint hashBucketSize = rTupleNum/rHashTableBucketNum;

	while(tid < sTupleNum)
	{
		//get one tuple from S table
		key = sTableOnDevice[tid * 2 + 0];
		val = sTableOnDevice[tid * 2 + 1];
		
		//since hash value calculation consumes only tens ms, so GPU will finish it first
		hash = djb2_hash(key,rHashTableBucketNum);

		//find out matched tuples in hash table for R table
		count = 0;
		while(count < hashBucketSize)
		{
			if(rHashTable[1 + hash * (1 + hashBucketSize * 2) + count * 2] == key) //compare the key
			{
				matchedNum = atomic_inc(&matchedTable[0]);
				matchedTable[4 + matchedNum * 4 + 0] = key;
				matchedTable[4 + matchedNum * 4 + 1] = val;
				matchedTable[4 + matchedNum * 4 + 2] = key;
				matchedTable[4 + matchedNum * 4 + 3] = rHashTable[1 + hash * (1 + hashBucketSize * 2) + count * 2 + 1];
			}
			count++;
		}
		
		tid += numWorkItems;
	}
}