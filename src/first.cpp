/* ========================================================================
   $File: first.cpp
   $Date: 2023-10-13
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/





#define _STRSAFE_H_INCLUDED_
#define _INC_STDIO



#define WIN32_LEAN_AND_MEAN 
#include "windows.h"
#include "winuser.h"
#pragma comment(lib,"Kernel32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Gdi32.lib")

// #ifdef debug
#pragma comment(lib,"Dbghelp.lib")

// @TODO(husamd): replace this with custom implementation
#define STOP_WARNINGS
#include "printf.c"

#pragma warning (disable: 4710)

#define HD_BASE_IMPL
#define HD_STRING
#define HD_MEMORY
#define HD_IO
#define HD_TEMP_RESULT
#include "hd/inc.inc"

#include "win32_nocrt.cpp"

#include "strsafe.h"
#include "main.h"
#include "opengl.cpp"

global_variable LinearAllocator global_allocator;
#include "hd/image.cpp"

#include "main.cpp"

#include "test.cpp"
