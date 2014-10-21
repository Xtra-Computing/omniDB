@echo off
	copy  .\Handshaking\primitive.cl .\primitive.cl
	call .\Handshaking\DLL_OpenCL.exe
	copy  KernelTimeSpecification.list .\KernelScheduler\KernelTimeSpecification.list
	copy  KernelTimeSpecification.list .\OperatorSheduler\KernelTimeSpecification.list
	copy  KernelTimeSpecification.list .\QueryScheduler\KernelTimeSpecification.list
	del   primitive.cl
	move  KernelTimeSpecification.list .\Handshaking\KernelTimeSpecification.list

