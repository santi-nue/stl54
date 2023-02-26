
@echo off

REM Positionnement de l'environnement
if EXIST    "..\..\..\..\zzz_wsgd_env.bat" (
    call    "..\..\..\..\zzz_wsgd_env.bat"
) else (
    if EXIST "..\..\..\..\..\zzz_wsgd_env.bat"  call "..\..\..\..\..\zzz_wsgd_env.bat"
)

set windiff="C:\Program Files\Microsoft SDKs\Windows\v7.0\Bin\windiff.exe"

set wireshark=%WIRESHARK_WSGD_EXE_DIR%\Wireshark.exe
set tshark=%WIRESHARK_WSGD_EXE_DIR%\tshark.exe

set WIRESHARK_GENERIC_DISSECTOR_DIR=.
