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

void SignalHandler(int signal)
{
    if (signal == SIGABRT) {
        // abort signal handler code
    } else {
        // ...
    }
}


struct structure {
    int a;
};

LONG WINAPI AccessViolationHandler (PEXCEPTION_POINTERS pExp);
LONG WINAPI ArrayBoundsCheckHandler (PEXCEPTION_POINTERS info);
void print_procedure_info(PVOID procedureAddress);

int RemoveCRTTest()
{
    // TODO: handle access violations
    AddVectoredExceptionHandler (0, AccessViolationHandler);
    AddVectoredExceptionHandler (0, ArrayBoundsCheckHandler);


	
    
    // structure* a = nullptr;
    // a->a = 420;

    // This should generate C4789 on msvc to report a buffer overrun
    U32 array[69] = {};
     array[69] = 420;
    return 0;
}

#include "dbghelp.h"

// I might want to handle everything in one procedure since I might always exit
// In case II don't want to exit I can write a procedure and give it a higher priority to handle the specific issue 
LONG WINAPI AccessViolationHandler (PEXCEPTION_POINTERS info)
{
    PEXCEPTION_RECORD er = info->ExceptionRecord;
    if(er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION){
        char stringBuffer[256] = {};
        char* formatString = "Null Pointer Exception at %p\n";
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
        ExitProcess(1);
        //return EXCEPTION_CONTINUE_SEARCH ;        
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI ArrayBoundsCheckHandler (PEXCEPTION_POINTERS info)
{
    PEXCEPTION_RECORD er = info->ExceptionRecord;
    if(er->ExceptionCode == EXCEPTION_ARRAY_BOUNDS_EXCEEDED){
        char stringBuffer[256] = {};
        char* formatString = "Array Bounds Exception at %p\n";
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
        ExitProcess(1);
        //return EXCEPTION_CONTINUE_SEARCH ;        
    }
    return EXCEPTION_CONTINUE_SEARCH;
}


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


        char* formatString = "Filename [%s:%lu] in %s";
        
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
        char* formatString2 = "SymFromAddr returned error : %d\n";
        auto a = (U64)snprintf(StringBuffer, 256, formatString2, GetLastError());
        String s = {a, StringBuffer};
        print_string(s);
    }
}
