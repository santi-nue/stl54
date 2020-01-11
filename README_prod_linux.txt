
Instructions for old versions

1) you need to have a wireshark tree
- Download wireshark source files (with git or ...)
- Generate wireshark following instructions ... (look at Makefile.Linux....)

2) Put this directory into <wireshark src dir>/plugins

3) Use Makefile.Linux.... as an example (see instructions inside it)

4) Production : make

5) Unit tests : make unit_test_exec

6) Usage : copy .libs/generic.so to the plugin dir
