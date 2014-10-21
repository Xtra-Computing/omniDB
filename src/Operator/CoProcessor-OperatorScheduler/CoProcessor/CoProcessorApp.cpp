#include "CPU_Dll.h"
#include "SingularThreadOp.h"
#include "CoProcessorTest.h"
#include <iostream>
#include "HandShaking.h"
using namespace std;
double evalautedQuery=0;
double Query_UpCPUBurden=0;
double Query_LoCPUBurden=0;
double Query_UpGPUBurden=0;
double Query_LoGPUBurden=0;
double Query_LothresholdForGPUApp=0;
double Query_LothresholdForCPUApp=0;
double Query_SpeedupGPUOverCPU[12];
double RunInCPU[12];
double RunInGPU[12];

double Query_CPUBurden=0;
double Query_GPUBurden=0;
CRITICAL_SECTION OP_GPUBurdenCS;
CRITICAL_SECTION OP_CPUBurdenCS;
CRITICAL_SECTION preEMCS;


int numQueries=30;//->corresponding to numOfThread for K_schedule
int numThread=4;//this is fixed to 1 for Q and O schedule
int OP_rLen=2*1024*1024;
FILE *OP_ofp;
void usage(int argc, char **argv) {
	fprintf(stderr, "Usage: %s <total amount of querys><total number of threads>\n", argv[0]);
	fprintf(stderr, "\t<total amount of querys>		  	- side size of matrix n (positive integer)\n");
	fprintf(stderr, "\t<total number of threads>			- number of thread per block(1 - 32)\n");
//	exit(1);
	fprintf(stdout,"Use default value: %s <30> <4>\n",argv[0]);
}
int main(int argc, char **argv)
{
	if(argc!=3){
		usage( argc, argv);
	}else{
		numQueries=atoi(argv[1]);
		numThread=atoi(argv[2]);
	}
	EngineStart(0,0);
	printf("Now start handshaking, please wait!\n");
	HandShake();
	printf("handshaking finished!\n\n\n");
	restore();
	InitializeCriticalSection(&(OP_GPUBurdenCS));
	InitializeCriticalSection(&(OP_CPUBurdenCS));
	InitializeCriticalSection(&(preEMCS));
	int choice;
	QUERY_TYPE qt;
	initDB2("RS.conf",TEST_MAX);
	QUERY_TYPE qT1=Q_RANGE_SELECTION;
	QUERY_TYPE qT2=Q_HJ;

	testQueryProcessor(qT1,qT2,numQueries,numThread);	
	EngineStop();

	DeleteCriticalSection(&(OP_GPUBurdenCS));
	DeleteCriticalSection(&(OP_CPUBurdenCS));
	DeleteCriticalSection(&(preEMCS));
	return 0;
}