#include "common.h"
void AddGPUBurden_Copy_handshake();
void AddCPUBurden_Copy_handshake();
void AddGPUBurden_Read_handshake();
void AddCPUBurden_Read_handshake();
void AddGPUBurden_Write_handshake();
void AddCPUBurden_Write_handshake();
void Project_map_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);
void getResult_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);

void copyLastElement_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void perscanFirstPass_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void perscan_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);

void filterImpl_map_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);
void filterImpl_write_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);

void optScatter_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);
void optGather_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);

void partition_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);
void mapPart_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);
void countHist_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);
void writeHist_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);
void getBound_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);
void BitonicSort_kernel_handshake(int HandShakeCPU_GPU,cl_kernel *HandShakeKernel);
void memset_int_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void mapImpl_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);

void blockAddition_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void prefixSum_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void ScanLargeArrays_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void gpuNLJ_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void nlj_write_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);

void gSearchTree_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void gIndexJoin_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void gJoinWithWrite_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void gCreateIndex_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);

void gCreateIndex_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);


void quanMap_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void gSearchTree_usingKeys_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void joinMBCount_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void joinMBWrite_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);


void build_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
void probe_kernel_handshake(int HandShakeCPU_GPU, cl_kernel  *HandShakeKernel);
