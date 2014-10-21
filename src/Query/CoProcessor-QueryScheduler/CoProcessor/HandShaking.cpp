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
extern int numQueries;
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
void Query_handShaking()
{
	FILE *Query_ofp;
	char outputFilename[50];
	sprintf(outputFilename, "./Output/QuerySpecification.list");
	Query_ofp = fopen(outputFilename, "w");
	int qid;
	QUERY_TYPE qt;
	initDB2("RS.conf",TEST_MAX);
	for(qid=0;qid<12;qid++){	
			switch(qid){
				case 0:{
						//_ASSERTE( _CrtCheckMemory( ) );
						//initDB2("RS.conf",TEST_SMALL);
						//qt=Q_POINT_SELECTION;
						//queryTest(qt,EXEC_CPU,qid);
						//queryTest(qt,EXEC_GPU,qid);
						//	_ASSERTE( _CrtCheckMemory( ) );
						break;
					   }
				case 1:{
						_ASSERTE( _CrtCheckMemory( ) );
						qt=Q_RANGE_SELECTION;
						queryTest(qt,EXEC_CPU,qid);
						queryTest(qt,EXEC_GPU,qid);
							_ASSERTE( _CrtCheckMemory( ) );
						break;
					   }
				case 2:{
						_ASSERTE( _CrtCheckMemory( ) );
						qt=Q_AGG;
						queryTest(qt,EXEC_CPU,qid);
						queryTest(qt,EXEC_GPU,qid);
							_ASSERTE( _CrtCheckMemory( ) );
						break;
					   }
				case 3:{
						qt=Q_ORDERBY;
						queryTest(qt,EXEC_CPU,qid);
						queryTest(qt,EXEC_GPU,qid);
						break;
					   }
				 case 4:{
						//qt=Q_AGG_GROUPBY;
						//queryTest(qt,EXEC_CPU,qid);
						//queryTest(qt,EXEC_GPU,qid);
						break;
					   }
				 case 5:{
						//qt=Q_NINLJ;
						//queryTest(qt,EXEC_CPU,qid);
						//queryTest(qt,EXEC_GPU,qid);
						break;
					   }
				case 6:{
						qt=Q_INLJ;
						queryTest(qt,EXEC_CPU,qid);
						queryTest(qt,EXEC_GPU,qid);
						break;
					   }
				case 7:{
						qt=Q_SMJ;
						queryTest(qt,EXEC_CPU,qid);
						queryTest(qt,EXEC_GPU,qid);
						break;
					   }
				case 8:{
						qt=Q_HJ;
						queryTest(qt,EXEC_CPU,qid);
						queryTest(qt,EXEC_GPU,qid);
						break;
					   }
				case 9:{
						//qt=Q_DBMBENCH1;
						//queryTest(qt,EXEC_CPU,qid);
						//queryTest(qt,EXEC_GPU,qid);
						break;
					   }
				case 10:{
						//qt=Q_DBMBENCH2;
						//queryTest(qt,EXEC_CPU,qid);
						//queryTest(qt,EXEC_GPU,qid);
						break;
					   }
				case 11:{
						//qt=Q_DBMBENCH3;
						//queryTest(qt,EXEC_CPU,qid);
						//queryTest(qt,EXEC_GPU,qid);
						break;
					   }			
			}//end of switch
	}//end of outer for loop
	int i;
	int counter=0;
	double *sortSpeedUp;
	double *sortCPUBurden;
	double *sortGPUBurden;
	sortSpeedUp=(double *)malloc(sizeof(double)*60 );
	sortCPUBurden=(double *)malloc(sizeof(double)*60 );
	sortGPUBurden=(double *)malloc(sizeof(double)*60 );
	for(i=0;i<12;i++){
		if(RunInCPU[i]!=0){
			Query_SpeedupGPUOverCPU[i]=RunInCPU[i]/RunInGPU[i];
			fprintf(Query_ofp,"Query_SpeedupGPUOverCPU %d is %lf\n",i,Query_SpeedupGPUOverCPU[i]);
			fprintf(Query_ofp,"RunInCPU %d is %lf\n",i,RunInCPU[i]);
			fprintf(Query_ofp,"RunInGPU %d is %lf\n",i,RunInGPU[i]);
			sortSpeedUp[counter]=Query_SpeedupGPUOverCPU[i];
			sortCPUBurden[counter]=RunInCPU[i];
			sortGPUBurden[counter]=RunInGPU[i];
			counter++;//indicate how many non-zero data.
		}
	}
	choise(sortSpeedUp,counter);
	choise(sortCPUBurden,counter);
	choise(sortGPUBurden,counter);
	/*set speed threshold*/
	//Query_LothresholdForGPUApp=_f(sortSpeedUp,0,counter-1,(int)(counter*20/58));//->set lothreshold to be the first 20 max,0-20 treat as GPU faster
	Query_LothresholdForGPUApp=sortSpeedUp[(int)(counter*35/58+1)];
	fprintf(Query_ofp,"Query_LothresholdForGPUApp is %lf\n",Query_LothresholdForGPUApp);
	//Query_LothresholdForCPUApp=_f(sortSpeedUp,0,counter-1,(int)(counter*25/58+1));//->set lothreshold to be the first 25 max,25-58 treat as CPU faster
	Query_LothresholdForCPUApp=sortSpeedUp[(int)(counter*25/58)];
	fprintf(Query_ofp,"Query_LothresholdForCPUApp is %lf\n",Query_LothresholdForCPUApp);
	//20-25 as in middle.
	/*set GPUburden threshold*/
	//Query_LoGPUBurden=_f(sortGPUBurden,0,counter-1,(int)(counter*40/58+1));//->set lothreshold as first 40 max.
	Query_LoGPUBurden=(sortGPUBurden[0]+sortGPUBurden[(int)(counter*30/58)])/2;
	//printf("Query_LoGPUBurden is %lf\n",Query_LoGPUBurden);
	//Query_UpGPUBurden=_f(sortGPUBurden,0,counter-1,(int)(counter*10/58));//->set UPthreshold as first 10 max.
	Query_UpGPUBurden=(sortGPUBurden[(int)(counter*35/58+1)]+sortGPUBurden[(int)(counter*45/58+1)])/2;
	fprintf(Query_ofp,"Query_UpGPUBurden is %lf\n",Query_UpGPUBurden);
	/*set CPUburden threshold*/
	Query_LoCPUBurden=(sortCPUBurden[0]+sortCPUBurden[(int)(counter*30/58)])/2;
	fprintf(Query_ofp,"Query_LoCPUBurden is %lf\n",Query_LoCPUBurden);
	
	Query_UpCPUBurden=(sortCPUBurden[(int)(counter*35/58+1)]+sortCPUBurden[(int)(counter*45/58+1)])/2;
	fprintf(Query_ofp,"Query_UpCPUBurden is %lf\n",Query_UpCPUBurden);

	int w;
	for(w=0;w<counter;w++){
		fprintf(Query_ofp,"sortSpeedUp%d is %lf\n",w,sortSpeedUp[w]);
	}
	fprintf(Query_ofp,"----------------------------------\n\n");
	for(w=0;w<counter;w++){
		fprintf(Query_ofp,"sortCPUBurden%d is %lf\n",w,sortCPUBurden[w]);
	}
	fprintf(Query_ofp,"----------------------------------\n\n");
	for(w=0;w<counter;w++){
		fprintf(Query_ofp,"sortGPUBurden%d is %lf\n",w,sortGPUBurden[w]);
	}
	fprintf(Query_ofp,"Final decision\n\n");
	fprintf(Query_ofp,"Query_LothresholdForGPUApp is %lf\n",Query_LothresholdForGPUApp);
	fprintf(Query_ofp,"Query_LothresholdForCPUApp is %lf\n",Query_LothresholdForCPUApp);
	fprintf(Query_ofp,"Query_LoGPUBurden is %lf\n",Query_LoGPUBurden);
	fprintf(Query_ofp,"Query_UpGPUBurden is %lf\n",Query_UpGPUBurden);
	fprintf(Query_ofp,"Query_LoCPUBurden is %lf\n",Query_LoCPUBurden);
	fprintf(Query_ofp,"Query_UpCPUBurden is %lf\n",Query_UpCPUBurden);
	fclose(Query_ofp);

}