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


B8 InializeWin32Console()
{
    BOOL success = AllocConsole();
    if (!success)
    {
        return false;
    }

    HANDLE hConOut = CreateFile("CONOUT$",
                                GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    SetStdHandle(STD_OUTPUT_HANDLE, hConOut);

    return true;
}




LRESULT CALLBACK
MainWindowCallback(HWND   window,
                   UINT   message,
                   WPARAM wParam,
                   LPARAM lParam)
{
    (void)wParam;
    (void)lParam;
    (void)window;
    
    LRESULT result = 0;

    switch(message)
    {
      case WM_SIZE:
      {
          print_string("WM_SIZE\n"_s);
      }break;
      case WM_DESTROY:
      {
          print_string("WM_DESTROY\n"_s);
      }
      break;
      case WM_CLOSE:
      {
          print_string("WM_CLOSE\n"_s);
          ExitProcess(0);
      }break;
      case WM_ACTIVATEAPP:
      {
          print_string("WM_ACTIVATEAPP\n"_s);
      }break;
      default:
      {
          result = DefWindowProc(window, message, wParam, lParam);
      }break;
    }
    return result;
}

/*
  const char * CMSWindow::RegisterWindowClass
  (DWORD style,  int brush, WNDPROC pfnWndProc, HICON hIcon, int iCursorResource)
*/
static const char* ClassName = "TestClass";
const char* RegisterWindowClass(HMODULE instance)
{
	WNDCLASSEXA wc = {0};
    wc.cbSize      = sizeof(wc);
    wc.style       = CS_OWNDC | CS_HREDRAW | CS_VREDRAW /*Redraw if size changes*/;
    wc.lpfnWndProc = MainWindowCallback;
    wc.cbClsExtra  = 0;
    wc.cbWndExtra  = 0;
    wc.hInstance   = instance;
    wc.hIcon       = NULL;
    wc.hCursor     = NULL;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
    wc.lpszMenuName  = "MenuName";
    wc.lpszClassName = ClassName;


    ATOM result = RegisterClassEx(&wc);
	if (!result)
    {
        // LOG HERE
        ExitProcess(1);
    }
	return ClassName;
}



int  main()
{
    HMODULE instance = GetModuleHandleA(0);
    //LPSTR commandLineArguments = GetCommandLineA();
    InializeWin32Console();
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

    
    // Create Windows Window

    // Register Class 
    const char* className = RegisterWindowClass(instance);


    // Create the window 
    HWND window = CreateWindowExA(0,
                                  className,
                                  "Window Name",
                                  WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                  0,
                                  0,
                                  800,
                                  600,
                                  NULL,
                                  NULL,
                                  instance,
                                  NULL);

    B8 running = true;
    while(running)
    {
        MSG message;
        BOOL messageResult = GetMessage(&message, 0, 0, 0);
        if (messageResult > 0)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
            continue;
        }
        
        running = false;
    }
    
    ExitProcess(0);
}
