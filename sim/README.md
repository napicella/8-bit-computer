To allow vscode gdb to step in the libraries like vremu6522, add this to the `CmakeLists.txt` that lives along side the C file, eg vrEmu6522.c

```
if (UNIX)
  add_definitions(-ggdb)
endif()

```

For example: 
```C
add_library(vrEmu6522 vrEmu6522.c)

if (WIN32)
  if (BUILD_SHARED_LIBS)
     add_definitions(-DVR_6522_EMU_COMPILING_DLL)
   endif()
endif()

# this allows vscode gdb to step in the library
if (UNIX)
  add_definitions(-ggdb)
endif()

target_include_directories (vrEmu6522 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
```
