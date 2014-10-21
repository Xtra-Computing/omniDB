#include "common.h"
#include "scheduler.h"
#include "KernelScheduler.h"
#include "OpenCL_DLL.h"
// OpenCL Vars---------0 for CPU, 1 for GPU
extern cl_context Context;        // OpenCL context
extern cl_program Program;           // OpenCL program
extern cl_command_queue CommandQueue[2];// OpenCL command que
extern cl_platform_id Platform[2];      // OpenCL platform
extern cl_device_id Device[2];          // OpenCL device
extern cl_ulong totalLocalMemory[2];      /**< Max local memory allowed */
extern cl_ulong totalGlobalMemory[2];      /**< Max global memory allowed */
extern cl_ulong usedtotalGlobalMemory[2];      /**< Max global memory used */
extern CRITICAL_SECTION CPUBurdenCS;
extern CRITICAL_SECTION GPUBurdenCS;
extern cl_device_id allDevices[10];
extern double GPUBurden;
extern double CPUBurden;
static int totalNumDevice=0;
extern double LoGPUBurden;
extern double LoCPUBurden;
extern double UpGPUBurden;
extern double UpCPUBurden;
cl_kernel Kernel[2];             // OpenCL kernel---------------->should been cancelled after all method update.
static int TAG_NO;
extern cl_ulong totalGlobalMemory[2];      /**< Max global memory allowed */
extern cl_ulong usedtotalGlobalMemory[2];      /**< Max global memory used */
#define APU
void bufferchecking(cl_mem R_in,size_t size)
{
	printf("checking size is %d\n",size);
	Record *R_out;
	R_out=(Record*)malloc(sizeof(Record)*size);
	CopyGPUToCPU(R_in,(void*)R_out,size);
	printf("checking: R_out [0].x:%d R_out[0].y:%d, size is %d\n",R_out->x,R_out->y,size);
	free(R_out);
}

void cl_init(cl_device_type TYPE)
{
	int CPU_GPU;
	if(TYPE==CL_DEVICE_TYPE_CPU)
		CPU_GPU=0;
	else
		CPU_GPU=1;
	cl_int ciErr1;
	//Get an OpenCL platform
	cl_uint numPlatform;
	cl_platform_id tempPlatform[10]; 
	ciErr1=clGetPlatformIDs( 0, NULL, &numPlatform );
    ciErr1 = clGetPlatformIDs(numPlatform, tempPlatform, NULL);
	cl_uint p=0;
	char buffer[100];
	size_t length=0;
	for(p=0;p<numPlatform;p++)
	{
		clGetPlatformInfo(tempPlatform[p],CL_PLATFORM_VENDOR,100,buffer,&length);
		if(TYPE==CL_DEVICE_TYPE_GPU){
		if(!strcmp(buffer, "Advanced Micro Devices, Inc."))//return 0 means equal
			Platform[CPU_GPU]=tempPlatform[p];
		}
		else if(!strcmp(buffer, "Advanced Micro Devices, Inc."))
			Platform[CPU_GPU]=tempPlatform[p];		
	}

	clGetPlatformInfo(Platform[CPU_GPU],CL_PLATFORM_VENDOR,100,buffer,&length);
	printf("%s, length, %d\n", buffer, length); 

    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clGetPlatformID, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
    //Get the number of devices
	cl_uint uiNumDevices;
	ciErr1 = clGetDeviceIDs(Platform[CPU_GPU], TYPE, 0, NULL, &uiNumDevices);
	
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clGetDeviceIDs, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	else
		printf("found: %d device\n", uiNumDevices);
	totalNumDevice=uiNumDevices;
	//get all the device
	ciErr1 = clGetDeviceIDs(Platform[CPU_GPU], TYPE, totalNumDevice, allDevices, NULL);
	
	
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in get all devices, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	//use the last one. =----->why??
	printf("all devices: ");
	for(int i=0;i<totalNumDevice;i++)
	{
		printf("D%d\t", allDevices[i]);
	}
	printf("\n");
	Device[CPU_GPU]=allDevices[totalNumDevice-1];    
	printf("Device[%d], %d\n",CPU_GPU, Device[CPU_GPU]);

	ciErr1 = clGetDeviceInfo(
        Device[CPU_GPU],
        CL_DEVICE_LOCAL_MEM_SIZE,
        sizeof(cl_ulong),
        (void *)&totalLocalMemory[CPU_GPU],
        NULL);
	printf("totalLocalMemory, %u\n", totalLocalMemory[CPU_GPU]);
	ciErr1 = clGetDeviceInfo(
        Device[CPU_GPU],
        CL_DEVICE_GLOBAL_MEM_SIZE,
        sizeof(cl_ulong),
        (void *)&totalGlobalMemory[CPU_GPU],
        NULL);
	printf("totalGlobalMemroy, %lu\n", totalGlobalMemory[CPU_GPU]);

	char* deviceName=NULL;
	size_t size = 0;
	ciErr1=clGetDeviceInfo( Device[CPU_GPU],
					CL_DEVICE_NAME,
					0,
					NULL,
					&size);
	deviceName=(char*)malloc(size);
	ciErr1=clGetDeviceInfo( Device[CPU_GPU],
					CL_DEVICE_NAME,
					size,
					deviceName,
					&size);
		printf("deviceName is %s \n",deviceName);
}
void cl_init_common ()
{
	cl_int ciErr1;
	//Create the context
    Context = clCreateContext(0, 2, Device, NULL, NULL, &ciErr1);
    //shrLog("clCreateContext...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clCreateContext, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	int CPU_GPU;
	for(CPU_GPU=0;CPU_GPU<2;CPU_GPU++){
		// Create a command-queue
		CommandQueue[CPU_GPU] = clCreateCommandQueue(Context, Device[CPU_GPU], 0, NULL);
		//shrLog("clCreateCommandQueue...\n"); 
		if (ciErr1 != CL_SUCCESS)
		{
			printf("Error in clCreateCommandQueue, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
			cl_clean(EXIT_FAILURE);
		}									
	}
}
int convertToString(const char *filename, std::string& s)
{
    size_t size;
    char*  str;

    std::fstream f(filename, (std::fstream::in | std::fstream::binary));

    if(f.is_open())
    {
        size_t fileSize;
        f.seekg(0, std::fstream::end);
        size = fileSize = (size_t)f.tellg();
        f.seekg(0, std::fstream::beg);

        str = new char[size+1];
        if(!str)
        {
            f.close();
            return NULL;
        }

        f.read(str, fileSize);
        f.close();
        str[size] = '\0';
    
        s = str;
        delete[] str;
        return 0;
    }
    printf("Error: Failed to open file %s\n", filename);
    return 1;
}
char *append(const char *orig, char c)
{
    size_t sz = strlen(orig);
    char *str = (char*)malloc(sz + 2);
    strcpy(str, orig);
    str[sz] = c;
    str[sz + 1] = '\0';
    return str;
}
char *append(char *orig, char* st)
{
	size_t sz = strlen(st);
	int i;
	for(i=0;i<sz;i++)
		orig=append(orig,st[i]);
	
	return orig;
}
void cl_prepareProgram(char* cSourceFile, char* dir)
{
	cSourceFile=append(dir,cSourceFile);
	std::cout << "Read:"<<cSourceFile<<"\n";
	cl_int ciErr1;
	//convert kernel file into string
    std::string  sourceStr;
    convertToString(cSourceFile, sourceStr);
    const char * source    = sourceStr.c_str();
    size_t sourceSize[]    = { strlen(source) };

	Program = ::clCreateProgramWithSource(
            Context, 1, &source, NULL, &ciErr1);
    // Create the program

    printf("clCreateProgramWithSource...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error %d in clCreateProgramWithSource, Line %u in file %s !!!\n\n", ciErr1,__LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }

    // Build the program with 'mad' Optimization option====>>>> what is "mad"
    #ifdef MAC
        char* flags = "-cl-fast-relaxed-math -DMAC -D WORKGROUP_SIZE=256";
    #else
        char* flags = "-cl-fast-relaxed-math";
    #endif
    ciErr1 = clBuildProgram(Program, 2, Device, flags, NULL, NULL);
    printf("clBuildProgram...\n");   
    if (ciErr1 == CL_BUILD_PROGRAM_FAILURE) {
    // Determine the size of the log
    size_t log_size;
    clGetProgramBuildInfo(Program, Device[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

    // Allocate memory for the log
    char *log = (char *) malloc(log_size);

    // Get the log
    clGetProgramBuildInfo(Program, Device[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

    // Print the log
    printf("%s\n", log);
	}
	else
	{
		printf("compilation success!\n");
	}
}
void cl_getKernel(char* kernelName,int CPU_GPU){};
void cl_getKernel(char* kernelName,cl_kernel *Kernel)
{
	cl_int ciErr1;
	(*Kernel) = clCreateKernel(Program, kernelName, &ciErr1);
    //shrLog("clCreateKernel (VectorAdd)...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error in clCreateKernel, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
}
void cl_getKernelByKernelFunction(char* kernelName,cl_kernel *Kernel)
{
	cl_int ciErr1;
	(*Kernel) = clCreateKernel(Program, kernelName, &ciErr1);
    //shrLog("clCreateKernel (VectorAdd)...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error % in clCreateKernel, Line %u in file %s !!!\n\n",ciErr1, __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
}
void CL_CREATE(cl_mem *mem,cl_int size)
{
	CL_MALLOC(mem,size);
}
void CL_DESTORY(cl_mem *mem)
{
	if(mem)clReleaseMemObject(*mem);
}
cl_int cl_malloc(cl_mem *mem, cl_mem_flags flag, cl_int size)
{
	cl_int ciErr1;
#ifdef APU
	*mem = clCreateBuffer(Context, CL_MEM_ALLOC_HOST_PTR |flag, size, NULL, &ciErr1);
#else
	*mem = clCreateBuffer(Context, flag, size, NULL, &ciErr1);
#endif
	return ciErr1;
}
void cl_readbuffer(void* to, cl_mem from, size_t size,int *index,cl_event *eventList,int *Flag_CPU_GPU,double * burden, int _CPU_GPU)
{
	cl_readbuffer(to, from, 0, size,index,eventList,Flag_CPU_GPU,burden,_CPU_GPU);
}
void cl_readbuffer(void* to, cl_mem from, size_t offset, size_t size,int *index,cl_event *eventList,int *Flag_CPU_GPU,double * burden, int _CPU_GPU)
{
	int preFlag=(*Flag_CPU_GPU);
	double preBurden=(*burden);
	int CPU_GPU=0;
	CPU_GPU=cl_readbufferscheduler(size,Flag_CPU_GPU,burden,_CPU_GPU);
	(*Flag_CPU_GPU)=CPU_GPU;
	cl_int ciErr1;
	if(*index!=0)
	{
		ciErr1 = clEnqueueReadBuffer(CommandQueue[CPU_GPU], from, CL_TRUE, offset, size, to,1, &eventList[(*index-1)%2], &eventList[(*index)%2]);
		deschedule(preFlag,preBurden); 
		
	}else
	ciErr1 = clEnqueueReadBuffer(CommandQueue[CPU_GPU], from, CL_TRUE, offset, size, to,0, NULL, &eventList[*index]);
	(*index)++;

		//clEnqueueWriteBuffer(CommandQueue[CPU_GPU], to, CL_FALSE, 0, size, from, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error %d in //cl_readbuffer, Line %u in file %s !!!\n\n", ciErr1,__LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	clFlush(CommandQueue[CPU_GPU]);
}

void cl_writebuffer(cl_mem to, void* from, size_t size,int *index,cl_event *eventList,int *Flag_CPU_GPU,double * burden, int _CPU_GPU)
{
	int preFlag=(*Flag_CPU_GPU);
	double preBurden=(*burden);
	int CPU_GPU=0;
	CPU_GPU=cl_writebufferscheduler(size,Flag_CPU_GPU,burden,_CPU_GPU);
	cl_int ciErr1;
	(*Flag_CPU_GPU)=CPU_GPU;
	if(*index!=0)
		{
		ciErr1 = clEnqueueWriteBuffer(CommandQueue[CPU_GPU], to, CL_FALSE, 0, size, from,1, &eventList[((*index)-1)%2], &eventList[(*index)%2]);
		deschedule(preFlag,preBurden);
	}
	else
	ciErr1 = clEnqueueWriteBuffer(CommandQueue[CPU_GPU], to, CL_FALSE, 0, size, from,0, NULL, &eventList[*index]);
	(*index)++;

    if (ciErr1 != CL_SUCCESS)
    {
        printf("ciErr1 is %d, Error in clEnqueueWriteBuffer, Line %u in file %s !!!\n\n",ciErr1, __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	clFlush(CommandQueue[CPU_GPU]);
}
void cl_copyBuffer(cl_mem dest, cl_mem src, size_t size,int *index,cl_event *eventList,int *Flag_CPU_GPU,double * burden, int _CPU_GPU)
{	
	int preFlag=(*Flag_CPU_GPU);
	double preBurden=(*burden);
	int CPU_GPU=0;
	CPU_GPU=cl_copyBufferscheduler(size,Flag_CPU_GPU,burden,_CPU_GPU);
	cl_int ciErr1;
	(*Flag_CPU_GPU)=CPU_GPU;
	if(*index!=0)
	{
		ciErr1 = clEnqueueCopyBuffer(CommandQueue[CPU_GPU], src, dest, 0, 0, size, 1, &eventList[((*index)-1)%2], &eventList[(*index)%2]);
		deschedule(preFlag,preBurden);
	}
	else
	ciErr1 = clEnqueueCopyBuffer(CommandQueue[CPU_GPU], src, dest, 0, 0, size, 0, NULL, &eventList[*index]);


	(*index)++;
    if (ciErr1 != CL_SUCCESS)
    {
        printf(" Error %d, in //cl_copyBuffer, Line %u in file %s !!!\n\n",ciErr1, __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	clFlush(CommandQueue[CPU_GPU]);	
}
void cl_copyBuffer(cl_mem dest, int destOffset, cl_mem src, size_t size,int *index,cl_event *eventList,int *Flag_CPU_GPU,double * burden, int _CPU_GPU)
{
	int preFlag=(*Flag_CPU_GPU);
	double preBurden=(*burden);
	int CPU_GPU=0;
	CPU_GPU=cl_copyBufferscheduler(size,Flag_CPU_GPU,burden,_CPU_GPU);
	CPU_GPU=_CPU_GPU;
	cl_int ciErr1;
	(*Flag_CPU_GPU)=CPU_GPU;
	if(*index!=0)
	{
		ciErr1 = clEnqueueCopyBuffer(CommandQueue[CPU_GPU], src, dest, 0, destOffset, size,1, &eventList[((*index)-1)%2], &eventList[(*index)%2]);
		deschedule(preFlag,preBurden);
	}
	else
	ciErr1 = clEnqueueCopyBuffer(CommandQueue[CPU_GPU], src, dest, 0, destOffset, size,0, NULL, &eventList[*index]);

	
	(*index)++;
		//clEnqueueWriteBuffer(CommandQueue[CPU_GPU], to, CL_FALSE, 0, size, from, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error %d in cl_copyBuffer, Line %u in file %s !!!\n\n",ciErr1, __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	clFlush(CommandQueue[CPU_GPU]);	
}
void cl_copyBuffer(cl_mem dest, int destOffset, cl_mem src, int srcOffset, size_t size,int *index,cl_event *eventList,int *Flag_CPU_GPU,double * burden, int _CPU_GPU)
{
	int preFlag=(*Flag_CPU_GPU);
	double preBurden=(*burden);
	int CPU_GPU=0;
	CPU_GPU=cl_copyBufferscheduler(size,Flag_CPU_GPU,burden,_CPU_GPU);
	cl_int ciErr1;
	(*Flag_CPU_GPU)=CPU_GPU;
	if(*index!=0)
	{
		ciErr1 = clEnqueueCopyBuffer(CommandQueue[CPU_GPU], src, dest, srcOffset, destOffset, size, 1, &eventList[((*index)-1)%2], &eventList[(*index)%2]);		
		deschedule(preFlag,preBurden);
	}
	else
	ciErr1 = clEnqueueCopyBuffer(CommandQueue[CPU_GPU], src, dest, srcOffset, destOffset, size, 0, NULL, &eventList[*index]);

	
	(*index)++;
		//clEnqueueWriteBuffer(CommandQueue[CPU_GPU], to, CL_FALSE, 0, size, from, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error %d in cl_copyBuffer, Line %u in file %s !!!\n\n", ciErr1,__LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	clFlush(CommandQueue[CPU_GPU]);	
}
void cl_clean (int iExitCode)
{
    // Cleanup allocated objects
    printf("Starting Cleanup...\n\n");
	int CPU_GPU;
	for(CPU_GPU=0;CPU_GPU<2;CPU_GPU++){
		if(CommandQueue[CPU_GPU])clReleaseCommandQueue(CommandQueue[CPU_GPU]);
	}
	if(Program)clReleaseProgram(Program);		
	if(Context)clReleaseContext(Context);
	exit(0);
}
//new 

int floorPow2(int n)
{
    int exp;
    frexp((float)n, &exp);
    return 1 << (exp - 1);
}
/*original*/
void cl_readbuffer(void* to, cl_mem from,size_t size,int CPU_GPU)
{
	cl_int ciErr1;
	//bufferchecking(from,size);
	ciErr1 = clEnqueueReadBuffer(CommandQueue[CPU_GPU], from, CL_TRUE, NULL, size, to, 0, NULL, NULL);
		//clEnqueueWriteBuffer(CommandQueue[CPU_GPU], to, CL_FALSE, 0, size, from, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error %d in cl_readbuffer, Line %u in file %s !!!\n\n",ciErr1, __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	clFinish(CommandQueue[CPU_GPU]);
}
void CopyCPUToGPU(cl_mem to, void* from, size_t size)
{
	cl_writebuffer(to,from,size,0);
}
void CopyGPUToGPU(cl_mem from, cl_mem to, size_t size)
{
	cl_copyBuffer(to,from,size,0);
}
		
void CopyGPUToCPU(cl_mem from, void* to, size_t size)
{
	cl_readbuffer(to,from,size,0);
}
void cl_writebuffer(cl_mem to, void* from, size_t size,int CPU_GPU)
{
	cl_int ciErr1;
	ciErr1 = clEnqueueWriteBuffer(CommandQueue[CPU_GPU], to, CL_FALSE, 0, size, from, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error  %d in clEnqueueWriteBuffer, Line %u in file %s !!!\n\n",ciErr1, __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	clFinish(CommandQueue[CPU_GPU]);
}
void cl_copyBuffer(cl_mem dest, cl_mem src, size_t size,int CPU_GPU)
{
	cl_int ciErr1;
	ciErr1 = clEnqueueCopyBuffer(CommandQueue[CPU_GPU], src, dest, 0, 0, size, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error %d in cl_copyBuffer, Line %u in file %s !!!\n\n",ciErr1, __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	clFinish(CommandQueue[CPU_GPU]);	
}
void cl_launchKernel(cl_uint work_dim, const size_t *groups, size_t *threads,cl_kernel *Kernel,int CPU_GPU)
{
	if(work_dim==1){
		//printf("dim 1: G%d T%d\n", groups[0], threads[0]);
		if(CPU_GPU&&threads[0]>256){
			threads[0]=256;
			//printf("!!!exceed GPU limit:max work item is 256!");
		}
	}else
		if(work_dim==2)
		printf("dim 2: G%d, %d T%d,%d\n", groups[0], groups[1], threads[0], threads[1]);
	
	cl_int ciErr1 = clEnqueueNDRangeKernel(CommandQueue[CPU_GPU], (*Kernel), work_dim, NULL, groups, threads, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        printf("Error %d in clEnqueueNDRangeKernel, Line %u in file %s !!!\n\n", ciErr1, __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
	ciErr1=clFinish(CommandQueue[CPU_GPU]);
	if (ciErr1 != CL_SUCCESS)
    {
        printf("Error %d in clEnqueueNDRangeKernel, Line %u in file %s !!!\n\n", ciErr1, __LINE__, __FILE__);
        cl_clean(EXIT_FAILURE);
    }
}
void wait(int index,cl_event *eventList){
	printf("index of %d Going to wait!\n",index);
	cl_int err=clWaitForEvents(1,&eventList[(index-1)%2]);
	printf("index of %d Finish wait! err is %d\n,",index,err);
}