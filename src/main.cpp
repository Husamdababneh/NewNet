/* ========================================================================
   $File: main.cpp
   $Date: 2023-02-27
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Entry point
    ========================================================================*/


#define HD_BASE_IMPL
#define HD_STRING
#include "hd/hdbase.h"
#include "hd/hd_str.cpp"

#include "windows.h"
#include "winuser.h"
#pragma comment(lib,"Kernel32.lib")
#pragma comment(lib,"User32.lib")
	


void print_string(const String str)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO old;
    GetConsoleScreenBufferInfo(console, &old);
    SetConsoleTextAttribute(console, FOREGROUND_RED);
    int output_size;
    WriteConsole(console, str.str_char, (DWORD)str.length, (LPDWORD)&output_size, NULL);
    SetConsoleTextAttribute(console, old.wAttributes);
}


int  main()
{
    //HMODULE instance = GetModuleHandleA(0);
    //LPSTR commandLineArguments = GetCommandLineA();
    
    
    
    //HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    String str = "This is a test string"_s;

    U64 offset = MIN(4, str.length);
    String sub = sub_str(str, offset);
    print_string(sub);
    //print_string(&str);

    
    DWORD tickCount = GetTickCount();
    char out[1024] = {0};
    out[0] = '\n';
    S64 count =  wsprintfA(out+1, "%d", (int)tickCount);
    if (count < 1)
        ExitProcess(1);
    String str2 = {(U64)count, out};
    print_string(str2);
    
    ExitProcess(0);
}
