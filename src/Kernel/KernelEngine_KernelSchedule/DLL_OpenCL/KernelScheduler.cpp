#include "MyThreadPoolCop.h"
#include "common.h"
#include "KernelScheduler.h"
#include "scheduler.h"
#include "Helper.h"
/////////////////////////////////////////////////////
extern cl_context Context;        // OpenCL context
extern cl_program Program;           // OpenCL program
extern cl_command_queue CommandQueue[2];// OpenCL command que
extern cl_platform_id Platform[2];      // OpenCL platform
extern cl_device_id Device[2];          // OpenCL device
extern cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
extern cl_device_id allDevices[10];
extern double AddGPUBurden_Copy;//->initial in handshaking. fix rLen to 1024*1024
extern double AddCPUBurden_Copy;
extern double AddGPUBurden_Read;//->initial in handshaking. fix rLen to 1024*1024
extern double AddCPUBurden_Read;
extern double AddGPUBurden_Write;//->initial in handshaking. fix rLen to 1024*1024
extern double AddCPUBurden_Write;
extern double AddGPUBurden[58];//->initial in handshaking. fix rLen to 1024*1024
extern double AddCPUBurden[58];//->initial in handshaking. fix rLen to 1024*1024
extern double speedupGPUoverCPU[58];
extern double  LothresholdForGPUApp;
extern double  LothresholdForCPUApp;
extern double LoGPUBurden;
extern double LoCPUBurden;
extern double UpGPUBurden;
extern double UpCPUBurden;
extern double GPUBurden;
extern double CPUBurden;
extern int rLen;
extern int pLen;
double base=rLen;
extern CRITICAL_SECTION CPUBurdenCS;
extern CRITICAL_SECTION GPUBurdenCS;
extern int global_KernelSchedule;

struct request_handler {
    struct request_handler *next, *prev;
	long index;
};

static struct request_handler *ListHead;
static struct request_handler *WTI; 
static struct request_handler *CTI;

static struct request_handler *testPointer;
static struct request_handler staticRH[64];

static inline void __list_add(struct request_handler *newp,
                             struct request_handler *prev,
                              struct request_handler *next)
{
       next->prev = newp;
       newp->next = next;
       newp->prev = prev;
       prev->next = newp;
}
static inline void list_add_tail(struct request_handler *newp, struct request_handler *head)
 {
        __list_add(newp, head->prev, head);
}

void kernel_initial()
{
		/*Prepare head*/
		ListHead=&staticRH[0];
		ListHead->next=ListHead;
		ListHead->prev=ListHead;
		ListHead->index=0;

		/*Create internal connection*/
		testPointer=ListHead;
		int i;
		for(i=1;i<MaxTag;i++){
			list_add_tail(&staticRH[i], ListHead);
			testPointer=testPointer->next;
			testPointer->index=i;
		}
		WTI=CTI=ListHead;
}

void kernel_enqueue(int size,int kid,cl_uint work_dim, const size_t *groups, size_t *threads,cl_event *List,int* index,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{

	int preFlag=(*Flag_CPU_GPU);
	double preBurden=(*burden);
	int CPU_GPU=0;

#if 0	
	printf("kid is %d\n",kid);
	printf("speedupGPUoverCPU is %lf, LothresholdForGPUApp is %lf,LothresholdForCPUApp is %lf\n",speedupGPUoverCPU[kid],LothresholdForGPUApp,LothresholdForCPUApp);
	
	
	printf("LoGPUBurden is %lf,UpGPUBurden is %lf,LoCPUBurden is %lf,UpCPUBurden is %lf\n",LoGPUBurden,UpGPUBurden,LoCPUBurden,UpCPUBurden);
	
	printf("CPUBurden is %lf,,GPUBurden is %lf\n",CPUBurden,GPUBurden);

#endif
	CPU_GPU=Kernelscheduler(size,kid,Flag_CPU_GPU,burden,_CPU_GPU);		
		(*Flag_CPU_GPU)=CPU_GPU;
		cl_int ciErr1;
		if(CPU_GPU&&threads[0]>256){
			//printf("!!!exceed GPU limit:max work item is 256!\n");
			threads[0]=256;
		}
		if((*index)!=0)
		{
			ciErr1 = clEnqueueNDRangeKernel(CommandQueue[CPU_GPU], (*kernel), work_dim, NULL, groups, threads, 1, &List[((*index)-1)%2], &(List[(*index)%2]));
			deschedule(preFlag,preBurden);
	}
		else
		ciErr1 = clEnqueueNDRangeKernel(CommandQueue[CPU_GPU], (*kernel), work_dim, NULL, groups, threads, 0, NULL, &(List[*index]));
		
		(*index)++;
		if (ciErr1 != CL_SUCCESS)
		{
			printf("Error %d in clEnqueueNDRangeKernel, Line %u in file %s !!!\n\n", ciErr1, __LINE__, __FILE__);
			cl_clean(EXIT_FAILURE);
		}
		ciErr1=clFlush(CommandQueue[CPU_GPU]);
		if (ciErr1 != CL_SUCCESS)
		{
			printf("Error %d in clEnqueueNDRangeKernel, Line %u in file %s !!!\n\n", ciErr1, __LINE__, __FILE__);
			cl_clean(EXIT_FAILURE);
		}	

}

void kernel_clean()
{
	
}