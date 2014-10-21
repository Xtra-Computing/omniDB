@echo off

set count=0
set querynum=15
set threadnum=2

:loopcount
:loopthread
:loopquery
	CoProcessor.exe %querynum% %threadnum% 
	set /a querynum=%querynum%+5
	if %querynum% GTR 30 goto endloopquery
goto loopquery
:endloopquery
	set /a querynum=15
	set /a threadnum=%threadnum%+2
	if %threadnum% GTR 8 goto endloopthread
goto loopthread
:endloopthread
	set /a querynum=15
	set /a threadnum=2
	set /a count=%count%+1
	if %count% GTR 0 goto endloopcount
goto loopcount
:endloopcount