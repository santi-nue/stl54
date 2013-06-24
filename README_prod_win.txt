
1) you need to have a wireshark tree
- Download wireshark source files (with svn or ...)
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
