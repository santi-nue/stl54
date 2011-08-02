
REM Set environment
call  buildenv.bat

REM Clean production
nmake -f Makefile.nmake  distclean


pause
