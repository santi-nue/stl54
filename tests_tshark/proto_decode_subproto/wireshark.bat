
pushd ..
REM Positionnement de l'environnement
call  testenv.bat

popd

start "" %wireshark%
