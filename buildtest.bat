
REM Set environment
call  buildenv.bat

REM Production & Execution
nmake -f Makefile.nmake unit_test_exec

pause

