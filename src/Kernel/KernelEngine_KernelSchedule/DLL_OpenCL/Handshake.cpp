#include "common.h"
#include "Helper.h"
#include "CSSTree.h"
#include "testSplit.h"
#include "testScan.h"
#include "testJoin.h"
extern cl_context Context;        // OpenCL context
extern double AddGPUBurden_Copy;//->initial in handshaking. fix rLen to 1024*1024
extern double AddCPUBurden_Copy;
extern double AddGPUBurden_Read;//->initial in handshaking. fix rLen to 1024*1024
extern double AddCPUBurden_Read;
extern double AddGPUBurden_Write;//->initial in handshaking. fix rLen to 1024*1024
extern double AddCPUBurden_Write;
extern double AddGPUBurden[60];//->initial in handshaking. fix rLen to 1024*1024
extern double AddCPUBurden[60];//->initial in handshaking. fix rLen to 1024*1024
extern double speedupGPUoverCPU[60+3];
extern double  LothresholdForGPUApp;
extern double  LothresholdForCPUApp;
extern double LoGPUBurden;
extern double LoCPUBurden;
extern double UpGPUBurden;
extern double UpCPUBurden;

extern cl_mem D1;
extern cl_mem D2;
extern cl_mem D3;
extern cl_mem D4;
extern cl_mem D5;//empty
extern cl_mem D6;//empty
extern cl_mem D7;//int*
extern void *H1;
extern void *H2;
extern void *H3;
extern void *H4;
extern void *H5;//empty
extern void *H6;//empty
extern int from;
extern int to;
extern int rLen;
extern int pLen;
//#define HandshakeDebug

double Count=1;
extern FILE *ofp;
void AddGPUBurden_Copy_handshake()
{
	double i;
	double sum=0;
	double scaler=1000;
	int timer = DLL_genTimer(0);
	for(i=0;i<Count;i++){
        DLL_getTimer(timer);
		cl_copyBuffer(D1, D2, rLen,1);
		 double t = DLL_getTimer(timer);
		 sum+=t;
#ifdef HandshakeDebug
        printf("AddGPUBurden_Copy invocatio overhead, %f\n", t);
#endif
	}
	 AddGPUBurden_Copy = sum/Count*scaler;
			 printf("sum is %lf\n, AddGPUBurden_Copy invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_Copy);
	fprintf(ofp,"gc %lf\n",AddGPUBurden_Copy);
}
void AddCPUBurden_Copy_handshake()
{
	double i;
	double sum=0;
	double scaler=1000;
	int timer = DLL_genTimer(1);
	for(i=0;i<Count;i++){

        DLL_getTimer(timer);
		cl_copyBuffer(D1, D2, rLen,0);
		 double t = DLL_getTimer(timer);
		 sum+=t;
#ifdef HandshakeDebug
        printf("AddCPUBurden_Copy invocatio overhead, %f\n", t);
#endif
	}
	 AddCPUBurden_Copy = sum/Count*scaler;
			 printf("sum is %lf\n, AddCPUBurden_Copy invocatio overhead in average in CPU, %lf\n", sum,AddCPUBurden_Copy);
	fprintf(ofp,"cc %lf\n",AddCPUBurden_Copy);
}
void AddGPUBurden_Read_handshake()
{
	double i;
	double sum=0;
	double scaler=1000;
	int timer = DLL_genTimer(2);
	for(i=0;i<Count;i++){
        DLL_getTimer(timer);
		cl_readbuffer(H1, D1, rLen,1);
		 double t = DLL_getTimer(timer);
		 sum+=t;
#ifdef HandshakeDebug
        printf("AddGPUBurden_Read invocatio overhead, %f\n", t);
#endif
	}
	 AddGPUBurden_Read = sum/Count*scaler;
			 printf("sum is %lf\n, AddGPUBurden_Read invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_Read);
	fprintf(ofp,"gr %lf\n",AddGPUBurden_Read);
}
void AddCPUBurden_Read_handshake()
{
	double i;
	double sum=0;
	double scaler=1000;
	int timer = DLL_genTimer(3);
	for(i=0;i<Count;i++){
        DLL_getTimer(timer);
		cl_readbuffer(H1, D1, rLen,0);
		 double t = DLL_getTimer(timer);
		 sum+=t;
#ifdef HandshakeDebug
        printf("AddCPUBurden_Read invocatio overhead, %f\n", t);
#endif
	}
	 AddCPUBurden_Read = sum/Count*scaler;
			 printf("sum is %lf\n, AddCPUBurden_Read invocatio overhead in average in CPU, %lf\n", sum,AddCPUBurden_Read);
	fprintf(ofp,"cr %lf\n",AddCPUBurden_Read);
}

void AddGPUBurden_Write_handshake()
{
	double i;
	double sum=0;
	double scaler=1000;
	int timer = DLL_genTimer(4);
	for(i=0;i<Count;i++){		
        DLL_getTimer(timer);
		cl_writebuffer(D1, H1, rLen,1);
		 double t = DLL_getTimer(timer);
		 sum+=t;
#ifdef HandshakeDebug
        printf("AddGPUBurden_Write invocatio overhead, %f\n", t);
#endif
	}
	 AddGPUBurden_Write = sum/Count*scaler;
			 printf("sum is %lf\n, AddGPUBurden_Write invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_Write);
	fprintf(ofp,"gw %lf\n",AddGPUBurden_Write);
}
void AddCPUBurden_Write_handshake()
{
	double i;
	double sum=0;
	double scaler=1000;
	int timer = DLL_genTimer(5);
	for(i=0;i<Count;i++){		
        DLL_getTimer(timer);
		cl_writebuffer(D1, H1, rLen,0);
		 double t = DLL_getTimer(timer);
		 sum+=t;
#ifdef HandshakeDebug
        printf("AddCPUBurden_Write invocatio overhead, %f\n", t);
#endif
	}
	 AddCPUBurden_Write = sum/Count*scaler;
			 printf("sum is %lf\n, AddCPUBurden_Write invocatio overhead in average in CPU, %lf\n", sum,AddCPUBurden_Write);
	fprintf(ofp,"cw %lf\n",AddCPUBurden_Write);
}
void Project_map_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=0;
	printf("Kid%d",kid);
	int timer = DLL_genTimer(kid);
	for(i=0;i<Count;i++){
		
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
        cl_getKernel("projection_map_kernel", _HandShakeKernel);
        // Set the Argument values
        cl_int ciErr1 = clSetKernelArg(( * _HandShakeKernel), 0, sizeof(cl_mem), (void *) & D1);
        ciErr1 |= clSetKernelArg(( * _HandShakeKernel), 1, sizeof(cl_int), (void *) & pLen);
        ciErr1 |= clSetKernelArg(( * _HandShakeKernel), 2, sizeof(cl_mem), (void *) & D2);
        ciErr1 |= clSetKernelArg(( * _HandShakeKernel), 3, sizeof(cl_mem), (void *) & D3);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("projection_map_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, projection_map_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, projection_map_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void getResult_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=1;
	printf("Kid%d",kid);
	int OPERATOR=0;
	size_t numThreadsPerBlock_x = 256;
    size_t globalWorkingSetSize = 32 * 64;
	int timer = DLL_genTimer(kid);
	for(i=0;i<Count;i++){
        DLL_getTimer(timer);
		cl_getKernel("getResult_kernel",_HandShakeKernel);
		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&OPERATOR);    
		 cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("getResult_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, getResult_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, getResult_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void copyLastElement_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=5;
	printf("Kid%d",kid);
	int base=0;
	int offset=0;
	int timer = DLL_genTimer(kid);
	for(i=0;i<Count;i++){
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
       	cl_getKernel("copyLastElement_kernel",_HandShakeKernel);
		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D2);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_mem), (void*)&D1);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&base);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&offset);   

        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("copyLastElement_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, copyLastElement_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, copyLastElement_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void perscanFirstPass_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=6;
	printf("Kid%d",kid);
	// Set the Argument values
	int sharedMemSize=3000;
	int bit_isFull=1;
	int numElementsPerBlock=512;
	int base=0;
    int OPERATOR=0;
	int d_odataOffset=0;
	// Set the Argument values
	CL_MALLOC( &D4, sizeof(int)*rLen );
	size_t numThreadsPerBlock_x = 256;
    size_t globalWorkingSetSize = 32 * 64;
	int timer = DLL_genTimer(kid);
	for(i=0;i<Count;i++){
        DLL_getTimer(timer);
		cl_getKernel("perscanFirstPass_kernel",_HandShakeKernel);
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D4);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D3);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&numElementsPerBlock);   
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_int), (void*)&bit_isFull);   
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_int), (void*)&base);   
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 7, sizeof(cl_int), (void*)&d_odataOffset);   
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 8, sizeof(cl_int), (void*)&OPERATOR);   
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 9, sizeof(cl_int), (void*)&sharedMemSize);   
		cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("perscanFirstPass_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, perscanFirstPass_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, perscanFirstPass_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void perscan_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=7;
	printf("Kid%d",kid);
	// Set the Argument values
	int sharedMemSize=3000;
	int bit_isFull=1;
	int numElementsPerBlock=512;
	int base=0;
    int OPERATOR=0;
	int d_odataOffset=0;
	// Set the Argument values
	size_t numThreadsPerBlock_x = 256;
    size_t globalWorkingSetSize = 32 * 64;
	int timer = DLL_genTimer(kid);
	for(i=0;i<Count;i++){
        DLL_getTimer(timer);
		cl_getKernel("perscan_kernel",_HandShakeKernel);
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_mem), (void*)&D3);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&numElementsPerBlock);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&bit_isFull);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_int), (void*)&base);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_int), (void*)&d_odataOffset);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 7, sizeof(cl_int), (void*)&OPERATOR);  
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 8, sizeof(cl_int), (void*)&sharedMemSize);   
		cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("perscan_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, perscan_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, perscan_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void mapImpl_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=14;
	printf("Kid%d",kid);
	int numRun=8;
	int runSize=rLen/numRun;	
	from=0;
	to=runSize;
	int smallKey = rand()%100;
	int largeKey = smallKey;
	int beginPos = 0;
	int value=0x7f;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("mapImpl_kernel",_HandShakeKernel);

		    // Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D5);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D6);    
      
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("mapImpl_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, mapImpl_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, mapImpl_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void memset_int_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=16;
	printf("Kid%d",kid);
	int numRun=8;
	int runSize=rLen/numRun;	
	from=0;
	to=runSize;
	int smallKey = rand()%100;
	int largeKey = smallKey;
	int beginPos = 0;
	int value=0x7f;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("memset_int_kernel",_HandShakeKernel);

		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&value);   

        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("memset_int_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, memset_int_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, memset_int_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void blockAddition_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=17;
	printf("Kid%d",kid);
	int numRun=8;
	int runSize=rLen/numRun;	
	from=0;
	to=runSize;
	int smallKey = rand()%100;
	int largeKey = smallKey;
	int beginPos = 0;
	int value=0x7f;
	int blockSize=256;
	/*private*/
	cl_event eventList[2];
	int index=0;
	cl_kernel _Kernel; 
	int FLAG_CPU_GPU;
	double burden;	
	static cl_ulong usedLocalMemory;       /**< Used local memory by _HandShakeKernel */
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(rLen,SP);

		
    /* Do block-wise sum */
    bScan_int(SP->gLength, &D1, &SP->outputBuffer[0], &SP->blockSumBuffer[0],&index,eventList,&_Kernel,&FLAG_CPU_GPU,&burden,SP,0);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
    for(int i = 1; i < (int)SP->pass; i++)
    {
        bScan_int((cl_uint)(SP->gLength / pow((float)SP->blockSize, (float)i)), 
            &SP->blockSumBuffer[i - 1],
            &SP->outputBuffer[i],
            &SP->blockSumBuffer[i],&index,eventList,&_Kernel,&FLAG_CPU_GPU,&burden,SP,0);
    }
	clWaitForEvents(1,&eventList[(index-1)%2]); 
    int tempLength = (int)(SP->gLength / pow((float)SP->blockSize, (float)SP->pass));

	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_int status;
		cl_getKernel("blockAddition_kernel",_HandShakeKernel);
		/* set the block size*/ 
		size_t globalThreads[1]= {tempLength};
		size_t localThreads[1] = {SP->blockSize};

   
		/*** Set appropriate arguments to the _HandShakeKernel ***/
		/* 1st argument to the _HandShakeKernel - inputBuffer */
		status = clSetKernelArg(
			(*_HandShakeKernel), 
			0, 
			sizeof(cl_mem), 
			(void*)&SP->tempBuffer);
		assert(status==CL_SUCCESS);

		/* 2nd argument to the _HandShakeKernel - SP->outputBuffer */
		status = clSetKernelArg(
			(*_HandShakeKernel), 
			1, 
			sizeof(cl_mem), 
			(void *)&SP->outputBuffer[SP->pass - 1]);
		assert(status==CL_SUCCESS);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("blockAddition_kernel invocatio overhead, %f\n", t);
#endif
	}
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	closeScan(SP);
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, blockAddition_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, blockAddition_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void prefixSum_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=18;
	printf("Kid%d",kid);
	int numRun=8;
	int runSize=rLen/numRun;	
	from=0;
	to=runSize;
	int smallKey = rand()%TEST_MAX;
	int largeKey = smallKey;
	int beginPos = 0;
	int value=0x7f;
	int blockSize=256;
	/*private*/
	cl_event eventList[2];
	int index=0;
	cl_kernel _Kernel; 
	int FLAG_CPU_GPU;
	double burden;	
	static cl_ulong usedLocalMemory;       /**< Used local memory by _HandShakeKernel */
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(rLen,SP);	
    /* Do block-wise sum */
    bScan_int(SP->gLength, &D1, &SP->outputBuffer[0], &SP->blockSumBuffer[0],&index,eventList,&_Kernel,&FLAG_CPU_GPU,&burden,SP,0);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
    for(int i = 1; i < (int)SP->pass; i++)
    {
        bScan_int((cl_uint)(SP->gLength / pow((float)SP->blockSize, (float)i)), 
            &SP->blockSumBuffer[i - 1],
            &SP->outputBuffer[i],
            &SP->blockSumBuffer[i],&index,eventList,&_Kernel,&FLAG_CPU_GPU,&burden,SP,0);
			clWaitForEvents(1,&eventList[(index-1)%2]); 
    }	
	clWaitForEvents(1,&eventList[(index-1)%2]); 
    int tempLength = (int)(SP->gLength / pow((float)SP->blockSize, (float)SP->pass));
	size_t numThreadsPerBlock_x = 256;
    size_t globalWorkingSetSize = 32 * 64;
	cl_int status;
	int timer = DLL_genTimer(kid);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	for(i=0;i<Count;i++){
    DLL_getTimer(timer);
    cl_getKernel("prefixSum_kernel",_HandShakeKernel);
    /* Set appropriate arguments to the _HandShakeKernel */
    /* 1st argument to the _HandShakeKernel - SP->outputBuffer */
    status = clSetKernelArg(
        (*_HandShakeKernel), 
        0, 
        sizeof(cl_mem), 
        (void *)&SP->tempBuffer);
	assert(status==CL_SUCCESS);

    /* 2nd argument to the _HandShakeKernel - inputBuffer */
    status = clSetKernelArg(
        (*_HandShakeKernel), 
        1, 
        sizeof(cl_mem), 
        (void *)&SP->blockSumBuffer[SP->pass - 1]);
	assert(status==CL_SUCCESS);

    /* 3rd argument to the _HandShakeKernel - local memory */
    status = clSetKernelArg(
        (*_HandShakeKernel), 
        2, 
        tempLength * sizeof(cl_int), 
        NULL);
	assert(status==CL_SUCCESS);

    /* 4th argument to the _HandShakeKernel - SP->gLength */
    status = clSetKernelArg(
        (*_HandShakeKernel), 
        3, 
        sizeof(cl_int),
        (void*)&tempLength);
		assert(status==CL_SUCCESS);
		printf("prefixSum_kernel lanch\n");
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("prefixSum_kernel invocation overhead, %f\n", t);
#endif
	}
	printf("prefixsum finish\n");
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	//closeScan(SP);
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, prefixSum_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, prefixSum_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void ScanLargeArrays_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=19;
	printf("Kid%d",kid);
	int numRun=8;
	int runSize=rLen/numRun;	
	from=0;
	to=runSize;
	int smallKey = rand()%100;
	int largeKey = smallKey;
	int beginPos = 0;
	int value=0x7f;
	int blockSize=256;
	/*private*/
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(rLen,SP);
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("ScanLargeArrays_kernel",_HandShakeKernel);
		 /* Set appropriate arguments to the _HandShakeKernel */
		/* 1st argument to the _HandShakeKernel - SP->outputBuffer */
		cl_int status = clSetKernelArg( (*_HandShakeKernel), 0, sizeof(cl_mem), (void *)&D6);
		/* 2nd argument to the _HandShakeKernel - inputBuffer */
		status |= clSetKernelArg( (*_HandShakeKernel), 1, sizeof(cl_mem),  (void *)&D5);
		/* 3rd argument to the _HandShakeKernel - local memory */
		status |= clSetKernelArg( (*_HandShakeKernel),  2,  blockSize * sizeof(cl_int), NULL);
		/* 4th argument to the _HandShakeKernel - block_size  */
		status |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int),&SP->blockSize);
		/* 5th argument to the _HandShakeKernel - SP->gLength  */
		status |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), &SP->gLength);
		/* 6th argument to the _HandShakeKernel - sum of blocks  */
		status |= clSetKernelArg( (*_HandShakeKernel), 5, sizeof(cl_mem), SP->blockSumBuffer);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("ScanLargeArrays_kernel invocatio overhead, %f\n", t);
#endif
	}
	closeScan(SP);
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, ScanLargeArrays_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, ScanLargeArrays_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}

void filterImpl_map_kernel_handshake (int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=20;
	printf("Kid%d",kid);
	int numRun=8;
	int runSize=rLen/numRun;	
	from=0;
	to=runSize;
	int smallKey = rand()%100;
	int largeKey = smallKey;
	int beginPos = 0;
	int timer = DLL_genTimer(kid);
	for(i=0;i<Count;i++){
		
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("filterImpl_map_kernel",_HandShakeKernel);

		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&beginPos);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&rLen);
		ciErr1 = clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D5);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&smallKey);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_int), (void*)&largeKey);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_mem), (void*)&D3); 
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("filterImpl_map_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, filterImpl_map_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, filterImpl_map_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void filterImpl_write_kernel_handshake (int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=23;
	printf("Kid%d",kid);
	int numRun=8;
	int runSize=rLen/numRun;	
	from=0;
	to=runSize;
	int smallKey = rand()%100;
	int largeKey = smallKey;
	int beginPos = 0;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("filterImpl_write_kernel",_HandShakeKernel);

		// Set the Argument values
		 cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D2);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D5);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D6);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&beginPos);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_int), (void*)&rLen);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("filterImpl_write_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, filterImpl_write_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, filterImpl_write_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void optScatter_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=24;
	printf("Kid%d",kid);
	int numRun=8;
	int runSize=rLen/numRun;	
	from=0;
	to=runSize;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("optScatter_kernel",_HandShakeKernel);
        // Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&from);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&to);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_mem), (void*)&D3);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("optScatter_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, optScatter_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, optScatter_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void optGather_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=25;
	printf("Kid%d",kid);
	int numRun=8;
	int runSize=rLen/numRun;	
	from=0;
	to=runSize;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
        cl_getKernel("optGather_kernel", _HandShakeKernel);
        // Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&from);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&to);
		ciErr1 = clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_mem), (void*)&D3);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_int), (void*)&pLen);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("optGather_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, optGather_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, optGather_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}

void partition_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=26;
	printf("Kid%d",kid);
	int numPart=1;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
        cl_getKernel("partition_kernel",_HandShakeKernel);
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&numPart);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D5);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_mem), (void*)&D6);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("partition_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, partition_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, partition_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}

void mapPart_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=27;
	printf("Kid%d",kid);
	int numPart=1;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
        cl_getKernel("mapPart_kernel",_HandShakeKernel);
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&numPart);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D5);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_mem), (void*)&D6);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("mapPart_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, mapPart_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, mapPart_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void countHist_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=28;
	printf("Kid%d",kid);
	int numThreadPB=256;
    size_t globalWorkingSetSize = 32 * 64;
	int numPart=8;
	/*private*/
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	double burden;
	int FLAG_CPU_GPU=0;
		//map->pid
	mapPart(D1, rLen, numPart, D6, SPLIT,numThreadPB, globalWorkingSetSize,&index,eventList,_HandShakeKernel,&FLAG_CPU_GPU,&burden,0);
	
	//pid->write loc
	size_t numThreadsPerBlock_x=0;
	if(numThreadPB==-1)
		numThreadsPerBlock_x=1<<(log2((int)(SHARED_MEMORY_PER_PROCESSOR/(numPart*sizeof(int)))));
	else
		numThreadsPerBlock_x=numThreadPB;
	if(numThreadsPerBlock_x>256)
		numThreadsPerBlock_x=256;
	int sharedMemSize=numThreadsPerBlock_x*numPart*sizeof(int);

	//assert(numThreadsPerBlock_x>=16);
	int numThreadsPerBlock_y=1;
	int numBlock_x;
	int numThreadBlock=-1;
	if(numThreadBlock==-1)
		numBlock_x=512;
	else
		numBlock_x=numThreadBlock;
	//printf("numThreadsPerBlock_x, %d,sharedMemSize, %d,  numBlock_x, %d\n", numThreadsPerBlock_x, sharedMemSize,numBlock_x);
	int numBlock_y=1;
	int numThread=numBlock_x*numThreadsPerBlock_x;
	int numInPS=numThread*numPart;

	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
       	cl_getKernel("countHist_kernel",_HandShakeKernel);
		 // Set the Argument values
		//(__global Record *d_R, int rLen,__global int *loc, int from, int to, __global Record *d_S)
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D6);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&numPart);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D5);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sharedMemSize, NULL);


        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("countHist_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, countHist_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, countHist_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void writeHist_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=29;
	printf("Kid%d",kid);
	int numPart=8;
	int numThreadPB=256;
	int numBlock=32*64;
    size_t globalWorkingSetSize = 32 * 64;

	/*private*/
	cl_event eventList[2];//now it seems this is a memory wasted method, change later!!!
	int index=0;
	double burden;
	int FLAG_CPU_GPU=0;

	int result=0;
	int memSize=sizeof(Record)*rLen;
	int outSize=sizeof(Record)*rLen;
	void* Rin;
	HOST_MALLOC(Rin, memSize);
	void* h_bound;
	HOST_MALLOC(h_bound, numPart*sizeof(Record));
	generateRand((Record*)Rin,TEST_MAX,rLen,0);
	void *Rout;
	HOST_MALLOC(Rout, outSize);
	CL_MALLOC(&D1, memSize);
	CL_MALLOC(&D2, outSize);
	CL_MALLOC(&D3, numPart*sizeof(Record));

	//copy to
	cl_writebuffer(D1, Rin, memSize,&index,eventList,&FLAG_CPU_GPU,&burden,0);
		//map->pid
	CL_MALLOC(&D5, sizeof(int)*rLen);
	mapPart(D1, rLen, numPart, D5, SPLIT,numThreadPB, numBlock,&index,eventList,_HandShakeKernel,&FLAG_CPU_GPU,&burden,0);
	
	/*void* tempResult;
	int totalSize=sizeof(int)*rLen;
	HOST_MALLOC(tempResult, totalSize);
	//cl_readbuffer(tempResult,D5,totalSize,_HandShakeCPU_GPU);*/
	
	//pid->write loc
	size_t numThreadsPerBlock_x=0;
	if(numThreadPB==-1)
		numThreadsPerBlock_x=1<<(log2((int)(SHARED_MEMORY_PER_PROCESSOR/(numPart*sizeof(int)))));
	else
		numThreadsPerBlock_x=numThreadPB;
	if(numThreadsPerBlock_x>256)
		numThreadsPerBlock_x=256;
	int sharedMemSize=numThreadsPerBlock_x*numPart*sizeof(int);

	//assert(numThreadsPerBlock_x>=16);
	int numThreadsPerBlock_y=1;
	int numBlock_x;
	int numThreadBlock=-1;
	if(numThreadBlock==-1)
		numBlock_x=512;
	else
		numBlock_x=numThreadBlock;
	//printf("numThreadsPerBlock_x, %d,sharedMemSize, %d,  numBlock_x, %d\n", numThreadsPerBlock_x, sharedMemSize,numBlock_x);
	int numBlock_y=1;
	int numThread=numBlock_x*numThreadsPerBlock_x;
	int numInPS=numThread*numPart;
	CL_MALLOC(&D6, sizeof(int)*numInPS);
	countHist_int(D5, rLen, numPart, D6, numThreadsPerBlock_x, numBlock_x, sharedMemSize,&index,eventList,_HandShakeKernel,&FLAG_CPU_GPU,&burden,0);
	
	
	
	//prefix sum
	CL_MALLOC(&D7, sizeof(int)*numInPS);
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(numInPS,SP);
	scanImpl(D6, numInPS, D7,&index,eventList,_HandShakeKernel,&FLAG_CPU_GPU,&burden,SP,0);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	closeScan(SP);
	
	CL_MALLOC(&D6, sizeof(int)*rLen);
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
       cl_getKernel("writeHist_kernel",_HandShakeKernel);
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D5);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&numPart);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D7);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_mem), (void*)&D6);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sharedMemSize, NULL);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("writeHist_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, writeHist_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, writeHist_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void getBound_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=30;
	printf("Kid%d",kid);
	int numPart=8;
	size_t numThreadsPerBlock_x = 256;
    size_t globalWorkingSetSize = 32 * 64;
	int sharedMemSize=numThreadsPerBlock_x*numPart*sizeof(int);
	int interval=0;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
		cl_getKernel("getBound_kernel",_HandShakeKernel);
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D5);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&interval);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&numPart);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_mem), (void*)&D6);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("getBound_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, getBound_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, getBound_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}

void BitonicSort_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=31;
	printf("Kid%d",kid);
	int stage=0;
	int passOfStage=0;
	cl_int  sortAscending = 1; //1: ascending order, 0: descending order
	size_t numThreadsPerBlock_x = 256;
    size_t globalWorkingSetSize = 32 * 64;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
		cl_getKernel("BitonicSort_kernel",_HandShakeKernel);

		cl_int err  = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void *) &D1);
		err |= clSetKernelArg((*_HandShakeKernel),1,sizeof(cl_uint),(void*)&stage);
		err |= clSetKernelArg((*_HandShakeKernel),2,sizeof(cl_uint),(void*)&passOfStage);
		err |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_uint), (void *) &rLen);
		err |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_uint), (void *) &sortAscending);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("BitonicSort_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, BitonicSort_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, BitonicSort_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}

void gpuNLJ_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=38;
	printf("kid:%d\n",kid);
	int sStart=0;
	int rLen=256*1024;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("gpuNLJ_kernel",_HandShakeKernel);
		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D6);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D3);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&sStart);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&rLen);
		ciErr1 = clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_int), (void*)&rLen);	
		ciErr1 = clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_mem), (void*)&D5);	
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("gpuNLJ_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gpuNLJ_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gpuNLJ_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void nlj_write_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=39;
	printf("Kid%d",kid);
	int sStart=0;
	int rLen=256*1024;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("nlj_write_kernel",_HandShakeKernel);
		// Set the Argument values 
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&sStart);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&rLen);
		ciErr1 = clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&rLen);	
		ciErr1 = clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_mem), (void*)&D5);	
		ciErr1 = clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_mem), (void*)&D3);	
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("nlj_write_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, nlj_write_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, nlj_write_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void gSearchTree_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=40;
	printf("Kid%d",kid);
	unsigned int nDataNodes;
	nDataNodes = uintCeilingDiv(rLen, TREE_NODE_SIZE);
	//#region Calculate parameters on host
	unsigned int lvlDir = uintCeilingLog(TREE_FANOUT, nDataNodes);
	unsigned int nDirNodes = uintCeilingDiv(nDataNodes - 1, TREE_NODE_SIZE);
	unsigned int tree_size = nDirNodes + nDataNodes;
	unsigned int bottom_start = ( uintPower(TREE_FANOUT, lvlDir) - 1 ) / TREE_NODE_SIZE;
	//#endregion
	unsigned int nNodesPerBlock = uintCeilingDiv(nDirNodes, BLCK_PER_GRID_create);
	unsigned int nKeysPerThread = uintCeilingDiv(pLen, THRD_PER_GRID_search);
	//#region Execute on device
	int numThreadPB=THRD_PER_BLCK_create;
	int numBlock= BLCK_PER_GRID_create;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("gSearchTree_kernel",_HandShakeKernel);

		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&nDataNodes);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&nDirNodes);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&lvlDir);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_mem), (void*)&D3);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_mem), (void*)&D4);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 7, sizeof(cl_int), (void*)&pLen);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 8, sizeof(cl_int), (void*)&nKeysPerThread); 
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 9, sizeof(cl_int), (void*)&tree_size);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 10, sizeof(cl_int), (void*)&bottom_start); 
		int rLen=nDirNodes*nKeysPerThread/THRD_PER_GRID_search;
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("gSearchTree_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gSearchTree_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gSearchTree_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void gIndexJoin_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=41;
	printf("Kid%d",kid);
	unsigned int nDataNodes;
	nDataNodes = uintCeilingDiv(rLen, TREE_NODE_SIZE);
	//#region Calculate parameters on host
	unsigned int lvlDir = uintCeilingLog(TREE_FANOUT, nDataNodes);
	unsigned int nDirNodes = uintCeilingDiv(nDataNodes - 1, TREE_NODE_SIZE);
	unsigned int tree_size = nDirNodes + nDataNodes;
	unsigned int bottom_start = ( uintPower(TREE_FANOUT, lvlDir) - 1 ) / TREE_NODE_SIZE;
	//#endregion
	unsigned int nNodesPerBlock = uintCeilingDiv(nDirNodes, BLCK_PER_GRID_create);
	unsigned int nKeysPerThread = uintCeilingDiv(pLen, THRD_PER_GRID_search);
		int clusterSize = uintCeilingDiv(pLen, THRD_PER_GRID_join);
	//#region Execute on device
	int numThreadPB=THRD_PER_BLCK_create;
	int numBlock= BLCK_PER_GRID_create;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("gIndexJoin_kernel",_HandShakeKernel);

		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D5);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&pLen);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_mem), (void*)&D6);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_int), (void*)&clusterSize);
		int rLen=nDirNodes*nKeysPerThread/THRD_PER_GRID_search;
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("gIndexJoin_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gIndexJoin_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gIndexJoin_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void gJoinWithWrite_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=42;
	printf("Kid%d",kid);
	unsigned int nDataNodes;
	nDataNodes = uintCeilingDiv(rLen, TREE_NODE_SIZE);
	//#region Calculate parameters on host
	unsigned int lvlDir = uintCeilingLog(TREE_FANOUT, nDataNodes);
	unsigned int nDirNodes = uintCeilingDiv(nDataNodes - 1, TREE_NODE_SIZE);
	unsigned int tree_size = nDirNodes + nDataNodes;
	unsigned int bottom_start = ( uintPower(TREE_FANOUT, lvlDir) - 1 ) / TREE_NODE_SIZE;
	//#endregion
	unsigned int nNodesPerBlock = uintCeilingDiv(nDirNodes, BLCK_PER_GRID_create);
	unsigned int nKeysPerThread = uintCeilingDiv(pLen, THRD_PER_GRID_search);
		int clusterSize = uintCeilingDiv(pLen, THRD_PER_GRID_join);
	//#region Execute on device
	int numThreadPB=THRD_PER_BLCK_create;
	int numBlock= BLCK_PER_GRID_create;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("gJoinWithWrite_kernel",_HandShakeKernel);

		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D5);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&pLen);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_mem), (void*)&D6);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_mem), (void*)&D3);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 7, sizeof(cl_int), (void*)&clusterSize);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("gJoinWithWrite_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gJoinWithWrite_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gJoinWithWrite_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void gCreateIndex_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=43;
	printf("Kid%d",kid);
	unsigned int nDataNodes;
	nDataNodes = uintCeilingDiv(rLen, TREE_NODE_SIZE);
	//#region Calculate parameters on host
	unsigned int lvlDir = uintCeilingLog(TREE_FANOUT, nDataNodes);
	unsigned int nDirNodes = uintCeilingDiv(nDataNodes - 1, TREE_NODE_SIZE);
	unsigned int tree_size = nDirNodes + nDataNodes;
	unsigned int bottom_start = ( uintPower(TREE_FANOUT, lvlDir) - 1 ) / TREE_NODE_SIZE;
	//#endregion
	unsigned int nNodesPerBlock = uintCeilingDiv(nDirNodes, BLCK_PER_GRID_create);

	//#region Execute on device
	int numThreadPB=THRD_PER_BLCK_create;
	int numBlock= BLCK_PER_GRID_create;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("gCreateIndex_kernel",_HandShakeKernel);

		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&nDirNodes);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&tree_size);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&bottom_start);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_int), (void*)&nNodesPerBlock);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("gCreateIndex_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gCreateIndex_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gCreateIndex_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void quanMap_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=44;
	printf("Kid%d",kid);
    size_t globalWorkingSetSize = 32 * 64;
	/*private*/
	int interval=SMJ_NUM_THREADS_PER_BLOCK;
	int numThreadsPerBlock_x=interval;
	int numThreadsPerBlock_y=1;
	int numBlock_X=divRoundUp(rLen, interval);
	int numBlock_Y=1;
	if(numBlock_X>NLJ_MAX_NUM_BLOCK_PER_DIM)
	{
		numBlock_Y=numBlock_X/NLJ_MAX_NUM_BLOCK_PER_DIM;
		if(numBlock_X%NLJ_MAX_NUM_BLOCK_PER_DIM!=0)
			numBlock_Y++;
		numBlock_X=NLJ_MAX_NUM_BLOCK_PER_DIM;
	}
	/////////////
	size_t  thread[2]={numThreadsPerBlock_x, numThreadsPerBlock_y};
	size_t  grid[2]={ numBlock_X*numThreadsPerBlock_x* numBlock_Y*numThreadsPerBlock_y, 1 };
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
		cl_getKernel("quanMap_kernel",_HandShakeKernel);

		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&interval);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_mem), (void*)&D2);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, interval*sizeof(Record), NULL);    
        cl_launchKernel(1,  grid,  thread, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("gCreateIndex_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, quanMap_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, quanMap_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void gSearchTree_usingKeys_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=45;
	printf("Kid%d",kid);
	/*private*/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];
	int index=0;
	cl_kernel test_Kernel; 
	int FLAG_CPU_GPU;
	double burden;
	int numResult = 0;

	int interval=SMJ_NUM_THREADS_PER_BLOCK;
	int numQuanR=divRoundUp(rLen,interval);
	CL_MALLOC(&D3,numQuanR*sizeof(int)*2); 
	getQuantile(D1, rLen, interval,D3,numQuanR,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,0);
	CL_MALLOC(&D4,numQuanR*sizeof(int)*2); 
	CUDA_CSSTree* tree;
	gpu_constructCSSTreeImpl(D2, rLen, &tree,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,0);

	///////////cuda_search_index_usingKeys////////////
	int nSearchKeys= numQuanR*2;
	unsigned int lvlDir = uintCeilingLog(TREE_FANOUT, tree->nDataNodes);
	unsigned int tree_size = tree->nDataNodes + tree->nDirNodes;
	unsigned int bottom_start = ( uintPower(TREE_FANOUT, lvlDir) - 1 ) / TREE_NODE_SIZE;

	int numThreadPB=THRD_PER_BLCK_search;
	int numBlock=BLCK_PER_GRID_search;
	

	unsigned int nKeysPerThread = uintCeilingDiv(nSearchKeys, THRD_PER_GRID_search);

	size_t numThreadsPerBlock_x = 256;
    size_t globalWorkingSetSize = 32 * 64;
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	int timer = DLL_genTimer(kid);
	for(i=0;i<Count;i++){	
        DLL_getTimer(timer);
		cl_getKernel("gSearchTree_usingKeys_kernel",_HandShakeKernel);
		// Set the Argument values
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&tree->data);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&tree->nDataNodes);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&tree->dir);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&tree->nDirNodes);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_int), (void*)&lvlDir);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_mem), (void*)&D3);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_mem), (void*)&D4);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 7, sizeof(cl_int), (void*)&nSearchKeys);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 8, sizeof(cl_int), (void*)&nKeysPerThread); 
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 9, sizeof(cl_int), (void*)&tree_size);    
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 10, sizeof(cl_int), (void*)&bottom_start); 
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("gSearchTree_usingKeys_kernel invocatio overhead, %f\n", t);
#endif
	}
	printf("try to delete tree\n");
	delete tree;
	printf("start test gSearchTree_usingKeys_kernel\n");
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gSearchTree_usingKeys_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, gSearchTree_usingKeys_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void joinMBCount_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=46;
	printf("Kid%d",kid);
	/*private*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];
	int index=0;
	cl_kernel test_Kernel; 
	int FLAG_CPU_GPU;
	double burden;
	int numResult = 0;
	int interval=SMJ_NUM_THREADS_PER_BLOCK;
	int numQuanR=divRoundUp(rLen,interval);
	CL_MALLOC(&D4,numQuanR*sizeof(int)*2); 
	getQuantile(D1, rLen, interval,D4,numQuanR,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,0);
	CL_MALLOC(&D5,numQuanR*sizeof(int)*2); 
	CUDA_CSSTree* tree;
	gpu_constructCSSTreeImpl(D1, rLen, &tree,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,0);
	cuda_search_index_usingKeys(tree->data, tree->nDataNodes, tree->dir,
		tree->nDirNodes, D4, D5, numQuanR*2,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,0);	
	int numThreadPerBlock =SMJ_NUM_THREADS_PER_BLOCK;
	int numBlock_X=numQuanR;
	int numBlock_Y=1;
	if(numBlock_X>NLJ_MAX_NUM_BLOCK_PER_DIM)
	{
		numBlock_Y=numBlock_X/NLJ_MAX_NUM_BLOCK_PER_DIM;
		if(numBlock_X%NLJ_MAX_NUM_BLOCK_PER_DIM!=0)
			numBlock_Y++;
		numBlock_X=NLJ_MAX_NUM_BLOCK_PER_DIM;
	}
	dim3  threads_NLJ( numThreadPerBlock, 1, 1);
	dim3  grid_NLJ( numBlock_X, numBlock_Y, 1);
	int resultBuf=grid_NLJ.x*grid_NLJ.y*numThreadPerBlock;
	CL_MALLOC(&D6, sizeof(int)*resultBuf );
	CL_MALLOC(&D7, sizeof(int)*resultBuf );	
	int h_n =0;	
	int h_sum =0;
	size_t numThreadsPerBlock_x = 256;
    size_t globalWorkingSetSize = 32 * 64;
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	printf("start test joinMBCount_kernel\n");
	int timer = DLL_genTimer(kid);
	for(i=0;i<Count;i++){
        DLL_getTimer(timer);
		cl_getKernel("joinMBCount_kernel",_HandShakeKernel);
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D1);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_mem), (void*)&D5);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_int), (void*)&numQuanR);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_mem), (void*)&D3);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("joinMBCount_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, joinMBCount_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, joinMBCount_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
		delete tree;
}
void joinMBWrite_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=47;
	printf("Kid%d",kid);
		/*private*/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_event eventList[2];
	int index=0;
	cl_kernel test_Kernel; 
	int FLAG_CPU_GPU;
	double burden;
	////////////////////MJImpl/////////////////////////////
	int numResult = 0;

	int interval=SMJ_NUM_THREADS_PER_BLOCK;
	int numQuanR=divRoundUp(rLen,interval);
	CL_MALLOC(&D3,numQuanR*sizeof(int)*2); 
	getQuantile(D1, rLen, interval,D3,numQuanR,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,0);
	CL_MALLOC(&D4,numQuanR*sizeof(int)*2); 
	CUDA_CSSTree* tree;
	gpu_constructCSSTreeImpl(D1, rLen, &tree,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,0);
	cuda_search_index_usingKeys(tree->data, tree->nDataNodes, tree->dir,
		tree->nDirNodes, D3, D4, numQuanR*2,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,0);	
///////////joinMatchingBlocks////////////////////////////////////////////////////
	int numResults=0;
	int numThreadPerBlock =SMJ_NUM_THREADS_PER_BLOCK;
	int numBlock_X=numQuanR;
	int numBlock_Y=1;
	if(numBlock_X>NLJ_MAX_NUM_BLOCK_PER_DIM)
	{
		numBlock_Y=numBlock_X/NLJ_MAX_NUM_BLOCK_PER_DIM;
		if(numBlock_X%NLJ_MAX_NUM_BLOCK_PER_DIM!=0)
			numBlock_Y++;
		numBlock_X=NLJ_MAX_NUM_BLOCK_PER_DIM;
	}
	dim3  threads_NLJ( numThreadPerBlock, 1, 1);
	dim3  grid_NLJ( numBlock_X, numBlock_Y, 1);
	int resultBuf=grid_NLJ.x*grid_NLJ.y*numThreadPerBlock;
	CL_MALLOC(&D5, sizeof(int)*resultBuf );
	CL_MALLOC(&D6, sizeof(int)*resultBuf );
	int h_n =0;	
	int h_sum =0;
	joinMBCount(D1, rLen, D2, rLen, 
				D4, numQuanR, D5, grid_NLJ, threads_NLJ,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,0);
	ScanPara *SP;
	SP=(ScanPara*)malloc(sizeof(ScanPara));
	initScan(resultBuf,SP);
	scanImpl(D5, resultBuf, D6,&index,eventList,&test_Kernel,&FLAG_CPU_GPU,&burden,SP,0);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	closeScan(SP);
	cl_readbuffer((void*)&h_n, D5, (resultBuf-1)*sizeof(int), sizeof(int),&index,eventList,&FLAG_CPU_GPU,&burden,0);
	cl_readbuffer((void*)&h_sum, D6, (resultBuf-1)*sizeof(int), sizeof(int),&index,eventList,&FLAG_CPU_GPU,&burden,0);
	clWaitForEvents(1,&eventList[(index-1)%2]); 
	numResults=h_n+h_sum;
	CL_MALLOC(&D7, sizeof(Record)*numResults );
	size_t numThreadsPerBlock_x = 256;
    size_t globalWorkingSetSize = 32 * 64;
	printf("start test joinMBWrite_kernel\n");
		int timer = DLL_genTimer(kid);
	for(i=0;i<Count;i++){
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("joinMBWrite_kernel",_HandShakeKernel);
		// Set the Argument values
		//gpuNLJ_int_kernel(cl_mem d_temp, Record *d_R, Record *d_S, int sStart, int rLen, int sLen, int *D5) 
		cl_int ciErr1 = clSetKernelArg((*_HandShakeKernel), 0, sizeof(cl_mem), (void*)&D2);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 1, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 2, sizeof(cl_mem), (void*)&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 3, sizeof(cl_int), (void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 4, sizeof(cl_mem), (void*)&D4);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 5, sizeof(cl_int), (void*)&numQuanR);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 6, sizeof(cl_mem), (void*)&D6);	
		ciErr1 |= clSetKernelArg((*_HandShakeKernel), 7, sizeof(cl_mem), (void*)&D7);
        cl_launchKernel(1,  & globalWorkingSetSize,  & numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("joinMBWrite_kernel invocatio overhead, %f\n", t);
#endif
	}
	printf("try to delete tree\n");
	delete tree;
	printf("start test gSearchTree_usingKeys_kernel\n");
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, joinMBWrite_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, joinMBWrite_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
     }
}



void build_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=49;
	printf("Kid%d",kid);
	cl_uint  rHashTableBucketNum = 2 * 1024 * 1024;
	CL_MALLOC(&D2,rLen * sizeof(Record) + rHashTableBucketNum * sizeof(cl_uint));
	//#region Execute on device;
	for(i=0;i<Count;i++){
		int timer = DLL_genTimer(kid);
        DLL_getTimer(timer);
        size_t numThreadsPerBlock_x = 256;
        size_t globalWorkingSetSize = 32 * 64;
		cl_getKernel("build_kernel",_HandShakeKernel);

		//configure build _HandShakeKernel
		cl_int ciErr1 =  clSetKernelArg((*_HandShakeKernel),0,sizeof(cl_mem),&D1);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel),1,sizeof(cl_mem),&D2);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel),2,sizeof(cl_uint),(void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel),3,sizeof(cl_uint),(void*)&rLen);
		ciErr1 |= clSetKernelArg((*_HandShakeKernel),4,sizeof(cl_uint),(void*)&rHashTableBucketNum);

        cl_launchKernel(1,  &globalWorkingSetSize,  &numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("build_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, build_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, build_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}
void probe_kernel_handshake(int _HandShakeCPU_GPU, cl_kernel  *_HandShakeKernel)
{
	double i;
	double sum=0;
	double scaler=1000;
	int kid=50;
	printf("Kid%d",kid);
	cl_int result = 0;
	cl_uint  rHashTableBucketNum = 2 * 1024 * 1024;

	//size of R and S tables
	int memSizeR = sizeof(Record) * rLen;
	int memSizeS = sizeof(Record) * rLen;
	void * h_R;
	HOST_MALLOC(h_R,memSizeR);
	generateRand((Record*)h_R,TEST_MAX,rLen,0);
	void * h_S;
	HOST_MALLOC(h_S,memSizeS);
	generateRand((Record*)h_S,TEST_MAX,rLen,1);
	CL_MALLOC(&D2,memSizeR);
	cl_writebuffer(D2,h_R,memSizeR,_HandShakeCPU_GPU);
	CL_MALLOC(&D3,memSizeS);
	cl_writebuffer(D3,h_S,memSizeS,_HandShakeCPU_GPU);
	CL_MALLOC(&D4,rLen * sizeof(Record) + rHashTableBucketNum * sizeof(cl_uint),_HandShakeCPU_GPU);
	cl_uint  resultsNum = rLen;
	size_t groupSize = 256, globalSize = 8192;
	CL_MALLOC(&D1,sizeof(Record) * resultsNum * 2);
	size_t numThreadsPerBlock_x = groupSize;
	size_t globalWorkingSetSize = globalSize;
	cl_getKernel("build_kernel",_HandShakeKernel);

	//configure build _HandShakeKernel
	cl_int ciErr1 =  clSetKernelArg((*_HandShakeKernel),0,sizeof(cl_mem),&D2);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),1,sizeof(cl_mem),&D4);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),2,sizeof(cl_uint),(void*)&rLen);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),3,sizeof(cl_uint),(void*)&rLen);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),4,sizeof(cl_uint),(void*)&rHashTableBucketNum);

	if (ciErr1 != CL_SUCCESS)
	{
		printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
		cl_clean(EXIT_FAILURE);
	}
	cl_launchKernel(1, &globalWorkingSetSize, &numThreadsPerBlock_x,_HandShakeKernel, _HandShakeCPU_GPU);
	for(i=0;i<Count;i++){
	int timer = DLL_genTimer(kid);
    DLL_getTimer(timer);
	cl_getKernel("probe_kernel",_HandShakeKernel);

	//configure probe _HandShakeKernel
	ciErr1 =  clSetKernelArg((*_HandShakeKernel),0,sizeof(cl_mem),&D4);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),1,sizeof(cl_mem),&D3);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),2,sizeof(cl_mem),&D1);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),3,sizeof(cl_uint),(void*)&rLen);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),4,sizeof(cl_uint),(void*)&rLen);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),5,sizeof(cl_uint),(void*)&rHashTableBucketNum);
	ciErr1 |= clSetKernelArg((*_HandShakeKernel),6,sizeof(cl_uint),(void*)&resultsNum);

	if (ciErr1 != CL_SUCCESS)
	{
		printf("Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
		cl_clean(EXIT_FAILURE);
	}
	cl_launchKernel(1, &globalWorkingSetSize, &numThreadsPerBlock_x, _HandShakeKernel, _HandShakeCPU_GPU);
	        double t = DLL_getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("probe_kernel invocatio overhead, %f\n", t);
#endif
	}
	    if (_HandShakeCPU_GPU) {
            AddGPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, probe_kernel invocatio overhead in average in GPU, %lf\n", sum, AddGPUBurden[kid]);
        } else {
            AddCPUBurden[kid] = sum/Count*scaler;
			 printf("sum is %lf\n, probe_kernel invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden[kid]);
        }
}