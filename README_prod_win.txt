
********************************************************************************
*** Instructions for wireshark >= 2.4
********************************************************************************
Example for 3.2.0

1) you need to have a wireshark tree
- Download wireshark source files (with git or ...)
- Generate wireshark following instructions ...
--> <wireshark build directory>/run/wireshark

2) clone wsgd
cd  <wireshark sources dir>/plugins/epan
git clone https://gitlab.com/wsgd/wsgd.git  generic

3) Configure generic directory
cd  generic
copy  CMakeLists.302XX.txt  CMakeLists.txt
copy  cmake_wireshark_version_number.cmake.example  cmake_wireshark_version_number.cmake
Edit cmake_wireshark_version_number.cmake to set 30200

4) Configure wireshark CMakeLists.txt
cd  ../../..
Edit CMakeLists.txt to add a generic line just before the gryphon line

5) Generate again wireshark
--> <wireshark build directory>/run/plugins/3.2/epan/generic.so




********************************************************************************
*** Instructions for old versions
********************************************************************************

1) you need to have a wireshark tree
- Download wireshark source files (with git or ...)
- Generate wireshark following instructions ...

2) Put this directory into <wireshark src dir>/plugins

3) Copy Makefile.version.example to Makefile.version
    and change the version inside

4) Copy buildenv.bat.example to buildenv.bat
    and change what you have to change

5) Production : buildinstall.bat

6) Unit tests : buildtest.bat

7) Usage : use ../../wireshark-gtk2/wireshark.exe
        or copy generic.dll to the plugin dir
