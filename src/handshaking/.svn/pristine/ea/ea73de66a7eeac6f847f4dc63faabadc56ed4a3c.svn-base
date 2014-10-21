#include "common.h"
#ifndef _ScanPara 
#define _ScanPara
struct ScanPara{
cl_mem              *outputBuffer;          /**< Array of output buffers */
cl_mem              *blockSumBuffer;        /**< Array of block sum buffers */
cl_mem              tempBuffer;             /**< Temporary bufer */

cl_uint             pass;                   /**< Number of passes */
cl_uint				gLength;
cl_uint             blockSize;              /**< Size of a block */
};
#endif
void testScanImpl(int rLen);
void scanImpl(cl_mem d_Src, int rLen, cl_mem d_Dst,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,ScanPara* SP,int _CPU_GPU);
void scanImpl(cl_mem d_Src, int offset, int rLen, cl_mem d_Dst,int *index,cl_event *eventList,cl_kernel *kernel,int *Flag_CPU_GPU,double * burden,ScanPara* SP,int _CPU_GPU);
void closeScan(ScanPara* SP);
void initScan(int rLen,ScanPara* SP);





/*additional*/
void bScan_int(cl_uint len, 
                       cl_mem *inputBuffer, 
                       cl_mem *outputBuffer,
                       cl_mem *blockSumBuffer,
					   int *index,
					   cl_event *eventList,cl_kernel *Kernel,int *Flag_CPU_GPU,double * burden,ScanPara* SP,int _CPU_GPU);
