#include "common.h"
#include "CPU_Dll.h"
#include "OpenCL_DLL.h"
#include "helper.h"
#include "QueryPlanNode.h"
#include "db.h"
/////////////////////////////////////////////////////////////
extern double OP_LothresholdForGPUApp;
extern double OP_LothresholdForCPUApp;
extern double SpeedupGPUOverCPU_Operator[25];
extern double AddCPUBurden_OP[25];
extern double AddGPUBurden_OP[25];
extern double OP_UpCPUBurden;
extern double OP_LoCPUBurden;
extern double OP_UpGPUBurden;
extern double OP_LoGPUBurden;
extern double OP_CPUBurden;
extern double OP_GPUBurden;
Record* Rin;
Record* Rin2;
Record* Rout;
Record* Rout2;//empty Rout
Record* RID;
cl_mem din;
cl_mem din2;
cl_mem dout1;
cl_mem dout2;//empty Rout
cl_mem dRID;
extern int OP_rLen;
double OCCount=1;
double scaler=1000;
extern FILE *OP_ofp;
int counter = 0;
void choise(double *a,int n)
{
	int i, j, k;
	double temp;
        for (i = 0; i < n - 1; i++) {
            k = i; 

            for (j = i + 1; j < n; j++) {
                if (a[k] > a[j]) {
                    k = j; 
                }
            }
            if (i != k) {
                temp = a[i];
                a[i] = a[k];
                a[k] = temp;
            }
        }
    }
void PROJECTION_HandShake(int CPU_GPU){
	int oid=PROJECTION;
	double i;
	double sum=0;
	for(i=0;i<OCCount;i++){
		for( int i = 0; i < OP_rLen; i++ )
		{
		Rin2[i].rid = rand()%OP_rLen;
		}
		CopyCPUToGPU(din,Rin,sizeof(Record)*OP_rLen);
		CopyCPUToGPU(din2,Rin2,sizeof(Record)*OP_rLen);
		int timer = genTimer(oid);
        getTimer(timer);
		CL_ProjectionOnly(din,OP_rLen,din2,OP_rLen,256, 64,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_Projection invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_Projection invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_Projection invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void SELECTION_HandShake(int CPU_GPU){
	int oid=SELECTION;
	double i;
	double sum=0;
	/*private*/	
	int smallKey = OP_rLen-rand()%TEST_MAX;
	int largeKey = OP_rLen;
	for(i=0;i<OCCount;i++){
		int timer = genTimer(oid);
        getTimer(timer);
		CL_RangeSelectionOnly(din,OP_rLen,smallKey, largeKey,&dout1,256,64,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_RangeSelection invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_RangeSelection invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_RangeSelection invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void AGG_SUM_HandShake(int CPU_GPU){
	int oid=AGG_SUM;
	double i;
	double sum=0;
	/*private*/
	int timer = genTimer(oid);
	for(i=0;i<OCCount;i++){
        getTimer(timer);
		CL_AggSumOnly(din,OP_rLen,&dout1,256,64,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_AggSum invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_AggSum invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_AggSum invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void AGG_MAX_HandShake(int CPU_GPU){
	int oid=AGG_MAX;
	double i;
	double sum=0;
	/*private*/
	int timer = genTimer(oid);
	for(i=0;i<OCCount;i++){
        getTimer(timer);
		CL_AggMaxOnly(din,OP_rLen,&dout2,256,64,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_AggMax invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_AggMax invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_AggMax invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void AGG_MIN_HandShake(int CPU_GPU){
	int oid=AGG_MIN;
	double i;
	double sum=0;
	/*private*/
	int timer = genTimer(oid);
	for(i=0;i<OCCount;i++){
        getTimer(timer);
		CL_AggMinOnly(din,OP_rLen,&dout2,256,64,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_AggMax invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_AggMin invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_AggMin invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void AGG_AVG_HandShake(int CPU_GPU){
	int oid=AGG_AVG;
	double i;
	double sum=0;
	/*private*/
	int timer = genTimer(oid);
	for(i=0;i<OCCount;i++){
        getTimer(timer);
		CL_AggAvgOnly(din,OP_rLen,&dout2,256,64,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_AggAvg invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_AggAvg invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_AggAvg invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void AGG_SUM_AFTER_GROUP_BY_HandShake(int CPU_GPU)
{
	int oid=AGG_SUM_AFTER_GROUP_BY;
	double i;
	double OCCount=100;
	double sum=0;
	double scaler=1000;
	int groupByRlen=0;
	int* startPos;
	int numGroups=0;
	/*private*/
	for(i=0;i<OCCount;i++){
		numGroups=CL_GroupBy(Rin,OP_rLen,Rout,&startPos,256,64,CPU_GPU);
		int* h_aggResults = (int*)malloc( sizeof(int)*numGroups );
		int timer = genTimer(1);
        getTimer(timer);
		CL_agg_sum_afterGroupBy(Rout,OP_rLen,startPos,numGroups,Rin2,h_aggResults,256,CPU_GPU);	
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_agg_sum_afterGroupBy invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_agg_sum_afterGroupBy invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_agg_sum_afterGroupBy invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void AGG_MAX_AFTER_GROUP_BY_HandShake(int CPU_GPU)
{
	int oid=AGG_MAX_AFTER_GROUP_BY;
	double i;
	double OCCount=100;
	double sum=0;
	double scaler=1000;
	int groupByRlen=0;
	int* startPos;
	int numGroups=0;
	/*private*/
	for(i=0;i<OCCount;i++){
		numGroups=CL_GroupBy(Rin,OP_rLen,Rout,&startPos,256,64,CPU_GPU);
		int* h_aggResults = (int*)malloc( sizeof(int)*numGroups );
		int timer = genTimer(1);
        getTimer(timer);
		CL_agg_max_afterGroupBy(Rout,OP_rLen,startPos,numGroups,Rin2,h_aggResults,256,CPU_GPU);	
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_agg_max_afterGroupBy invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_agg_max_afterGroupBy invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_agg_max_afterGroupBy invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void AGG_AVG_AFTER_GROUP_BY_HandShake(int CPU_GPU)
{
	int oid=AGG_AVG_AFTER_GROUP_BY;
	double i;
	double OCCount=100;
	double sum=0;
	double scaler=1000;
	int groupByRlen=0;
	int* startPos;
	int numGroups=0;
	/*private*/
	for(i=0;i<OCCount;i++){
		numGroups=CL_GroupBy(Rin,OP_rLen,Rout,&startPos,256,64,CPU_GPU);
		int* h_aggResults = (int*)malloc( sizeof(int)*numGroups );
		int timer = genTimer(1);
        getTimer(timer);
		CL_agg_avg_afterGroupBy(Rout,OP_rLen,startPos,numGroups,Rin2,h_aggResults,256,CPU_GPU);	
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_agg_avg_afterGroupBy invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_agg_avg_afterGroupBy invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_agg_avg_afterGroupBy invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void AGG_MIN_AFTER_GROUP_BY_HandShake(int CPU_GPU)
{
	int oid=AGG_MIN_AFTER_GROUP_BY;
	double i;
	double OCCount=100;
	double sum=0;
	double scaler=1000;
	int groupByRlen=0;
	int* startPos;
	int numGroups=0;
	/*private*/
	for(i=0;i<OCCount;i++){
		numGroups=CL_GroupBy(Rin,OP_rLen,Rout,&startPos,256,64,CPU_GPU);
		int* h_aggResults = (int*)malloc( sizeof(int)*numGroups );
		int timer = genTimer(1);
        getTimer(timer);
		CL_agg_min_afterGroupBy(Rout,OP_rLen,startPos,numGroups,Rin2,h_aggResults,256,CPU_GPU);	
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_agg_min_afterGroupBy invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_agg_min_afterGroupBy invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_agg_min_afterGroupBy invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void GROUP_BY_HandShake(int CPU_GPU)
{
	int oid=GROUP_BY;
	double i;
	double sum=0;
	double scaler=1000;
	int groupByRlen=0;
	int* startPos;
	int numGroups=0;
	/*private*/
	for(i=0;i<OCCount;i++){
		int timer = genTimer(1);
        getTimer(timer);
		numGroups=CL_GroupBy(Rin,OP_rLen,Rout,&startPos,256,64,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_GroupBy invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_GroupBy invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_GroupBy invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void ORDER_BY_HandShake(int CPU_GPU){
	int oid=ORDER_BY;
	double i;
	double sum=0;
	double scaler=1000;
	for(i=0;i<OCCount;i++){
		int timer = genTimer(1);
        getTimer(timer);
		CL_RadixSortOnly(din,OP_rLen,256,512,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_RadixSort invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_RadixSort invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_RadixSort invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void JOIN_NINLJ_HandShake(int CPU_GPU){
	int oid=JOIN_NINLJ;
	double i;
	double sum=0;
	int timer = genTimer(oid);
	generateRand((Record*)Rin, TEST_MAX,256*1024,1);
	generateRand((Record*)Rin2, TEST_MAX,256*1024,1);
	for(i=0;i<OCCount;i++){
        getTimer(timer);
		CL_ninlj(Rin,256*1024,Rin2,256*1024,&Rout,CPU_GPU);
		//CL_ninlj(Rin,20941,Rin2,21149,&Rout,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_ninlj invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_ninlj invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_ninlj invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void JOIN_INLJ_HandShake(int CPU_GPU){
	int oid=JOIN_INLJ;
	double i;
	double sum=0;
	CUDA_CSSTree *gpu_tree;
	CL_RadixSortOnly(din,OP_rLen,256,512,CPU_GPU);
	int timer = genTimer(oid);
	for(i=0;i<OCCount;i++){
        getTimer(timer);
		CL_inljOnly(din,OP_rLen,&gpu_tree,din2,OP_rLen,&dout2,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_inlj invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_inlj invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_inlj invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void JOIN_SMJ_HandShake(int CPU_GPU){
	int oid=JOIN_SMJ;
	double i;
	double sum=0;
	int timer = genTimer(oid);
	for(i=0;i<OCCount;i++){
		CL_RadixSortOnly(din,OP_rLen,256,512,CPU_GPU);
		CL_RadixSortOnly(din2,OP_rLen,256,512,CPU_GPU);
        getTimer(timer);
		CL_smjOnly(din,OP_rLen,din2,OP_rLen,&dout2,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_smj invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_smj invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_smj invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void JOIN_HJ_HandShake(int CPU_GPU){
	int oid=JOIN_HJ;
	double i;
	double sum=0;
	int timer = genTimer(oid);

	for(i=0;i<OCCount;i++){
        getTimer(timer);
		CL_hjOnly(din,OP_rLen,din2,OP_rLen,&dout2,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_hj invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_hj invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_hj invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void JOIN_MJ_HandShake(int CPU_GPU){
	int oid=JOIN_MJ;
	double i;
	double sum=0;
	int timer = genTimer(oid);
	for(i=0;i<OCCount;i++){

        getTimer(timer);
		CL_mj(Rin,256*1024,Rin2,256*1024,&Rout,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
#ifdef HandshakeDebug
        printf("CL_mj invocatio overhead, %f\n", t);
#endif
	}
		if (CPU_GPU) {
            AddGPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_mj invocatio overhead in average in GPU, %lf\n", sum,AddGPUBurden_OP[oid]);
        } else {
            AddCPUBurden_OP[oid] = sum/OCCount*scaler;
			 printf("sum is %lf\n, CL_mj invocatio overhead in average in CPU, %lf\n",sum, AddCPUBurden_OP[oid]);
        }
}
void HandShake() {
	char outputFilename[50];
	sprintf(outputFilename, "./Output/OpeartorSpecification.list");
	OP_ofp = fopen(outputFilename, "w");
        int o;
        int CPU_GPU;
        Rin = (Record *) malloc(sizeof(Record) * OP_rLen);
        Rin2 = (Record *) malloc(sizeof(Record) * OP_rLen);
        Rout = (Record *) malloc(sizeof(Record) * OP_rLen);
        RID = (Record *) malloc(sizeof(Record) * OP_rLen);
		CL_CREATE(&din,sizeof(Record) * OP_rLen);
		CL_CREATE(&din2,sizeof(Record) * OP_rLen);
		CL_CREATE(&dout1,sizeof(Record) * OP_rLen);
		CL_CREATE(&dout2,sizeof(Record) * OP_rLen);
		generateRand((Record *) Rin, TEST_MAX, OP_rLen, 0);
        generateRand((Record *) Rin2, TEST_MAX, OP_rLen, 1);
		CopyCPUToGPU(din,Rin,sizeof(Record) * OP_rLen);
		CopyCPUToGPU(din2,Rin2,sizeof(Record) * OP_rLen);
        generateRand(RID, TEST_MAX, OP_rLen, 1);
        for (o =0; o < 23; o++) {
            for (CPU_GPU = 0; CPU_GPU < 2; CPU_GPU++) {
                switch (o) {
                    case 0: {/*
                         * PROJECTION op
                         */
                        PROJECTION_HandShake(CPU_GPU);
                        break;
                    }
                    case 1: {
                       SELECTION_HandShake(CPU_GPU);
                        break;
                    }
                    case 2: {
						//	TYPE_AGGREGATION,//the default.
                        break;
                    }
                    case 3: {
                        AGG_SUM_HandShake(CPU_GPU);
                        break;
                    }
                    case 4: {
                        AGG_MAX_HandShake(CPU_GPU);
                        break;
                    }
                    case 5: {
                        AGG_MIN_HandShake(CPU_GPU);
                        break;
                    }
                    case 6: {
                        AGG_AVG_HandShake(CPU_GPU);
                        break;
                    }
                    case 7: {
                        break;
                    }
                    case 8: {
                        //AGG_SUM_AFTER_GROUP_BY_HandShake(CPU_GPU);
                        break;
                    }
                    case 9: {
                       //AGG_MAX_AFTER_GROUP_BY_HandShake(CPU_GPU);
                        break;
                    }
                    case 10: {
                        //AGG_AVG_AFTER_GROUP_BY_HandShake(CPU_GPU);
                        break;
                    }
                    case 11: {
                        break;
                    }
                    case 12: {
                        //AGG_MIN_AFTER_GROUP_BY_HandShake(CPU_GPU);
                        break;
                    }
                    case 13: {
                       //GROUP_BY_HandShake(CPU_GPU);
                        break;
                    }
                    case 14: {
                        ORDER_BY_HandShake(CPU_GPU);
                        break;
                    }
                    case 15: {
                        break;
                    }
                    case 16: {
                        break;
                    }
                    case 17: {
                        //JOIN_NINLJ_HandShake(CPU_GPU);
                        break;
                    }
                    case 18: {
                        JOIN_INLJ_HandShake(CPU_GPU);
                        break;
                    }
                    case 19: {
                        //JOIN_MJ_HandShake(CPU_GPU);
                        break;
                    }
                    case 20: {
                        JOIN_SMJ_HandShake(CPU_GPU);
                        break;
                    }
                    case 21: {
						JOIN_HJ_HandShake(CPU_GPU);
                        break;
                    }
                    case 22: {
                        break;
                    }
                }
            }
        }
        int i;
       
        double *sortSpeedUp;
        double *sortCPUBurden;
        double *sortGPUBurden;
        sortSpeedUp = (double *) malloc(sizeof(double) * 30);
        sortCPUBurden = (double *) malloc(sizeof(double) * 30);
        sortGPUBurden = (double *) malloc(sizeof(double) * 30);
        for (i = 0; i < 23; i++) {
            if (AddCPUBurden_OP[i] != 0) {
				fprintf(OP_ofp,"oc %d  %lf\n",i, AddCPUBurden_OP[i]);
				fprintf(OP_ofp,"og %d  %lf\n",i, AddGPUBurden_OP[i]);
                SpeedupGPUOverCPU_Operator[i] = AddCPUBurden_OP[i] / AddGPUBurden_OP[i];
                sortSpeedUp[counter] = SpeedupGPUOverCPU_Operator[i];
                sortCPUBurden[counter] = AddCPUBurden_OP[i];
                sortGPUBurden[counter] = AddGPUBurden_OP[i];
                counter++;//indicate how many non-zero data.
            }
        }
		fprintf(OP_ofp,"EN\n");
		
        choise(sortSpeedUp, counter);
        choise(sortCPUBurden, counter);
        choise(sortGPUBurden, counter);
        /*
         * set speed threshold
         */
        //OP_LothresholdForGPUApp=_f(sortSpeedUp,0,counter-1,(int)(counter*20/58));//->set lothreshold to be the first 20 max,0-20 treat as GPU faster
        OP_LothresholdForGPUApp = sortSpeedUp[(int) (counter * 40 / 58 + 1)];
        printf("OP_LothresholdForGPUApp is %lf\n", OP_LothresholdForGPUApp);
        //OP_LothresholdForCPUApp=_f(sortSpeedUp,0,counter-1,(int)(counter*25/58+1));//->set lothreshold to be the first 25 max,25-58 treat as CPU faster
        OP_LothresholdForCPUApp = sortSpeedUp[(int) (counter * 10 / 58)];
        printf("OP_LothresholdForCPUApp is %lf\n", OP_LothresholdForCPUApp);
        //20-25 as in middle.
		/*
         * set GPUburden threshold
         */
        //OP_LoGPUBurden=_f(sortGPUBurden,0,counter-1,(int)(counter*40/58+1));//->set lothreshold as first 40 max.
        OP_LoGPUBurden = (sortGPUBurden[0] +sortGPUBurden[(int) (counter * 10 / 58)])/2;
        printf("OP_LoGPUBurden is %lf\n", OP_LoGPUBurden);
        //OP_UpGPUBurden=_f(sortGPUBurden,0,counter-1,(int)(counter*10/58));//->set UPthreshold as first 10 max.
        OP_UpGPUBurden =( sortGPUBurden[(int) (counter * 40 / 58 + 1)]+ sortGPUBurden[(int) (counter * 50 / 58 + 1)])/2;
        printf("OP_UpGPUBurden is %lf\n", OP_UpGPUBurden);
        /*
         * set CPUburden threshold
         */
        //OP_LoCPUBurden=_f(sortCPUBurden,0,counter-1,(int)(counter*40/58+1));//->set lothreshold as first 40 max.
        OP_LoCPUBurden =  (sortCPUBurden[0] +sortCPUBurden[(int) (counter * 10 / 58)])/2; 
        printf("OP_LoCPUBurden is %lf\n", OP_LoCPUBurden);
        //OP_UpCPUBurden=_f(sortCPUBurden,0,counter-1,(int)(counter*10/58));//->set UPthreshold as first 10 max.
        OP_UpCPUBurden = (sortCPUBurden[(int) (counter * 40 / 58 + 1)]+ sortCPUBurden[(int) (counter * 50 / 58 + 1)])/2;
        printf("OP_UpCPUBurden is %lf\n", OP_UpCPUBurden);
		fclose(OP_ofp);
		CL_DESTORY(&din);
		CL_DESTORY(&din2);
		CL_DESTORY(&dout1);
		CL_DESTORY(&dout2);
		free(Rin);
		free(Rin2);
		free(Rout);
		free(RID);
    }
