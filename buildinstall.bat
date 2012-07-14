
REM Set environment
call  buildenv.bat

REM Production
REM nmake -f Makefile.nmake  distclean
nmake -f Makefile.nmake -all

REM Include manifest into dll (1 = exe, 2 = dll).
REM This is necessary for dll works with a wireshark generated by another VC++
mt.exe -manifest generic.dll.manifest -outputresource:generic.dll;2


REM copy the dll into the current wireshark tree
for /f %%R in ('dir /b/ad "..\..\wireshark-gtk2\plugins\"') do   set subdir=%%R
copy generic.dll  "..\..\wireshark-gtk2\plugins\%subdir%"


pause

