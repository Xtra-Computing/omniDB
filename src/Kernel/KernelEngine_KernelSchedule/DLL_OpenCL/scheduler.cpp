#include "scheduler.h"
#include "common.h"
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
extern int KERNELSCHEDULE;
extern int GPUALWAYS;
extern int CPUALWAYS;
extern double GPUBurden;
extern double CPUBurden; 
extern double base;
extern int scheduler_index;
extern int descheduler_index;
extern int cpuburden_index;
extern int gpuburden_index;
extern float cpuburden[100000];
extern float gpuburden[100000];
extern CRITICAL_SECTION CPUBurdenCS;
extern CRITICAL_SECTION GPUBurdenCS;
extern CRITICAL_SECTION schedulerflag;
extern CRITICAL_SECTION deschedulerflag;

/*kernel operation can be either greedy or adaptive*/
//#define Adaptive
#define Greedy

/*buffer operation is by default greedy*/
//#define Continuous
extern int global_KernelSchedule;
double inline getAddBurden_Copy(const int* Flag_CPU_GPU,double size){
	if((*Flag_CPU_GPU)) return AddGPUBurden_Copy/base*size;
	else return AddCPUBurden_Copy/base*size;
}

double inline getAddBurden_Read(const int* Flag_CPU_GPU,double size){
		if((*Flag_CPU_GPU)) return AddGPUBurden_Read/base*size;
	else return AddCPUBurden_Read/base*size;
}
double inline getAddBurden_Write(const int* Flag_CPU_GPU,double size){
	if((*Flag_CPU_GPU)) return AddGPUBurden_Write/base*size;
	else return AddCPUBurden_Write/base*size;
}
double inline getAddGPUBurden(const int kid,double size){
	return AddGPUBurden[kid]/base*size;
}
double inline getAddCPUBurden(const int kid,double size){
	return AddCPUBurden[kid]/base*size;
}
void inline GPUBurdenINC(const double *burden){
		EnterCriticalSection(&(GPUBurdenCS));	
		GPUBurden+=(*burden);	
		LeaveCriticalSection(&(GPUBurdenCS));	
		//recordUpdate(GPUBurden,CPUBurden);
}
void inline CPUBurdenINC(const double *burden){
		EnterCriticalSection(&(CPUBurdenCS));		
		CPUBurden+=(*burden);
		LeaveCriticalSection(&(CPUBurdenCS));
		//recordUpdate(GPUBurden,CPUBurden);
}


int Kernelscheduler(int size,int kid,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int CPU_GPU=0;
if(global_KernelSchedule){
#ifdef Greedy
			if((CPUBurden+getAddCPUBurden(kid,size))<(GPUBurden+getAddGPUBurden(kid,size)))
			{
				CPU_GPU=0;
				(*burden)=getAddCPUBurden(kid,size);
				CPUBurdenINC(burden);
				//_tonyPrint_("KernelScheduler: case 2.1\n");
			}else {
				CPU_GPU=1;
				(*burden)=getAddGPUBurden(kid,size);
				GPUBurdenINC(burden);
				//_tonyPrint_("KernelScheduler: case 3.1\n");
			}
#else
#ifdef Adaptive
			if(CPUBurden<GPUBurden){
				CPU_GPU=0;
				(*burden)=getAddCPUBurden(kid,size);
				CPUBurdenINC(burden);
				//_tonyPrint_("KernelScheduler: case 2.1\n");
			}else {
				CPU_GPU=1;
				(*burden)=getAddGPUBurden(kid,size);
				GPUBurdenINC(burden);
				//_tonyPrint_("KernelScheduler: case 3.1\n");
			}

#else //KERNEL-FIFO
		if(*Flag_CPU_GPU)//previously GPU
			CPU_GPU=0;
		else CPU_GPU=1;//previously CPU

		if(CPU_GPU){
		(*burden)=getAddGPUBurden(kid,size);
		GPUBurdenINC(burden);
		}else{
		(*burden)=getAddCPUBurden(kid,size);
		CPUBurdenINC(burden);
		}
#endif

#endif
}else{//this part is used by O and Q schedule
	CPU_GPU=_CPU_GPU;
	if(CPU_GPU){
		(*burden)=getAddGPUBurden(kid,size);
		GPUBurdenINC(burden);
	}else{
		(*burden)=getAddCPUBurden(kid,size);
		CPUBurdenINC(burden);
	}
}
	return CPU_GPU;		
}
void deschedule(const int preFlag, const double preBurden)
{
	//printf("GPUBurden is %lf,CPUBurden is %lf\n",GPUBurden,CPUBurden);
		if(preFlag)//GPU
			{
				//printf("DEC GPUBurden is %lf,preBurden is %lf\n",GPUBurden,preBurden);
				EnterCriticalSection(&(GPUBurdenCS));		
				GPUBurden-=preBurden;
				LeaveCriticalSection(&(GPUBurdenCS));
				//recordUpdate(GPUBurden,CPUBurden);
			}
			else
			{
				//printf("DEC CPUBurden is %lf,preBurden is %lf\n",CPUBurden,preBurden);
				EnterCriticalSection(&(CPUBurdenCS));
				CPUBurden-=preBurden;
				LeaveCriticalSection(&(CPUBurdenCS));
				//recordUpdate(GPUBurden,CPUBurden);
			}
}

int cl_readbufferscheduler(int size,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int CPU_GPU;
	(*burden)=getAddBurden_Read(Flag_CPU_GPU,size);
	if(global_KernelSchedule){
#ifdef Continuous
		if(*Flag_CPU_GPU){
			CPU_GPU=1;
			GPUBurdenINC(burden);
		}else
		{CPU_GPU=0;
		CPUBurdenINC(burden);
		}

#else
		/*read is GPU favor*/	
		if(GPUBurden<CPUBurden)
		{
			CPU_GPU=1;
			GPUBurdenINC(burden);
			//_tonyPrint_("readbufferscheduler: case 1\n");
		}else if(GPUBurden>UpGPUBurden)
		{
			CPU_GPU=0;	
			CPUBurdenINC(burden);
			//_tonyPrint_("readbufferscheduler: case 2\n");
		}else 
		{
			CPU_GPU=1;
			GPUBurdenINC(burden);
			//_tonyPrint_("readbufferscheduler: case 3\n");
		}
#endif
}else{
	CPU_GPU=_CPU_GPU;
	if(CPU_GPU){
		GPUBurdenINC(burden);
	}else{
		CPUBurdenINC(burden);
	}
}
		return CPU_GPU;
}
int cl_writebufferscheduler(int size,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int CPU_GPU;
		(*burden)=getAddBurden_Write(Flag_CPU_GPU,size);
if(global_KernelSchedule){
#ifdef Continuous
	if(*Flag_CPU_GPU){
		CPU_GPU=1;
		GPUBurdenINC(burden);
	}else
	{CPU_GPU=0;
	CPUBurdenINC(burden);
	}

#else
	/*WRITE is CPU favor*/
	if(CPUBurden<GPUBurden)
	{	
		CPU_GPU=0;
		CPUBurdenINC(burden);
		//_tonyPrint_("writebufferscheduler: case 1\n");
	}else if(CPUBurden>UpCPUBurden)
	{
		CPU_GPU=1;
		GPUBurdenINC(burden);
		//_tonyPrint_("writebufferscheduler: case 2\n");
	}else
	{	
		CPU_GPU=0;
		CPUBurdenINC(burden);
		//_tonyPrint_("writebufferscheduler: case 3\n");
	}	
#endif
}
else{
		CPU_GPU=_CPU_GPU;
		if(CPU_GPU){
			GPUBurdenINC(burden);
		}else{
			CPUBurdenINC(burden);
		}
}
		return CPU_GPU;
}
int cl_copyBufferscheduler(int size,int *Flag_CPU_GPU,double * burden,int _CPU_GPU)
{
	int CPU_GPU;
	(*burden)=getAddBurden_Copy(Flag_CPU_GPU,size);
if(global_KernelSchedule){
#ifdef Continuous
	if(*Flag_CPU_GPU){
		CPU_GPU=1;
		GPUBurdenINC(burden);
	}else
	{CPU_GPU=0;
		CPUBurdenINC(burden);
	}

#else
		if(CPUBurden<GPUBurden)
		{	
			CPU_GPU=0;
			CPUBurdenINC(burden);
			//_tonyPrint_("copyBufferscheduler: case 1\n");
		}
		else
		{	
			CPU_GPU=1;
			GPUBurdenINC(burden);
			//_tonyPrint_("copyBufferscheduler: case 2\n");
		}
#endif
}else{
		CPU_GPU=_CPU_GPU;
		if(CPU_GPU){
			GPUBurdenINC(burden);
		}else{
			CPUBurdenINC(burden);
		}
}
		return CPU_GPU;
}