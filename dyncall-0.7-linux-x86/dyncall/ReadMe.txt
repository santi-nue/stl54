
Directory lib is not used.
Because link with .a does not work.
# gives warning "... is not portable!"
# gives U dlLoadLibrary
Even when re-generated.
Even when re-generated with :
- -fPIC
- -Wl,-export-dynamic
Even when linked with :
- -Wl,-whole-archive



Directories ..._o are used instead.
Directories ..._o are re-generated.

