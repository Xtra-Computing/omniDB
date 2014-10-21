#include "common.h"
#include "PrimitiveCommon.h"
#include "Helper.h"
void validateScan( int* input, int rLen, int* output )
{
	int* checkOutput = (int*)malloc( sizeof(int)*rLen );

	checkOutput[0] = 0;

	for( int i = 1; i < rLen; i++ )
	{
		checkOutput[i] = checkOutput[i - 1] + input[i - 1];
	}

	bool pass = true;
	for( int i = 0; i < rLen; i++ )
	{

		//printf("checkOutput[%d ]=%d , output[%d]=%d",i,checkOutput[i],i,output[i]);
		if( checkOutput[i] != output[i] )
		{
			printf("output[%d]=%d",i,output[i]);
			pass = false;
			printf( "!!!error\n" );
			break;
		}
	}

	if( pass )
	{
		printf( "scan test pass!\n" );
	}
	else
	{
		printf( "scan test failed!\n" );
	}
}

void validateProjection( Record* h_Rin, int rLen, Record* originalProjTable, Record* h_projTable, int pLen )
{
	Record* checkProjTable = (Record*)malloc( sizeof(Record)*pLen );
	bool pass = true;

	int timer=DLL_genTimer(0);

	//startTimer( &timer );
	for( int i = 0; i < pLen; i++ )
	{
		checkProjTable[i].x = originalProjTable[i].x;
		checkProjTable[i].y = h_Rin[originalProjTable[i].x].y;
	}
	endTimer( "cpu projection", &timer );

	for( int i = 0; i < pLen; i++ )
	{
		if( (h_projTable[i].x != checkProjTable[i].x) || (h_projTable[i].y != h_projTable[i].y) )
		{
			pass = false;
			break;
		}
	}

	if( pass )
	{
		//printf( "projection test pass!\n" );
	}
	else
	{
		printf( "!error, porjection test failed! \n" );
	}
}

void validateAggAfterGroupBy( Record * h_Rin, int rLen, int* startPos, int numGroups, Record * Ragg, int* aggResults, int OPERATOR )
{
	bool pass = true;

	int* checkResult = (int*)malloc( sizeof(int)*numGroups );
	int result;
	//int groupIdx = 0;

	Record* S = (Record*)malloc( sizeof(Record)*rLen );
	for( int i = 0; i < rLen; i++ )
	{
		S[i] = Ragg[h_Rin[i].x];
	}

	//aggregation
	int timer=DLL_genTimer(0);
	//startTimer( &timer );
			
	int* endPos = (int*)malloc( sizeof(int)*numGroups );
	for( int i = 0; i < numGroups - 1; i++ )
	{
		endPos[i] = startPos[i + 1];
	}
	endPos[numGroups - 1] = rLen;

	for( int i = 0; i < numGroups; i++ )
	{
		if( OPERATOR == REDUCE_MAX )
		{
			result = 0;
			for( int j = startPos[i]; j < endPos[i]; j++ )
			{
				if( S[j].y > result )
				{
					result = S[j].y;
				}
			}
			checkResult[i] = result;
		}
		else if( OPERATOR == REDUCE_MIN )
		{
			result = TEST_MAX;
			for( int j = startPos[i]; j < endPos[i]; j++ )
			{
				if( S[j].y < result )
				{
					result = S[j].y;
				}
			}
			checkResult[i] = result;
		}
		else if( OPERATOR == REDUCE_SUM )
		{
			result = 0;
			for( int j = startPos[i]; j < endPos[i]; j++ )
			{
				result += S[j].y;
			}
			checkResult[i] = result;
		}
		else if( OPERATOR == REDUCE_AVERAGE )
		{
			result = 0;
			for( int j = startPos[i]; j < endPos[i]; j++ )
			{
				result += S[j].y;
			}
			checkResult[i] = result/(endPos[i] - startPos[i]);
		}
	}
	endTimer( "cpu aggregration after group by", &timer );

	for( int i = 0; i < numGroups; i++ )
	{
		if( checkResult[i] != aggResults[i] )
		{
			printf( "Aggregrate test failed!\n" );
			pass = false;
			break;
		}
	}

	if( pass == true )
	{
		printf( "Test Passed!\n" );
	}
	
	free( S );
}

void validateGroupBy( Record* h_Rin, int rLen, Record* h_Rout, int* h_startPos, int numGroup )
{
	bool pass = true;
	qsort(h_Rin,rLen,sizeof(Record),compare);

	//test sort
	for( int i = 0; i < rLen; i++ )
	{
		if( (h_Rin[i].y != h_Rout[i].y) )
		{
			pass = false;
			printf( "sort error!\n" );
		}
		break;
	}

	//test group
	int count = 1;
	for( int i = 0; i < rLen - 1; i++ )
	{
		if( h_Rin[i].y != h_Rin[i+1].y )
		{
			count++;
		}
	}
	if( count != numGroup )
	{
		pass = false;
		printf( "count error! GPU, %d, CPU, %d\n", numGroup, count );
	}
	int* startPos = (int*)malloc( sizeof(int)*count );
	int j = 1;
	for( int i = 0; i < rLen - 1; i++ )
	{
		if( h_Rin[i].y != h_Rin[i+1].y )
		{
			startPos[j] = i + 1;
			j++;
		}
	}
	startPos[0] = 0;
	for( int idx = 0; idx < count; idx++ )
	{
		if( h_startPos[idx] != startPos[idx] )
		{
			pass = false;
			printf( "start position error!, GPU position: %d, CPU position: %d\n", h_startPos[idx], startPos[idx] );
			break;
		}
	}

	if( pass == true )
	{
		printf( "GroupBy Test passed!\n" );
	}
	else
	{
		printf( "GroupBy Test failed!\n" );
	}
}
void validateFilter( Record* h_Rin, int beginPos, int rLen, 
					Record* h_Rout, int outSize, int smallKey, int largeKey)
{
	bool passed = true;
	
	//int timer=DLL_genTimer(0);
	//startTimer( &timer );
	int count = 0;
	for( int i = 0; i < rLen; i++ )
	{
		//the filter condition
		int idx = beginPos + i;
		if( ( h_Rin[idx].y >= smallKey ) && ( h_Rin[idx].y <= largeKey ) )
		{
			count++;
		}
	}

	if( count != outSize )
	{
		printf( "!!!filter error: the number error, GPU, %d, CPU, %d\n", outSize, count );
		passed = false;
		exit(0);
	}

	Record* v_Rout = (Record*)malloc( sizeof(Record)*outSize );
	int j = 0;
	for( int i = 0; i < rLen; i++ )
	{
		//the filter condition
		int idx = beginPos + i;
		if( ( h_Rin[idx].y >= smallKey ) && ( h_Rin[idx].y <= largeKey ) )
		{
			v_Rout[j] = h_Rin[beginPos+i];
			j++;
		}
	}
	//endTimer( "cpu timer", &timer );

	for( int i = 0; i < outSize; i++ )
	{
		if( (v_Rout[i].x != h_Rout[i].x) || (v_Rout[i].y != h_Rout[i].y) )
		{
			printf( "!!! filter error\n" );
			passed = false;
			exit(0);
		}
	}

	if( passed )
	{
		//printf( "filter passed\n" );
	}
}

void validateReduce( Record * h_Rin, int rLen, unsigned int gpuResult, int OPERATOR )
{
	int timer=DLL_genTimer(0);

	if( OPERATOR == REDUCE_SUM )
	{
		unsigned int cpuSum = 0;

		//startTimer( &timer );
		for( int i = 0; i < rLen; i++ )
		{
			//printf("Host value :%d",h_Rin[i].y);
			if(h_Rin[i].y<0)
			{
				printf("WTF!!");
			}
			cpuSum += h_Rin[i].y;
		}
		endTimer( "cpu sum", &timer );

		if( gpuResult == cpuSum )
		{
			printf( "Test Passed: gpuSum = %u, cpuSum = %u\n", gpuResult, cpuSum );
		}
		else
		{
			printf( "!!!Test Failed: gpuSum = %u, cpuSum = %u\n", gpuResult, cpuSum );
		}
	}
	else if ( OPERATOR == REDUCE_AVERAGE )
	{
		unsigned int cpuAvg = 0;

		//startTimer( &timer );
		for( int i = 0; i < rLen; i++ )
		{
			cpuAvg += h_Rin[i].y;
		}
		cpuAvg = cpuAvg/rLen;
		endTimer( "cpu sum", &timer );

		if( gpuResult == cpuAvg )
		{
			printf( "Test Passed: gpuAvg = %u, cpuAvg = %u\n", gpuResult, cpuAvg );
		}
		else
		{
			printf( "!!!Test Failed: gpuAvg = %u, cpuAvg = %u\n", gpuResult, cpuAvg );
		}
	}
	else if( OPERATOR == REDUCE_MAX )
	{
		int cpuMax = h_Rin[0].y;		

		//startTimer( &timer );
		for( int i = 1; i < rLen; i++ )
		{
			if( h_Rin[i].y > cpuMax )
			{
				cpuMax = h_Rin[i].y;
			}
		}
		endTimer( "cpu max", &timer );

		if( gpuResult == cpuMax )
		{
			printf( "Test Passed: gpuMax = %u, cpuMax = %u\n", gpuResult, cpuMax );
		}
		else
		{
			printf( "!!!Test Failed: gpuMax = %u, cpuMax = %u\n", gpuResult, cpuMax );
		}
	}
	else if( OPERATOR == REDUCE_MIN )
	{
		int cpuMin = h_Rin[0].y;		

		//startTimer( &timer );
		for( int i = 1; i < rLen; i++ )
		{
			if( h_Rin[i].y < cpuMin )
			{
				cpuMin = h_Rin[i].y;
			}
		}
		endTimer( "cpu min", &timer );

		if( gpuResult == cpuMin )
		{
			printf( "Test Passed: gpuMin = %u, cpuMin = %u\n", gpuResult, cpuMin );
		}
		else
		{
			printf( "!!!Test Failed: gpuMin = %u, cpuMin = %u\n", gpuResult, cpuMin );
		}
	}
}

void validateSort(Record * h_Rin, int rLen)
{
	int i=0;
	bool passed=true;
	for(i=1;i<rLen;i++)
	{
		if(h_Rin[i].y<h_Rin[i-1].y)
		{
			printf("!!!error in sorting: %d, %d, %d, %d\n", i-1, h_Rin[i-1].y, i,h_Rin[i].y);
			passed=false;
			return;
		}
	}
	if(passed)
		printf("sorting passed\n");
}


/*void gpuValidateSort(Record *d_R, int rLen)
{
	int i=0;
	Record * h_Rin;
	HOST_MALLOC((void**)&R, rLen*sizeof(Record));
	//cl_readbuffer(R, d_R, rLen*sizeof(Record),CPU_GPU);
	bool passed=true;
	for(i=1;i<rLen;i++)
	{
		if(h_Rin[i].y<R[i-1].y)
		{
			printf("!!!!!!!! error in sorting: %d, %d, %d, %d\n", i-1, R[i-1].y, i,h_Rin[i].y);
			passed=false;
			return;
		}
	}
	if(passed)
		printf("sorting passed\n");
	HOST_FREE(R);
}*/
void validateSplit(Record * h_Rin, int rLen, int numPart)
{
	int i=0;
	bool passed=true;
	for(i=1;i<rLen;i++)
	{
		//printf("left is %d, right is %d\n", h_Rin[i].y,h_Rin[i-1].y);
		//printf("left is %d, right is %d\n", h_Rin[i].y%numPart,h_Rin[i-1].y%numPart);
		if((h_Rin[i].y%numPart)<(h_Rin[i-1].y%numPart))
		{
			printf("error in partition: %d, %d, %d, %d\n", i-1, h_Rin[i-1].y, i,h_Rin[i].y);
			passed=false;
			//break;
		}
	}
	if(passed)
		printf("\npartition passed\n");
}