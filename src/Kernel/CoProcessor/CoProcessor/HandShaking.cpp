#include "Database.h"
#include "QueryPlanTree.h"
#include "CoProcessorTest.h"
#include "Helper.h"
extern double evalautedQuery;
extern double Query_UpCPUBurden;
extern double Query_LoCPUBurden;
extern double Query_UpGPUBurden;
extern double Query_LoGPUBurden;
extern double Query_CPUBurden;
extern double Query_GPUBurden;
extern double Query_LothresholdForGPUApp;
extern double Query_LothresholdForCPUApp;
extern double Query_SpeedupGPUOverCPU[12];
extern double RunInCPU[12];
extern double RunInGPU[12];
Record* Rin;
Record* Rin2;
Record* Rout;
Record* Rout2;//empty Rout
Record* RID;
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

void queryEXE(QUERY_TYPE qT,EXEC_MODE CPU_GPU){
	setHighPriority();
	char *query=new char[500];
	makeTestQuery(qT,query);
	QueryPlanTree tree;//set GPUONLY_QP always false. Assume CoProcessor can only see host data. so directly cancell it.
	tree.buildTree(query);
	tree.execute(CPU_GPU);
	free(tree.planStatus->finalResult);
	//printf("tree execute finish!\n");
//	tree.~QueryPlanTree();
}
void queryTest(QUERY_TYPE qt, EXEC_MODE CPU_GPU,int qid){
	double counter=1;
	double sum=0;
	double scale=1000;
	int i;
	int timer=genTimer(1);
	for(i=0;i<counter;i++){		
		getTimer(timer);
		queryEXE(qt,CPU_GPU);
		double t=getTimer(timer);
		sum+=t;
		}
	//printf("----------------Hand Shake Result of qid %d\n\n",qid);
	//printf("CPU_GPU:%d, QID:%d time spend is:%lf\n\n\n",CPU_GPU,qid,sum/counter);
	if(CPU_GPU){
		RunInGPU[qt]=sum/counter*scale;
	}
	else{
		RunInCPU[qt]=sum/counter*scale;
	}
}