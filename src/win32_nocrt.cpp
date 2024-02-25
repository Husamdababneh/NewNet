/* ========================================================================
   $File: nocrt.cpp
   $Date: 2023-08-07
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
   ========================================================================*/

#include "stdint.h"
#include "dbghelp.h"

extern "C"
{
int _fltused;

#ifdef _M_IX86 // following functions are needed only for 32-bit architecture

__declspec(naked) void _ftol2()
{
    __asm
    {
        fistp qword ptr [esp-8]
            mov   edx,[esp-4]
            mov   eax,[esp-8]
            ret
            }
}

__declspec(naked) void _ftol2_sse()
{
    __asm
    {
        fistp dword ptr [esp-4]
            mov   eax,[esp-4]
            ret
            }
}

#endif
}

// This comes from win32_asm.masm file 
extern "C" __int64 getRSP();

#if CHECK_STACK
size_t __chkstk (size_t stackSpaceSize)
{
  // Calculate what the stack pointer would be, if the caller of
  //   __chkstk simply made its stack allocation instead of
  //   calling __chkstk.
  //
  //   0x18: 0x10 for 2 saved registers (used by __chkstk), plus
  //         0x8 for the saved return address (__chkstk was call'd)
  uintptr_t adjustedSP = getRSP() + 0x18 - stackSpaceSize;

  // This error handling branch takes care of wraparounds. If the
  //   stack allocation is so big that it would wrap adjustedSP around,
  //   adjustedSP is set to zero. 
  //
  // With adjustedSP == 0, the loop at the end of the function will try 
  //   to touch every page from stackLimit to the top of the stack, 
  //   essentially triggering a stack overflow.
  if (getRSP() + 0x18 > stackSpaceSize)
    adjustedSP = 0;

  NT_TIB *tib = (NT_TIB *) NtCurrentTeb();
  uintptr_t stackLimit = (uintptr_t)tib->StackLimit;

  // Will the caller's SP reside below the guard area?
  if (adjustedSP >= stackLimit) {
    // If yes, then no probing needs to be done.
    return stackSpaceSize;
  }

  // If we got here, the caller's SP would skip (the) guard page(s).
  //   Probing of pages is needed.

  // Get to the first byte of the adjusted SP's page by rounding
  // down to 4K (a page spans 0x0 - 0xFFF)
  uintptr_t firstByteOfLastPage = adjustedSP &= 0xFFFFFFFFFFFFF000;
  uintptr_t currentPageToProbe = stackLimit;
  do {
	// Go "up" a page
	currentPageToProbe = currentPageToProbe - 0x1000;
		
    // Probe the page by writing a zero to it
    *reinterpret_cast<char*> (currentPageToProbe) = 0;
  } while (currentPageToProbe != firstByteOfLastPage);

  return stackSpaceSize;
}

#endif

void print_procedure_info(PVOID procedureAddress)
{
    HANDLE hProcess = GetCurrentProcess();
    SymInitialize(hProcess, NULL, TRUE);

    DWORD64  dwDisplacement = 0;
    DWORD64  dwAddress = (DWORD64)(*(&procedureAddress));

    char StringBuffer[256] = {};
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;
    if (SymFromAddr(hProcess, dwAddress, &dwDisplacement, pSymbol))
    {
        print_string("Procedure Name:["_s);
        String symbolName = {(U64)pSymbol->NameLen, pSymbol->Name};
        print_string(symbolName);
        print_string("]\n"_s);
        IMAGEHLP_LINE               il;
        IMAGEHLP_MODULE             im;


        const char* formatString = "Filename [%s:%lu] in Module [%s]";
        
        il.SizeOfStruct = sizeof(il);
        im.SizeOfStruct = sizeof(im);
        SymGetLineFromAddr64(hProcess, dwAddress, (PDWORD)&dwDisplacement, &il);
        SymGetModuleInfo(hProcess, dwAddress, &im);
        auto a = (U64)snprintf(StringBuffer,
                               256,
                               formatString,
                               //args
                               il.FileName,
                               il.LineNumber,
                               im.ModuleName);

        String s = {a, StringBuffer};
        print_string(s);
    }
    else
    {
        memset(StringBuffer, 0, 256);
        const char* formatString2 = "SymFromAddr returned error : %d\n";
        auto a = (U64)snprintf(StringBuffer, 256, formatString2, GetLastError());
        String s = {a, StringBuffer};
        print_string(s);
    }
}

LONG WINAPI ExceptionHandler(PEXCEPTION_POINTERS info)
{
    PEXCEPTION_RECORD er = info->ExceptionRecord;
    print_string("EXCEPTION Caught"_s);
    if(er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION){
        char stringBuffer[256] = {};
        const char* formatString = "ACCESS VIOLATION Exception at %p\n";
        auto a = (U64)snprintf(stringBuffer, 256, formatString, er->ExceptionAddress);
        String s = {a, stringBuffer};
        print_string(s);
        // TODO: more info where this happens and if we can debug it
        constexpr auto FLAG = EXCEPTION_NONCONTINUABLE;
        print_procedure_info(er->ExceptionAddress);
        if (er->ExceptionFlags & FLAG)
        {
            print_string("Exiting Program"_s);
        }
        UnhandledExceptionFilter(info);
        ExitProcess(1);
        //return EXCEPTION_CONTINUE_SEARCH ;        
    }
    return EXCEPTION_CONTINUE_SEARCH;
}


