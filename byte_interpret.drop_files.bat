@echo off
rem
rem Drop 1 file .desc + some binary files on this script.
rem For each binary file, a .txt file is generated and displayed.
rem 


setlocal enableDelayedExpansion

set FDESC_FILE_SUFFIX=.fdesc
set OUTPUT_FILE_SUFFIX=.txt

set THIS_SCRIPT_PATH=%~dp0
set BYTEINTERPRET=%THIS_SCRIPT_PATH%\byte_interpret.exe
set FDESC_FILE=

rem check existence of BYTEINTERPRET
if NOT exist %BYTEINTERPRET% (
  echo %BYTEINTERPRET% does not exist.
  goto exit_error
)

rem check there are parameters (FOR? will fail if not)
if ""!FDESC_FILE!""=="""" (
  if ""%2""=="""" (
    rem 0 or 1 file, I need at least 1 binary file and 1 .fdesc file.
    goto usage_and_exit
  )
) else (
  if ""%1""=="""" (
    rem no files, I need at least 1 binary file.
    goto usage_and_exit
  )
)


rem search for a mandatory and unique <file>.fdesc
FOR %%A IN (%*) DO (
  if ""%%~xA""==""%FDESC_FILE_SUFFIX%"" (
    if ""!FDESC_FILE!"" NEQ """" (
	  echo "Only 1 <file>%FDESC_FILE_SUFFIX% is expected."
      goto usage_and_exit
	)
    set FDESC_FILE=%%A
  )
)

if ""!FDESC_FILE!""=="""" (
  echo "1 <file>%FDESC_FILE_SUFFIX% is expected."
  goto usage_and_exit
)
	  
rem @echo on
echo %FDESC_FILE%


rem Interpret each file (except .fdesc and .txt)
FOR %%A IN (%*) DO (
  if ""%%~xA"" NEQ ""%FDESC_FILE_SUFFIX%"" (
    if ""%%~xA"" NEQ ""%OUTPUT_FILE_SUFFIX%"" (
	  set OUTPUT_FILE=%%A%OUTPUT_FILE_SUFFIX%
	  echo.
	  echo Generate !OUTPUT_FILE!
	  echo.
      %BYTEINTERPRET%  %FDESC_FILE%  -frame_bin  "%%A"  >  "!OUTPUT_FILE!"
	  
	  rem display the generated file
	  START /B CMD /C CALL "!OUTPUT_FILE!"
rem	  START /MIN /B CMD /C CALL "!OUTPUT_FILE!"
rem	  START CMD /C CALL "!OUTPUT_FILE!"
rem	  START START /B CMD /C CALL "!OUTPUT_FILE!"
rem	  START /B CALL CMD /C CALL "!OUTPUT_FILE!"
rem	  START /B START /B CMD /C CALL !OUTPUT_FILE!
rem	  START /B CMD /C !OUTPUT_FILE!
rem	  CALL START /B CMD /C !OUTPUT_FILE!
	)
  )
)

rem script do not exit, it waits for child processes.
rem can close it manually (without killing child processes).
goto exit


:usage_and_exit
rem @echo on
  echo.
  echo "Expect 1 and only 1 <file>.fdesc."
  echo "Expect at least 1 <other_file> (except %OUTPUT_FILE_SUFFIX%)."
  echo "Call byte_interpret.exe  <file>.fdesc  -frame_bin  <other_file>  >  <other_file>%OUTPUT_FILE_SUFFIX%"
  echo "  for each <other_file>."
goto exit_error

:exit_error
pause
goto exit

:exit
exit

