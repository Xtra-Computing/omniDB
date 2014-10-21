// common SDK header for standard utilities and system libs 
#include "common.h"
#include "testMap.h"
#include "testAggAfterGB.h"
#include "testFilter.h"
#include "testGroupBy.h"
#include "testProjection.h"
#include "testReduce.h"
#include "testRIDList.h"
#include "testScan.h"
#include "testScatter.h"
#include "testSort.h"
#include "testSplit.h"
#include "Helper.h"
#include "MyThreadPoolCop.h"
#include "KernelScheduler.h"
#include "Handshake.h"
#include "MidNumber.h"
#include "testJoin.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include "OpenCL_DLL.h"
#define NUM_QUERY_TEMPLATE 3
// OpenCL Vars---------0 for CPU, 1 for GPU
cl_context Context;       // OpenCL context shared by CPU and GPU
cl_command_queue CommandQueue[2];// OpenCL command que
cl_platform_id Platform[2];      // OpenCL platform
cl_device_id Device[2];          // OpenCL device
cl_program Program;           // OpenCL program
cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
cl_device_id allDevices[10];
cl_ulong totalGlobalMemory[2];      /**< Max global memory allowed */
cl_ulong usedtotalGlobalMemory[2];      /**< Max global memory used */
CRITICAL_SECTION CPUBurdenCS;
CRITICAL_SECTION GPUBurdenCS;

CRITICAL_SECTION schedulerflag;
CRITICAL_SECTION deschedulerflag;
//cl_kernel Kernel[2];             // OpenCL kernel---------------->should been cancelled after all method update.
/////////////////////////////////////////////////////////////////////////////////////////////////
double AddGPUBurden_Copy;
double AddCPUBurden_Copy;
double AddGPUBurden_Read;
double AddCPUBurden_Read;
double AddGPUBurden_Write;
double AddCPUBurden_Write;
double AddGPUBurden[60];
double AddCPUBurden[60];
double speedupGPUoverCPU[60+3];
double  LothresholdForGPUApp;
double  LothresholdForCPUApp;
cl_mem D1;
cl_mem D2;
cl_mem D3;
cl_mem D4;
cl_mem D5;//int*
cl_mem D6;//int*
cl_mem D7;//int*
void *H1;
void *H2;
void *H3;
void *H4;
void *H5;//int*
void *H6;//int*
int from;
int to;


double LoGPUBurden;
double LoCPUBurden;
double UpGPUBurden;
double UpCPUBurden;

int scheduler_index=0;
int descheduler_index=0;
int cpuburden_index=0;
int gpuburden_index=0;
float cpuburden[10000000];
float gpuburden[10000000];
double GPUBurden;
double CPUBurden;
int TestTime=1;

int kernelcase=0;
int copycase=0;

//#define debugTony
int rLen=1024*1024*2;
int pLen=0.01*rLen;
int numthread=1;
int choice=1;
int runTime=1;
FILE *ofp;
HANDLE h_thread;
HANDLE h_thread1;
HANDLE h_thread2;
HANDLE h_thread3;
HANDLE h_thread4;
int global_KernelSchedule=0;
using namespace std;
void Cleanup (int iExitCode);
char* dir="";
void testJoin(int argc, char ** argv)
{
	int rLen=1024*1024;
	int choice=0;
			switch(choice)
			{
				case 0: {
						testINLJ(rLen, rLen*2);
						break;
						}
				case 1:testSMJ(rLen,rLen);break;
				case 2:{
						testHJ(rLen, rLen);
						break;
					   }
				case 3:{
						testNINLJ(rLen, rLen);
						
						break;
					   }
				case 4:testMJ(rLen,rLen);
					break;
			}

}
void testAllPrimitive(int argc, char** argv)
{
	int i = 0;
        for (i = 0; i < argc; i++) {
            printf("%s ", argv[i]);
        }
        printf("\n");
        int choice=7;
               switch (choice) {
                    case 0: {
							testMapImpl(rLen, 256, 1024);
							break;   
							}
                    case 1:{
							testScanImpl(rLen);
							break;   
							}
                    case 2:{
							testFilterImpl(rLen, 256, 128);
							break;   
							}
                    case 3:
							testGatherImpl(rLen, 256, 128);                   
							break;
                    case 4:
							testScatterImpl(rLen, 256, 128);
							break;
                    case 5:
							testSplitImpl(rLen, 8, 256, 128);
                        break;
                    case 6:
							 testSortImpl(rLen, 256, 4);
                        break;
                    case 7:
                        //			testGroupByImpl(rLen, 256, 1024,0);
                        testGroupByImpl(rLen, 256, 1024);
                    case 8:
                        testAggAfterGroupByImpl(rLen, REDUCE_SUM, 256, 1024);
                        testAggAfterGroupByImpl(rLen, REDUCE_SUM, 256, 1024);
						testAggAfterGroupByImpl(rLen, REDUCE_MAX, 256, 1024 );
                        testAggAfterGroupByImpl(rLen, REDUCE_MAX, 256, 1024);
						testAggAfterGroupByImpl(rLen, REDUCE_MIN, 256, 1024);
                        testAggAfterGroupByImpl(rLen, REDUCE_MIN, 256, 1024);
						testAggAfterGroupByImpl(rLen, REDUCE_AVERAGE, 256, 1024);
                        testAggAfterGroupByImpl(rLen, REDUCE_AVERAGE, 256, 1024);
                        break;
                    case 12:
                        testProjection(rLen, rLen * 0.01, 256, 64);
                        break;
                    case 13:
                        testReduceImpl(rLen, REDUCE_SUM, 256, 1024);
                        break;
                    case 14:
                        testReduceImpl(rLen, REDUCE_MAX, 256, 1024);
                        break;
                    case 15:
                        testReduceImpl(rLen, REDUCE_MIN, 256, 1024);
                        break;
                    case 16:
                        testReduceImpl(rLen, REDUCE_AVERAGE, 256, 1024);
                        break;
                    case 17:
                        break;
        }
}
void inital(){
	int memSize=sizeof(Record)*rLen;
	int intmemSize=sizeof(int)*rLen;
	CL_MALLOC( &D1, memSize);
	CL_MALLOC( &D2, memSize);
	CL_MALLOC( &D3, memSize);
	CL_MALLOC( &D4, memSize);
	CL_MALLOC( &D5, intmemSize);
	CL_MALLOC( &D6, intmemSize);
	CL_MALLOC( &D7, intmemSize);
	HOST_MALLOC(H1, memSize);
	HOST_MALLOC(H2, memSize);
	HOST_MALLOC(H3, memSize);
	HOST_MALLOC(H4, memSize);
	HOST_MALLOC(H5, intmemSize);
	HOST_MALLOC(H6, intmemSize);
	generateRand((Record *)H1, TEST_MAX, rLen, 0 );
	cl_writebuffer( D1, H1, sizeof(Record)*rLen,0);
	generateRand((Record *)H2, TEST_MAX, rLen, 0 );
	cl_writebuffer( D2, H2, sizeof(Record)*rLen,0);
	generateRand((Record *)H3, TEST_MAX, rLen, 0 );
	cl_writebuffer( D3, H3, sizeof(Record)*rLen,0);
	generateRand((Record *)H4, TEST_MAX, rLen, 0 );
	cl_writebuffer( D4, H4, sizeof(Record)*rLen,0);
	fillint((int*)H5,rLen);
	cl_writebuffer( D5, H5, rLen,0);
	fillint((int*)H6,rLen);
	cl_writebuffer( D6, H6, rLen,0);
}
void initalSort(){
	int memSize=sizeof(Record)*rLen;
	int intmemSize=sizeof(int)*rLen;
	CL_MALLOC( &D1, memSize);
	CL_MALLOC( &D2, memSize);
	CL_MALLOC( &D3, memSize);
	CL_MALLOC( &D4, memSize);
	CL_MALLOC( &D5, intmemSize);
	CL_MALLOC( &D6, intmemSize);
	CL_MALLOC( &D7, intmemSize);
	HOST_MALLOC(H1, memSize);
	HOST_MALLOC(H2, memSize);
	HOST_MALLOC(H3, memSize);
	HOST_MALLOC(H4, memSize);
	HOST_MALLOC(H5, intmemSize);
	HOST_MALLOC(H6, intmemSize);
	generateSort((Record *)H1, TEST_MAX, rLen, 0 );
	cl_writebuffer( D1, H1, sizeof(Record)*rLen,0);
	generateRand((Record *)H2, TEST_MAX, rLen, 0 );
	cl_writebuffer( D2, H2, sizeof(Record)*rLen,0);
	generateSort((Record *)H3, TEST_MAX, rLen, 0 );
	cl_writebuffer( D3, H3, sizeof(Record)*rLen,0);
	generateSort((Record *)H4, TEST_MAX, rLen, 0 );
	cl_writebuffer( D4, H4, sizeof(Record)*rLen,0);
	fillint((int*)H5,rLen);
	cl_writebuffer( D5, H5, rLen,0);
	fillint((int*)H6,rLen);
	cl_writebuffer( D6, H6, rLen,0);
}
void AnyHowFree(){
	CL_FREE(D1);
	CL_FREE(D2);
	CL_FREE(D3);
	CL_FREE(D4);
	CL_FREE(D5);
	CL_FREE(D6);
	CL_FREE(D7);
	HOST_FREE(H1);
	HOST_FREE(H2);
	HOST_FREE(H3);
	HOST_FREE(H4);
	HOST_FREE(H5);
	HOST_FREE(H6);
}
void handShaking()
{
	char outputFilename[50];
	sprintf(outputFilename, "KernelTimeSpecification.list");
	ofp = fopen(outputFilename, "w");
	int CPU_GPU=0;
	int kid;
	cl_kernel testkernel;
		for(kid=0;kid<51;kid++){
			for(CPU_GPU=0;CPU_GPU<2;CPU_GPU++){			
			printf("CPU_GPU:%d, KID:%d\n",CPU_GPU,kid);
			switch(kid){
				case 0:{/*projection_map_kernel*/
						inital();
					 	Project_map_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 1:{/*getResult_kernel*/
						inital();
					 	getResult_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 2:{
						break;
					   }
				case 3:{
						break;
					   }
				case 4:{
						break;
					   }
				case 5:{/*projection_map_kernel*/
		
						break;
					   }
				case 6:{/*perscanFirstPass_kernel*/
						inital();
					 	perscanFirstPass_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 7:{/*perscan_kernel*/
						inital();
					 	perscan_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 8:{/*projection_map_kernel*/
		
						break;
					   }
				case 9:{/*projection_map_kernel*/
		
						break;
					   }
				case 10:{/*projection_map_kernel*/
				
						break;
					   }
				case 11:{/*projection_map_kernel*/
		
						break;
					   }
				case 12:{/*projection_map_kernel*/
				
						break;
					   }
				case 13:{/*projection_map_kernel*/

						break;
					   }
				case 14:{/*mapImpl_kernel_handshake*/
						inital();
						mapImpl_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 15:{/*projection_map_kernel*/
			
						break;
					   }
				case 16:{/*memset_int_kernel*/
						inital();
						 memset_int_kernel_handshake(CPU_GPU,&testkernel);
						 AnyHowFree();
						break;
					   }
				case 17:{/*blockAddition_kernel*/
						inital();
						blockAddition_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 18:{/*prefixSum_kernel*/
						inital();
						prefixSum_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 19:{/*ScanLargeArrays_kernel*/
						inital();
						ScanLargeArrays_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 20:{/*filterImpl_map_kernel*/
						inital();
						filterImpl_map_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 21:{/*projection_map_kernel*/
				
						break;
					   }
				case 22:{/*projection_map_kernel*/
					
						break;
					   }
				case 23:{/*projection_map_kernel*/
						inital();
						filterImpl_write_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 24:{/*optScatter_kernel*/
						inital();
					 	optScatter_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 25:{/*optGather_kernel*/
						inital();
					 	optGather_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 26:{/*partition_kernel*/
					 	inital();
					 	partition_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 27:{/*mapPart_kernel*/
						inital();
					 	mapPart_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 28:{/*countHist_kernel*/
						inital();
					 	countHist_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 29:{/*writeHist_kernel*/
						inital();
					 	writeHist_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 30:{/*getBound_kernel*/
						inital();
					 	getBound_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 31:{/*BitonicSort_kernel*/
						inital();
					 	BitonicSort_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 32:{/*projection_map_kernel*/
					
						break;
					   }
				case 33:{/*projection_map_kernel*/
					 	
						break;
					   }
				case 34:{/*projection_map_kernel*/
					 	
						break;
					   }
				case 35:{/*projection_map_kernel*/
					 
						break;
					   }
				case 36:{/*projection_map_kernel*/
					 
						break;
					   }
				case 37:{/*projection_map_kernel*/
					 	
						break;
					   }
				case 38:{/*gpuNLJ_kernel*/
						inital();
						gpuNLJ_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 39:{/*nlj_write_kernel*/
						inital();
						 nlj_write_kernel_handshake(CPU_GPU,&testkernel);
						 AnyHowFree();
						break;
					   }
				case 40:{/*gSearchTree_kernel*/
						initalSort();
						gSearchTree_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 41:{/*gIndexJoin_kernel*/
						initalSort();
						 gIndexJoin_kernel_handshake(CPU_GPU,&testkernel);
						 AnyHowFree();
						break;
					   }
				case 42:{/*gJoinWithWrite_kernel*/
						initalSort();
					 	gJoinWithWrite_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 43:{/*gCreateIndex_kernel*/
						initalSort();
					 	gCreateIndex_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 44:{/*quanMap_kernel*/
						initalSort();
					 	quanMap_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
					
						break;
					   }
				case 45:{/*gSearchTree_usingKeys_kernel*/
						 initalSort();
					 	gSearchTree_usingKeys_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 46:{/*joinMBCount_kernel*/
						 inital();
					 	joinMBCount_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 47:{/*joinMBWrite_kernel*/
					 	initalSort();
					 	joinMBWrite_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 48:{/*projection_map_kernel*/
					
						break;
					   }
				case 49:{/*build_kernel*/
					 	inital();
					 	build_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
				case 50:{/*probe_kernel*/
						inital();
					 	probe_kernel_handshake(CPU_GPU,&testkernel);
						AnyHowFree();
						break;
					   }
			}//end of switch
		}//end of inner for loop
	}//end of outer for loop
	int b;
	inital();
	for(b=0;b<3;b++){
		for(CPU_GPU=0;CPU_GPU<2;CPU_GPU++){
			switch(b){
				case 0:{/*AddBurden_Copy*/
					 	if(CPU_GPU)
							AddGPUBurden_Copy_handshake();
						else
							AddCPUBurden_Copy_handshake();
						break;
					   }
				case 1:{/*AddBurden_Read*/
					 	if(CPU_GPU)
							AddGPUBurden_Read_handshake();
						else
							AddCPUBurden_Read_handshake();
						break;
					   }
				case 2:{/*AddBurden_Write*/
					 	if(CPU_GPU)
							AddGPUBurden_Write_handshake();
						else
							AddCPUBurden_Write_handshake();
						break;
					   }
			}
		}
	}
	AnyHowFree();
	int i;
	int counter=0;
	double *sortSpeedUp;
	double *sortCPUBurden;
	double *sortGPUBurden;
	sortSpeedUp=(double *)malloc(sizeof(double)*60 );
	sortCPUBurden=(double *)malloc(sizeof(double)*60 );
	sortGPUBurden=(double *)malloc(sizeof(double)*60 );
	/*SORT for KERNEL*/
	for(i=0;i<60;i++){
		if(AddGPUBurden[i]!=0){
			fprintf(ofp,"kc %d  %lf\n",i, AddCPUBurden[i]);
			fprintf(ofp,"kg %d  %lf\n",i, AddGPUBurden[i]);
			speedupGPUoverCPU[i]=AddCPUBurden[i]/AddGPUBurden[i];
			sortSpeedUp[counter]=speedupGPUoverCPU[i];
			sortCPUBurden[counter]=AddCPUBurden[i];
			sortGPUBurden[counter]=AddGPUBurden[i];
			counter++;//indicate how many non-zero data.
		}
	}
	fprintf(ofp,"EN\n");
	choise(sortSpeedUp,counter);
	choise(sortCPUBurden,counter);
	choise(sortGPUBurden,counter);
	/*set speed threshold*/
	//LothresholdForGPUApp=_f(sortSpeedUp,0,counter-1,(int)(counter*20/58));//->set lothreshold to be the first 20 max,0-20 treat as GPU faster
	LothresholdForGPUApp=sortSpeedUp[(int)(counter*35/58+1)];
	printf("LothresholdForGPUApp is %lf\n",LothresholdForGPUApp);
	//LothresholdForCPUApp=_f(sortSpeedUp,0,counter-1,(int)(counter*25/58+1));//->set lothreshold to be the first 25 max,25-58 treat as CPU faster
	LothresholdForCPUApp=sortSpeedUp[(int)(counter*25/58)];
	printf("LothresholdForCPUApp is %lf\n",LothresholdForCPUApp);
	//20-25 as in middle.
	/*set GPUburden threshold*/
	//LoGPUBurden=_f(sortGPUBurden,0,counter-1,(int)(counter*40/58+1));//->set lothreshold as first 40 max.
	LoGPUBurden=(sortGPUBurden[0]+sortGPUBurden[(int)(counter*10/58)])/2;
	printf("LoGPUBurden is %lf\n",LoGPUBurden);
	//UpGPUBurden=_f(sortGPUBurden,0,counter-1,(int)(counter*10/58));//->set UPthreshold as first 10 max.
	UpGPUBurden=(sortGPUBurden[(int)(counter*35/58+1)]+sortGPUBurden[(int)(counter*50/58+1)])/2;
	printf("UpGPUBurden is %lf\n",UpGPUBurden);
	/*set CPUburden threshold*/
	LoCPUBurden=(sortCPUBurden[0]+sortCPUBurden[(int)(counter*10/58)])/2;
	printf("LoCPUBurden is %lf\n",LoCPUBurden);
	
	UpCPUBurden=(sortCPUBurden[(int)(counter*35/58+1)]+sortCPUBurden[(int)(counter*50/58+1)])/2;
	printf("UpCPUBurden is %lf\n",UpCPUBurden);
	int w;
	for(w=0;w<counter;w++){
		fprintf(ofp,"sortSpeedUp%d is %lf\n",w,sortSpeedUp[w]);
	}
	fprintf(ofp,"----------------------------------\n\n");
	for(w=0;w<counter;w++){
		fprintf(ofp,"sortCPUBurden%d is %lf\n",w,sortCPUBurden[w]);
	}
	fprintf(ofp,"----------------------------------\n\n");
	for(w=0;w<counter;w++){
		fprintf(ofp,"sortGPUBurden%d is %lf\n",w,sortGPUBurden[w]);
	}
	fprintf(ofp,"Final decision\n\n");
	fprintf(ofp,"LothresholdForGPUApp is %lf\n",LothresholdForGPUApp);
	fprintf(ofp,"LothresholdForCPUApp is %lf\n",LothresholdForCPUApp);
	fprintf(ofp,"LoGPUBurden is %lf\n",LoGPUBurden);
	fprintf(ofp,"UpGPUBurden is %lf\n",UpGPUBurden);
	fprintf(ofp,"LoCPUBurden is %lf\n",LoCPUBurden);
	fprintf(ofp,"UpCPUBurden is %lf\n",UpCPUBurden);
	fclose(ofp);
}


DWORD WINAPI burdenMeasure( LPVOID lpParam )
{
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
	while(1){
		Sleep(1);
		recordUpdate(GPUBurden,CPUBurden);
	}
}
DWORD WINAPI performanceTester( LPVOID lpParam ){
	threadPar* pData;
	pData = (threadPar*)lpParam;
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
	int pid=pData->threadid;
	int choice =11;
	        switch (choice) {
            case 0: {	
            }
			case 1:{

					testMapImpl(rLen, 256, 1024);
					break;
				   }
			case 2:{
					testScanImpl(rLen);
					break;
				   }
			case 3:{
					
					break;
				   }
			case 4:{
					testProjection(rLen, rLen * 0.01, 256, 64);
					break;
				   }
			case 5:{
					testINLJ(rLen, rLen * 2);
					break;
				   }
			case 6:{
					
					testHJ(rLen, rLen);
						
					break;
				   }
			case 7:{
					testSortImpl(rLen, 256, 4);
					testFilterImpl(rLen, 256, 128);
					testSMJ(256*1024,256*1024);		
					//testNINLJ(256*1024/numthread, 256*1024/numthread);
					//testMJ(256*1024,256*1024);
					break;
				   }
			case 8:{
					testReduceImpl(rLen, REDUCE_SUM, 256, 1024);
					break;
				   }
			case 9:{
					testReduceImpl(rLen, REDUCE_MAX, 256, 1024);
					break;
				   }
			case 10:{
					testReduceImpl(rLen, REDUCE_MIN, 256, 1024);
					break;
				   }
			case 11:{
					testSortImpl(rLen, 256, 4);
					testReduceImpl(rLen, REDUCE_MAX, 256, 1024);
					break;
				   }
			case 12:{
					testGatherImpl(rLen, 256, 128);    
					break;
					}
			case 13:{
					testScatterImpl(rLen, 256, 128);
					break;
					}
			case 14:{
					//testSplitImpl(rLen, 8, 256, 128);
					break;
					}
			case 15:{
					testSortImpl(rLen, 256, 4);
					break;
				}
			case 16:{
					//testGroupByImpl(rLen, 256, 64);
					break;
					}
			case 17:{
					//testAggAfterGroupByImpl(rLen,REDUCE_SUM,256,1024);
					break;
					}
	}
	return 0;
}
void restore()
{
int i;
for(i=0;i<cpuburden_index;i++)
	cpuburden[i]=0;
for(i=0;i<gpuburden_index;i++)
	gpuburden[i]=0;

scheduler_index=0;
descheduler_index=0;
cpuburden_index=0;
gpuburden_index=0;
}
void readFromFile(){
  string line;
  double result;
  int kid;
  ifstream myfile ("KernelTimeSpecification.list");
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);
      cout << line << endl;
	  string temp=line.substr(0,2);
	  char * cstr = new char [100];
	  strcpy (cstr, temp.c_str());
	  if(!strcmp(cstr,"cr")){
				result= atof(line.substr(3,100).c_str());
				//cout<<result;
				AddCPUBurden_Read=result;
		  }
	  else if(!strcmp(cstr,"gr")){
			    result= atof(line.substr(3,100).c_str());
				//cout<<result;
				AddGPUBurden_Read=result;
	  }
	  else if(!strcmp(cstr,"cw")){
				result= atof(line.substr(3,100).c_str());
				//cout<<result;
				AddCPUBurden_Write=result;
	  }
	  else if(!strcmp(cstr,"gw")){
				 result= atof(line.substr(3,100).c_str());
				//cout<<result;
				AddGPUBurden_Write=result;
	  }
	  else if(!strcmp(cstr,"cc")){
				result= atof(line.substr(3,100).c_str());
				//cout<<result;
				AddCPUBurden_Copy=result;
	  }
	  else if(!strcmp(cstr,"gc")){
				result= atof(line.substr(3,100).c_str());
				//cout<<result;
				AddGPUBurden_Copy=result;
	  }	
	  else if(!strcmp(cstr,"kc")){
			  kid= atoi(line.substr(3,2).c_str());
			  result=atof(line.substr(5,100).c_str());
			  AddCPUBurden[kid]=result;
	  }	
	  else if(!strcmp(cstr,"kg")){
			   kid= atoi(line.substr(3,2).c_str());
			   result=atof(line.substr(5,100).c_str());
			   AddGPUBurden[kid]=result;
	  }	  
	  else if(!strcmp(cstr,"EN")){
			int i;
			int counter=0;
			double *sortSpeedUp;
			double *sortCPUBurden;
			double *sortGPUBurden;
			sortSpeedUp=(double *)malloc(sizeof(double)*60 );
			sortCPUBurden=(double *)malloc(sizeof(double)*60 );
			sortGPUBurden=(double *)malloc(sizeof(double)*60 );
			/*SORT for KERNEL*/
				for(i=0;i<60;i++){
					if(AddGPUBurden[i]!=0){			
						speedupGPUoverCPU[i]=AddCPUBurden[i]/AddGPUBurden[i];
						sortSpeedUp[counter]=speedupGPUoverCPU[i];
						sortCPUBurden[counter]=AddCPUBurden[i];
						sortGPUBurden[counter]=AddGPUBurden[i];
						counter++;//indicate how many non-zero data.
					}
				}
				choise(sortSpeedUp,counter);
				choise(sortCPUBurden,counter);
				choise(sortGPUBurden,counter);
				/*set speed threshold*/
				//LothresholdForGPUApp=_f(sortSpeedUp,0,counter-1,(int)(counter*20/58));//->set lothreshold to be the first 20 max,0-20 treat as GPU faster
				LothresholdForGPUApp=sortSpeedUp[(int)(counter*35/58+1)];
				printf("LothresholdForGPUApp is %lf\n",LothresholdForGPUApp);
				//LothresholdForCPUApp=_f(sortSpeedUp,0,counter-1,(int)(counter*25/58+1));//->set lothreshold to be the first 25 max,25-58 treat as CPU faster
				LothresholdForCPUApp=sortSpeedUp[(int)(counter*25/58)];
				printf("LothresholdForCPUApp is %lf\n",LothresholdForCPUApp);
				//20-25 as in middle.
				/*set GPUburden threshold*/
				//LoGPUBurden=_f(sortGPUBurden,0,counter-1,(int)(counter*40/58+1));//->set lothreshold as first 40 max.
				LoGPUBurden=(sortGPUBurden[0]+sortGPUBurden[(int)(counter*10/58)])/2;
				printf("LoGPUBurden is %lf\n",LoGPUBurden);
				//UpGPUBurden=_f(sortGPUBurden,0,counter-1,(int)(counter*10/58));//->set UPthreshold as first 10 max.
				//UpGPUBurden=(sortGPUBurden[(int)(counter*35/58+1)]+sortGPUBurden[(int)(counter*40/58+1)])/2;
				UpGPUBurden=LoGPUBurden*2;
				printf("UpGPUBurden is %lf\n",UpGPUBurden);
				/*set CPUburden threshold*/
				LoCPUBurden=(sortCPUBurden[0]+sortCPUBurden[(int)(counter*10/58)])/2;
				printf("LoCPUBurden is %lf\n",LoCPUBurden);
				//UpCPUBurden=(sortCPUBurden[(int)(counter*35/58+1)]+sortCPUBurden[(int)(counter*50/58+1)])/2;
				UpCPUBurden=LoCPUBurden*2;
				printf("UpCPUBurden is %lf\n",UpCPUBurden);  
				myfile.close();
			  return;
	  }	 
	  }
    }
   
}
void EngineStart(bool handShake,int _KernelSchedule)
{	
		global_KernelSchedule=_KernelSchedule;
		InitializeCriticalSection(&(GPUBurdenCS));
		InitializeCriticalSection(&(CPUBurdenCS));
		InitializeCriticalSection(&(schedulerflag));
		InitializeCriticalSection(&(deschedulerflag));
		cl_init(CL_DEVICE_TYPE_CPU);
		cl_init(CL_DEVICE_TYPE_GPU);
		cl_init_common();
		
		cl_prepareProgram("primitive.cl",".");
		if(handShake){
		printf("Now start handShaking!Please wait!\n");
		handShaking();
		printf("HandShaking successfully done!\n");
		}else{
			printf("warning! hand shake skipped!\n");
			readFromFile();
		}
		h_thread=CreateThread(NULL,0,burdenMeasure,NULL,0,NULL);
		Sleep(0.2);
		h_thread1=CreateThread(NULL,0,burdenMeasure,NULL,0,NULL);
		Sleep(0.4);
		h_thread2=CreateThread(NULL,0,burdenMeasure,NULL,0,NULL);
		Sleep(0.6);
		h_thread3=CreateThread(NULL,0,burdenMeasure,NULL,0,NULL);
		Sleep(0.8);
		h_thread4=CreateThread(NULL,0,burdenMeasure,NULL,0,NULL);
}
void EngineStop()
{
	SuspendThread(h_thread);
	SuspendThread(h_thread1);
	SuspendThread(h_thread2);
	SuspendThread(h_thread3);
	SuspendThread(h_thread4);
	DeleteCriticalSection(&(GPUBurdenCS));
	DeleteCriticalSection(&(CPUBurdenCS));
	DeleteCriticalSection(&(schedulerflag));
	DeleteCriticalSection(&(deschedulerflag));
	char outputFilename[50];
	int i;
	sprintf(outputFilename, "./Output/ExpOut_cpuBurden.tony");
	ofp = fopen(outputFilename, "w");
	for(i=0;i<cpuburden_index;i++){
		fprintf(ofp,"%.5f\n",cpuburden[i]);
	}
	fclose(ofp);

	sprintf(outputFilename, "./Output/ExpOut_gpuBurden.tony");
	ofp = fopen(outputFilename, "w");
	for(i=0;i<gpuburden_index;i++){
		fprintf(ofp,"%.5f\n",gpuburden[i]);
	}
	fclose(ofp);
}
// Main function 
// *********************************************************************
int gmain(int argc, char **argv)
{			
		//readFromFile();
		EngineStart(0,1);
		//usedtotalGlobalMemory[0]=0;
		//usedtotalGlobalMemory[1]=0;
		//testAllPrimitive(argc,argv);
		//testJoin(argc,argv);
		
/*Performance Test*/
#if 0
		int TestCounter=0;
		for(TestCounter=0;TestCounter<TestTime;TestCounter++){
			printf("Test%d begin!\n",TestCounter);
			MyThreadPoolCop *pool=(MyThreadPoolCop*)malloc(sizeof(MyThreadPoolCop));
			threadPar** pData=(threadPar**)malloc(sizeof(threadPar*)*numthread);
			pool->create(numthread);
			int i;
			for( i=0; i<numthread; i++ )
			{
				// Allocate memory for thread data.
				pData[i] = (threadPar*) HeapAlloc(GetProcessHeap(),
						HEAP_ZERO_MEMORY, sizeof(threadPar));
				if( pData[i]  == NULL )
					ExitProcess(2);
				pData[i]->init(i);
				pool->assignParameter(i, pData[i]);
				pool->assignTask(i, performanceTester);
			}
			int type;
			for(type=0;type<1;type++){
			FILE *ofp;
			char outputFilename[50];
			sprintf(outputFilename, "testscript%dof%d.cpp", TestCounter,type);
			ofp = fopen(outputFilename, "w");

			if (ofp == NULL) {
			  fprintf(stderr, "Can't open output file %s!\n",
					  outputFilename);
				 exit(1);
			}				
			int test;
			
			double sum;
		
				   sum=0;
				int timer=DLL_genTimer(0);
				for(test=0;test<runTime;test++){
					printf("runTime is %d\n",test);
					DLL_getTimer(timer);
					pool->run();
					double t=DLL_getTimer(timer);									
					sum+=t;		
					
				}		
				
			
			//fprintf(ofp,"--------------------Result of Alltest for type of %d:----------------------\n",type);
			//fprintf(ofp,"with kernel schedule, run 10 Operator need in average :%lf \n",sum/runTime);
			//fprintf(ofp,"Test%d end! in %s\n",TestCounter,ctime (&rawtime));			
			fclose(ofp);
			}
			
			for(i=0;i<numthread;i++)
			{
				HeapFree(GetProcessHeap(),0, pData[i]);
			}
			pool->destory();
			
	}	
		EngineStop();
#endif
			
}
void Cleanup (int iExitCode)
{
    // Cleanup allocated objects
    cl_clean(iExitCode);   
    exit (iExitCode);
	kernel_clean();
}