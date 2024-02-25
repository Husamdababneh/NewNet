/* ========================================================================
   $File: test.cpp
   $Date: 2023-10-13
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/


#include <signal.h>

#pragma warning(disable : 4189)
#pragma warning (disable : 4235)

#define SUCCESS(x) x == 0


#define PNG_MISC_CHUNKS


int test()
{
    Initalize_LinearAllocate(&global_allocator, MB(100));
    
    LinearAllocator allocator;
    Initalize_LinearAllocate(&allocator, KB(4));
    
    // Get info about the file 
    //File file = open_file("../assets/Rock035_1K-PNG_Color.png"_s8);
    //File file = open_file("../assets/Rock035.png"_s8);
    File file = open_file("../assets/tests/8x8_uncompressed-no-interlace.png"_s8);
    //File file = open_file("../assets/tests/8x8_uncompressed-no-interlace"_s8);
    
    // This needs to be fixed
    if (file.handle == NULL)
    {
        print_string("File not found\n"_s);
        ExitProcess(1);
    }
    
    // Allocate Enough memory to house the file itself
    void* memory = VirtualAlloc(0, (SIZE_T)file.size, MEM_COMMIT, PAGE_READWRITE);
    
    // Read entire file to the allocated memory above
    StreamingBuffer entire_file = read_entire_file(file, memory);
    
    // Get info about the image itself
    PNG_FILE image_info = parse_png_file(entire_file, &allocator);
    
    // Compute how much memory does the image need, 
    // usually width*height*channels*(bit_depth/8)
    U32 requested_memory_size =  compute_requested_memory_size(&image_info);
    
    // Allocate enough memory for the image data
    void* image_data = VirtualAlloc(0, (SIZE_T)requested_memory_size, MEM_COMMIT, PAGE_READWRITE);    
    
    // Recounstruct the image data (load image)
    //load_png_image(&image_info, image_data);
    
    // Deallocate/free the file memory (give it back to the allocator)
    Free_LinearAllocate(&allocator);
    Free_LinearAllocate(&global_allocator);
    ExitProcess(1);
}


int test2(){
    Initalize_LinearAllocate(&global_allocator, MB(100));
    
    LinearAllocator allocator;
    Initalize_LinearAllocate(&allocator, KB(4));
    
    // Get info about the file 
    //File file = open_file("../assets/tests/8x8_uncompressed-no-interlace.png"_s8);
    //File file = open_file("../assets/tests/8x8_uncompressed-no-interlace"_s8);
    
    File file = open_file("../assets/tests/fixed.zlib"_s8);
    // File file = open_file("../assets/tests/dynamic.zlib"_s8);
    // File file = open_file("../assets/tests/store.zlib"_s8);
    // File file = open_file("../assets/tests/original.txt"_s8);
    
    // This needs to be fixed
    if (file.handle == NULL)
    {
        print_string("File not found\n"_s);
        ExitProcess(1);
    }
    
    // Allocate Enough memory to house the file itself
    MemoryBlock memory = RequestMemoryBlock(file.size);
    
    // Read entire file to the allocated memory above
    StreamingBuffer entire_file = read_entire_file(file, memory.memory);
    
	
    Buffer buffer;
	constexpr U32 bufferSize = 445;
	U8 tempBuffer[bufferSize] = {};
	buffer.content = tempBuffer;
	buffer.size = bufferSize;
    
    B8 result = inflate(entire_file, &buffer);
    if (result)
    {
      
        String s = {buffer.size, (char*)buffer.content};
        print_string(s);
    }
    
    
    // Deallocate/free the file memory (give it back to the allocator)
    FreeMemoryBlock(memory);
    Free_LinearAllocate(&allocator);
    Free_LinearAllocate(&global_allocator);
    ExitProcess(1);
    
}

struct structure {
    int a;
};

void tt()
{
    NT_TIB *tib = (NT_TIB *) NtCurrentTeb();
    ULONG_PTR lowLimit = (ULONG_PTR)tib->StackLimit;
    ULONG_PTR highLimit = (ULONG_PTR)tib->StackBase;

#if 1
    constexpr auto BUFFER_SIZE = 1024 * 1024 * 8;
    const char* formatString = "LARGE_INTEGER SIZE(%lld)\n";
    char bigArray[BUFFER_SIZE] = {};
#else 
    //This is handled by `ExceptionHandler`
    structure* struc = 0;
    struc->a = 3;
#endif
}


internal LONG WINAPI win32_exception_filter(EXCEPTION_POINTERS* exception_ptrs);


void test_vec2F32()
{
    Vec2F32 a = {1, 2};
    Vec2F32 z = -a;
    Vec2F32 b = {1, 2};
    Vec2F32 c = a + b;
    Vec2F32 d = a - b;
    Vec2F32 e = a * b;
    Vec2F32 f = a / b;
    
    constexpr U32 buffer_len = 1024;
    char buffer[buffer_len]  = {};
    const char* formatString       = R"str(
a    =  {x = %f, y = %f}
b    =  {x = %f, y = %f}
c +  =  {x = %f, y = %f}
d -  =  {x = %f, y = %f}
e *  =  {x = %f, y = %f}
f /  =  {x = %f, y = %f}
-a   =  {x = %f, y = %f}
)str";


    S32 size = snprintf(buffer, buffer_len, formatString,
                        a.x, a.y,
                        b.x, b.y,
                        c.x, c.y,
                        d.x, d.y,
                        e.x, e.y,
                        f.x, f.y,
                        z.x, z.y        );
    String str = cstr_to_string(buffer, (U64)size);
    print_string(str);
}

void test_vec3F32()
{
    Vec3F32 a0 = {3, 3, 3};
    Vec3F32 a = {1, 2, 3};
    Vec3F32 b = {1, 2, 3};
    Vec3F32 c = a + b;
    Vec3F32 d = a - b;
    Vec3F32 e = a * b;
    Vec3F32 f = a / b;
    Vec3F32 g = e - a;

    Vec3F32 cross_prod = cross(a, g);
    F32 dot_prod = dot(a, g);
    
    constexpr U32 buffer_len = 1024;
    char buffer[buffer_len]  = {};
    const char* formatString = R"str(
a0          =  {x = %f, y = %f, z = %f}
a           =  {x = %f, y = %f, z = %f}
b           =  {x = %f, y = %f, z = %f}
c +         =  {x = %f, y = %f, z = %f}
d -         =  {x = %f, y = %f, z = %f}
e *         =  {x = %f, y = %f, z = %f}
f /         =  {x = %f, y = %f, z = %f}
g = e - a   =  {x = %f, y = %f, z = %f}
a X g       =  {x = %f, y = %f, z = %f}
a . g       =  %f
)str";


    S32 size = snprintf(buffer, buffer_len, formatString,
                        a0.x, a0.y, a0.z,
                        a.x, a.y, a.z,
                        b.x, b.y, b.z,
                        c.x, c.y, c.z,
                        d.x, d.y, d.z,
                        e.x, e.y, e.z,
                        f.x, f.y, f.z,
                        g.x, g.y, g.z,
                        cross_prod.x, cross_prod.y, cross_prod.z,
                        dot_prod);
    String str = cstr_to_string(buffer, (U64)size);
    print_string(str);
}


int RemoveCRTTest()
{
//    SetUnhandledExceptionFilter(&win32_exception_filter);
    AddVectoredExceptionHandler(1, ExceptionHandler);

    constexpr U32 buffer_len = 256;
    char buffer[buffer_len]  = {};
    const char* formatString       = "%f";


    S32 size = snprintf(buffer, buffer_len, formatString,
                        q_rsqrt(4.0f));
    String str = cstr_to_string(buffer, (U64)size);
    print_string(str);
    test_vec3F32();
    //test_vec2F32();
    return 0;
}


#if 0
internal B32 g_is_quiet = 0;


internal LONG WINAPI
win32_exception_filter(EXCEPTION_POINTERS* exception_ptrs)
{
  if(g_is_quiet)
  {
    ExitProcess(1);
  }
  
  static volatile LONG first = 0;
  if(InterlockedCompareExchange(&first, 1, 0) != 0)
  {
    // prevent failures in other threads to popup same message box
    // this handler just shows first thread that crashes
    // we are terminating afterwards anyway
    for (;;) Sleep(1000);
  }
  
  WCHAR buffer[4096] = {0};
  int buflen = 0;
  
  DWORD exception_code = exception_ptrs->ExceptionRecord->ExceptionCode;
  buflen += wnsprintfW(buffer + buflen,
                       sizeof(buffer) - buflen,
                       L"A fatal exception (code 0x%x) occurred. The process is terminating.\n",
                       exception_code);
  
  // load dbghelp dynamically just in case if it is missing
  HMODULE dbghelp = LoadLibraryA("dbghelp.dll");
  if(dbghelp)
  {
    DWORD (WINAPI *dbg_SymSetOptions)(DWORD SymOptions);
    BOOL (WINAPI *dbg_SymInitializeW)(HANDLE hProcess, PCWSTR UserSearchPath, BOOL fInvadeProcess);
    BOOL (WINAPI *dbg_StackWalk64)(DWORD MachineType, HANDLE hProcess, HANDLE hThread,
                                   LPSTACKFRAME64 StackFrame, PVOID ContextRecord,
                                   PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
                                   PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
                                   PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
                                   PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);
    
    PVOID (WINAPI *dbg_SymFunctionTableAccess64)(HANDLE hProcess, DWORD64 AddrBase);
    DWORD64 (WINAPI *dbg_SymGetModuleBase64)(HANDLE hProcess, DWORD64 qwAddr);
    BOOL (WINAPI *dbg_SymFromAddrW)(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFOW Symbol);
    BOOL (WINAPI *dbg_SymGetLineFromAddrW64)(HANDLE hProcess, DWORD64 dwAddr, PDWORD pdwDisplacement,
                                             PIMAGEHLP_LINEW64 Line);
    BOOL (WINAPI *dbg_SymGetModuleInfoW64)(HANDLE hProcess, DWORD64 qwAddr, PIMAGEHLP_MODULEW64 ModuleInfo);
    
    *(FARPROC*)&dbg_SymSetOptions            = GetProcAddress(dbghelp, "SymSetOptions");
    *(FARPROC*)&dbg_SymInitializeW           = GetProcAddress(dbghelp, "SymInitializeW");
    *(FARPROC*)&dbg_StackWalk64              = GetProcAddress(dbghelp, "StackWalk64");
    *(FARPROC*)&dbg_SymFunctionTableAccess64 = GetProcAddress(dbghelp, "SymFunctionTableAccess64");
    *(FARPROC*)&dbg_SymGetModuleBase64       = GetProcAddress(dbghelp, "SymGetModuleBase64");
    *(FARPROC*)&dbg_SymFromAddrW             = GetProcAddress(dbghelp, "SymFromAddrW");
    *(FARPROC*)&dbg_SymGetLineFromAddrW64    = GetProcAddress(dbghelp, "SymGetLineFromAddrW64");
    *(FARPROC*)&dbg_SymGetModuleInfoW64      = GetProcAddress(dbghelp, "SymGetModuleInfoW64");
    
    if(dbg_SymSetOptions &&
       dbg_SymInitializeW &&
       dbg_StackWalk64 &&
       dbg_SymFunctionTableAccess64 &&
       dbg_SymGetModuleBase64 &&
       dbg_SymFromAddrW &&
       dbg_SymGetLineFromAddrW64 &&
       dbg_SymGetModuleInfoW64)
    {
      HANDLE process = GetCurrentProcess();
      HANDLE thread = GetCurrentThread();
      CONTEXT* context = exception_ptrs->ContextRecord;
      
      dbg_SymSetOptions(SYMOPT_EXACT_SYMBOLS | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
      if(dbg_SymInitializeW(process, L"", TRUE))
      {
        // check that raddbg.pdb file is good
        B32 raddbg_pdb_valid = 0;
        {
          IMAGEHLP_MODULEW64 module = {0};
          module.SizeOfStruct = sizeof(module);
          if(dbg_SymGetModuleInfoW64(process, (DWORD64)&win32_exception_filter, &module))
          {
            raddbg_pdb_valid = (module.SymType == SymPdb);
          }
        }
        
        if(!raddbg_pdb_valid)
        {
          buflen += wnsprintfW(buffer + buflen,
                               sizeof(buffer) - buflen,
                               L"\nraddbg.pdb debug file is not valid or not found. Please rebuild binary to get call stack.\n");
        }
        else
        {
          STACKFRAME64 frame = {0};
          DWORD image_type;
#if defined(_M_AMD64)
          image_type = IMAGE_FILE_MACHINE_AMD64;
          frame.AddrPC.Offset = context->Rip;
          frame.AddrPC.Mode = AddrModeFlat;
          frame.AddrFrame.Offset = context->Rbp;
          frame.AddrFrame.Mode = AddrModeFlat;
          frame.AddrStack.Offset = context->Rsp;
          frame.AddrStack.Mode = AddrModeFlat;
#elif defined(_M_ARM64)
          image_type = IMAGE_FILE_MACHINE_ARM64;
          frame.AddrPC.Offset = context->Pc;
          frame.AddrPC.Mode = AddrModeFlat;
          frame.AddrFrame.Offset = context->Fp;
          frame.AddrFrame.Mode = AddrModeFlat;
          frame.AddrStack.Offset = context->Sp;
          frame.AddrStack.Mode = AddrModeFlat;
#else
#  error Architecture not supported!
#endif
          
          for(U32 idx=0; ;idx++)
          {
            const U32 max_frames = 32;
            if(idx == max_frames)
            {
              buflen += wnsprintfW(buffer + buflen, sizeof(buffer) - buflen, L"...");
              break;
            }
            
            if(!dbg_StackWalk64(image_type,
                                process,
                                thread,
                                &frame,
                                context,
                                0,
                                dbg_SymFunctionTableAccess64,
                                dbg_SymGetModuleBase64,
                                0)
                )
            {
              break;
            }
            
            U64 address = frame.AddrPC.Offset;
            if(address == 0)
            {
              break;
            }
            
            if(idx==0)
            {
              buflen += wnsprintfW(buffer + buflen, sizeof(buffer) - buflen,
                                   L"\nPress Ctrl+C to copy this text to clipboard, then create a new issue in\n"
                                   L"<a href=\"%S\">%S</a>\n\n", RADDBG_GITHUB_ISSUES, RADDBG_GITHUB_ISSUES);
              buflen += wnsprintfW(buffer + buflen, sizeof(buffer) - buflen, L"Call stack:\n");
            }
            
            buflen += wnsprintfW(buffer + buflen, sizeof(buffer) - buflen, L"%u. [0x%I64x]", idx, address);
            
            struct {
              SYMBOL_INFOW info;
              WCHAR name[MAX_SYM_NAME];
            } symbol = {0};
            
            symbol.info.SizeOfStruct = sizeof(symbol.info);
            symbol.info.MaxNameLen = MAX_SYM_NAME;
            
            DWORD64 displacement = 0;
            if(dbg_SymFromAddrW(process, address, &displacement, &symbol.info))
            {
              buflen += wnsprintfW(buffer + buflen,
                                   sizeof(buffer) - buflen,
                                   L" %s +%u",
                                   symbol.info.Name,
                                   (DWORD)displacement);
              
              IMAGEHLP_LINEW64 line = {0};
              line.SizeOfStruct = sizeof(line);
              
              DWORD line_displacement = 0;
              if(dbg_SymGetLineFromAddrW64(process, address, &line_displacement, &line))
              {
                buflen += wnsprintfW(buffer + buflen,
                                     sizeof(buffer) - buflen,
                                     L", %s line %u",
                                     PathFindFileNameW(line.FileName), line.LineNumber);
              }
            }
            else
            {
              IMAGEHLP_MODULEW64 module = {0};
              module.SizeOfStruct = sizeof(module);
              if(dbg_SymGetModuleInfoW64(process, address, &module))
              {
                buflen += wnsprintfW(buffer + buflen, sizeof(buffer) - buflen, L" %s", module.ModuleName);
              }
            }
            
            buflen += wnsprintfW(buffer + buflen, sizeof(buffer) - buflen, L"\n");
          }
        }
      }
    }
  }
  
  // remove last newline
  buffer[buflen] = 0;
  
  TASKDIALOGCONFIG dialog = {0};
  dialog.cbSize = sizeof(dialog);
  dialog.dwFlags = TDF_SIZE_TO_CONTENT | TDF_ENABLE_HYPERLINKS | TDF_ALLOW_DIALOG_CANCELLATION;
  dialog.pszMainIcon = TD_ERROR_ICON;
  dialog.dwCommonButtons = TDCBF_CLOSE_BUTTON;
  dialog.pszWindowTitle = L"Fatal Exception";
  dialog.pszContent = buffer;
  dialog.pfCallback = &win32_dialog_callback;
  TaskDialogIndirect(&dialog, 0, 0, 0);
  
  ExitProcess(1);
}
#endif
