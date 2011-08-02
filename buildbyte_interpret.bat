
REM Set environment
call  buildenv.bat

REM Production & Execution
nmake -f Makefile.nmake byte_interpret.exe

REM Include manifest into dll (1 = exe, 2 = dll).
REM This is necessary for exe works on another windows ?
REM No manifest for VC++ 2003, not necessary for VC++ 2008 ?
REM mt.exe -manifest byte_interpret.exe.manifest -outputresource:byte_interpret.exe;1

pause

