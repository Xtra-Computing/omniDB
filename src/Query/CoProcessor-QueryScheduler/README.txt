/*
Source code and executable packages for GPUQP
http://www.cse.ust.hk/gpuqp/
July, 2008.
*/

This is the source code for the co-processor using the CPU- and the GPU-based engines. 

*INSTALL*
- OS and hardware: Windows XP, NVIDIA CUDA SDK 1.0 or above, NVIDIA 8800 GTX GPU or compatible GPUs.
- Programming IDE: Microsoft Visual Studio C++ 2005 or compatible compilers.
- Library: the DLLs and header files from the CPU- and the GPU-based engines.

*How-to*
- Copy the DLLs and header files from the CPU- and the GPU-based engines to the target directory. The DLLs should be put in the same directory as the executable. 
- Set the project property for the dlls and header files. 

*Description*
In this code package, we have four directories:
- CoProcessor, the source code for the coprocessor. It consists of the coprocessing techniques for the CPU- and the GPU-based engines.
- MianLib, the directory for the header files from the CPU-based engine.
- MyLib, the directory for the header files from the GPU-based engine.
- SampleDLLs, the sample DLLs for testing the coprocessor.






