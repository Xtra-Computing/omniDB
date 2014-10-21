int  Kernelscheduler(int size,int kid,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
void deschedule(const int preFlag, const double preBurden);
int  cl_readbufferscheduler(int size,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
int  cl_writebufferscheduler(int size,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);
int  cl_copyBufferscheduler(int size,int *Flag_CPU_GPU,double * burden,int _CPU_GPU);