
@echo off
setlocal ENABLEDELAYEDEXPANSION

REM Positionnement de l'environnement
call  testenv.bat

rem ***************************************************************************
rem Build versions list (from our version to lower)
rem ***************************************************************************

set version_list_full=204XX;206XX;300XX;302XX;304XX;306XX;400XX
set version_list=

for %%v in (%version_list_full%) do (
    rem echo %%v
    if "!version_list!" NEQ "" (
        set version_list=%%v;!version_list!
    ) else (
        set version_list=%%v
    )
    rem echo version_list=!version_list!
    if "%WS_VER_TARGET_DIR%" == "%%v"  goto :version_list_DONE
)

:version_list_DONE
echo version_list=%version_list%

rem ***************************************************************************
rem Test only 1 sub-directory
rem ***************************************************************************

if  NOT  X%1==X  (
    rem Keep only sub-directory
    set WIRESHARK_GENERIC_DISSECTOR_DIR=%~nx1
	set WIRESHARK_GENERIC_DISSECTOR_TRACES_DIR=%~nx1
    call  :tst_dir
    goto :the_end
)

rem ***************************************************************************
rem Test all proto_... sub-directories
rem ***************************************************************************

FOR  /D  %%i  IN ("proto_*")  DO  (
    set WIRESHARK_GENERIC_DISSECTOR_DIR=%%i
	set WIRESHARK_GENERIC_DISSECTOR_TRACES_DIR=%%i
    call  :tst_dir
)

goto :the_end


rem ***************************************************************************
rem Test all *.pcap into WIRESHARK_GENERIC_DISSECTOR_DIR
rem ***************************************************************************
:tst_dir

FOR  %%i  IN ("%WIRESHARK_GENERIC_DISSECTOR_DIR%\*.pcap")  DO  (
  set infile=%%i
  call  :tst_dir_file
)
goto :EOF


rem ***************************************************************************
rem Test infile (pcap file)
rem ***************************************************************************
:tst_dir_file

set tst_out_full_file=%infile%.tst_out.full.txt
set tst_out_file=%infile%.tst_out.txt
set tst_err_full_file=%infile%.tst_err.full.txt
set tst_err_file=%infile%.tst_err.txt

for %%v in (%version_list%) do (
    set ref_out_file=%infile%.ref_out.%%v.txt
    if EXIST "!ref_out_file!"  goto :ref_out_file_DONE
)
:ref_out_file_DONE
if NOT EXIST "!ref_out_file!" (
    echo !ref_out_file! not found
    goto :EOF
)

for %%v in (%version_list%) do (
    set ref_err_file=%infile%.ref_err.%%v.txt
    if EXIST "!ref_err_file!"  goto :ref_err_file_DONE
)
:ref_err_file_DONE
if NOT EXIST "!ref_err_file!" (
    echo !ref_err_file! not found
    goto :EOF
)

rem  Remove previous test result files
IF EXIST "%tst_out_file%"  del  %tst_out_file%
IF EXIST "%tst_err_file%"  del  %tst_err_file%

rem set display_filter_option="-R example_1.error_in_packet"

rem Call tshark
%tshark%  -n -r %infile%  -T text -V  %display_filter_option%  > %tst_out_full_file%  2> %tst_err_full_file%

rem Remove all details of upper level (TCP, IP, Ethernet, Frame)
perl test_output_filter.pl  %tst_out_full_file%  >  %tst_out_file%

rem Remove some details
perl test_error_filter.pl  %tst_err_full_file%  >  %tst_err_file%


set tst_file=%tst_err_file%
set ref_file=%ref_err_file%
set chk_label=err
call :tst_chk_file

set tst_file=%tst_out_file%
set ref_file=%ref_out_file%
set chk_label=out
call :tst_chk_file

goto :EOF

rem ***************************************************************************
rem Compare 2 files (tst_file & ref_file) which should be identic
rem ***************************************************************************
:tst_chk_file

fc  %ref_file%  %tst_file%  > nul
if  %ERRORLEVEL%  NEQ  0  (
    rem Files are different
    echo  NOT ok :  %chk_label%  %infile%
  
    rem  "" mandatory, sinon j'ai juste un bloc-notes qui est ouvert
    start  ""  %windiff%  %ref_file%  %tst_file%
  
) else (
    echo      ok :  %chk_label%  %infile%
)
goto :EOF


:the_end
pause


REM tshark [ -a <capture autostop condition> ] ...
REM        [ -b <capture ring buffer option>] ...
REM        [ -B <capture buffer size (Win32 only)> ]  
REM        [ -c <capture packet count> ] 
REM        [ -C <configuration profile> ] 
REM        [ -d <layer type>==<selector>,<decode-as protocol> ] 
REM        [ -D ] 
REM        [ -e <field> ] 
REM        [ -E <field print option> ] 
REM        [ -f <capture filter> ] 
REM        [ -F <file format> ]
REM        [ -h ] 
REM        [ -i <capture interface>|- ] [ -l ] [ -L ] [ -n ] [ -N <name resolving flags> ] [ -o <preference setting> ] ... [ -p ] [ -q ] 
REM        [ -r <infile> ] 
REM        [ -R <read (display) filter> ] 
REM        [ -s <capture snaplen> ] [ -S ] [ -t ad|a|r|d|e ] [ -T pdml|psml|ps|text|fields ] [ -v ] [ -V ] 
REM        [ -w <outfile>|- ] 
REM        [ -x ] [ -X <eXtension option>] [ -y <capture link type> ] [ -z <statistics> ] [ <capture filter> ]
