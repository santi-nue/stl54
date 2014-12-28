
Some wireshark files are copied here.

Because it is complicated to configure to generate at the same time :
- dll     used by ...shark
- dll not used by ...shark
- stand-alone executable

And must also take care of :
- Windows vs Linux
- 1.12 vs 1.10 ...



shark_wsutil_aes.x :
- commented #include "ws_symbol_export.h"
- commented WS_DLL_PUBLIC
- rename functions rijndael_ to fdesc_rijndael_
