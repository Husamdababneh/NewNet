/* ========================================================================
$File: base.h
$Date: July 31st 2021 4:02 pm
$Creator: Husam Dababneh
$Desc:  
=========================================================================*/
/* NOTEs
		
																						
*/

/* TODOs

TODO(Husam Dababneh): Add Intel Compiler ?? 
TODO(Husam Dababneh): Create new Printf function to handle our strings :) !!
TODO(Husam Dababneh): Architectures

*/


#ifndef BASE_H
#define BASE_H


//~
#if !defined(HD_TYPES)
# define HD_TYPES
#endif

//~ 
// NOTE(Husam Dababneh): Some useful Macros

// Casey Muratori way of differentiating between the meaning of static in different locations
// Static Local variable
#define local_presist static 
// Global variable
#define global_variable static 
// translation-unit scope function
#define internal static  

#define stringify_literal( x ) # x
#define stringify_expanded( x ) stringify_literal( x )
#define stringify_with_quotes( x ) stringify_expanded( stringify_expanded( x ) )
#define contact(x, y) #x ": " stringify_with_quotes(y)
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)


#define offsetof(a, b) (U64)(&((a*)0)->b)
#define member_size(type, member) sizeof(((type *)0)->member)
#define countof(x) sizeof(x) / sizeof(x[0])

// Taken from :
// https://github.com/EpicGamesExt/raddebugger/blob/c757388bb409e8670b63e31033e0ea0658587484/src/base/base_types.h#L163C1-L169C1
#define Compose64Bit(a,b)  ((((U64)a) << 32) | ((U64)b));
#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define AlignPadPow2(x,b)  ((0-(x)) & ((b) - 1))
#define IsPow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define IsPow2OrZero(x)    ((((x) - 1)&(x)) == 0)
 

//~ Compilers

// NOTE(Husam Dababneh): Do I Need more Compilers ?? I dont think so :)
// TODO(Husam Dababneh): Add Intel Compiler ?? 
#if defined(_MSC_VER)
# define COMPILER_CL 1 // MSVC Default Compiler
# define COMPILER_NAME contact(Microsoft Visual C++ Compiler (CL) Version, _MSC_FULL_VER)
# if defined(_M_AMD64) || defined(_M_X64)
#  define ARCH_AMD64 1
#  define ARCH_NAME "AMD 64-bit Architecture"
# elif defined(_M_ARM)
#  define ARCH_ARM   1
#  define ARCH_NAME "ARM Architecture"
# elif defined(_M_I86)
#  define ARCH_INTEL32   1
#  define ARCH_NAME "Intel 16-Bit Architecture"
# elif defined(_M_IX86)
#  define ARCH_INETL64   1
#  define ARCH_NAME "Intel 32-Bit Architecture"
# endif 
#elif defined(__clang__)
# // TODO(Husam Dababneh):  Architectures
# define COMPILER_CLANG 1
# define COMPILER_NAME contact(Clang C++ Compiler (Clang) Version, __clang_version__)
#elif defined(__GNUC__)
# define COMPILER_GCC 1
# // TODO(Husam Dababneh):  Architectures
# if defined(__GNUC_PATCHLEVEL__)
#    define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
# else
#    define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
# endif
# define COMPILER_NAME contact(GCC C++ Compiler (GCC) Version, __GNUC_VERSION__ )
#else
# error Unregistered Compiler
#endif 


// Zero Fill missing context macros
#if !defined(COMPILER_CL)
# define COMPILER_CL 0
#endif
#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif

#if !defined(COMPILER_NAME)
# define COMPILER_NAME "Unknown Compiler"
#endif

//~ Operating System
// TODO(Husam Dababneh): Check if can merge this with the above
#if defined(__linux__)
# define OS_LINUX 1
# define OS_NAME "Linux"
# define OS OS_NAME
#elif defined(_WIN64)
# define OS_WINDOWS 1
# define OS_WINDOWS_64 1
# define OS_NAME "Windows x64"
# define OS OS_NAME
#elif defined(_WIN32)
# define OS_WINDOWS 1
# define OS_WINDOWS_32 1
# define OS_NAME "Windows x32"
# define OS OS_NAME
#elif defined(__unix__)
# define OS_UNIX 1
# define OS_NAME "Unix"
# define OS OS_NAME
#elif defined(__FreeBSD__)
# define OS_FREEBSD 1
# define OS_NAME "FreeBSD"
# define OS OS_NAME
#elif defined(__APPLE__ ) && defined(__MACH__)
# define OS_MAC 1
# define OS_NAME "Apple Macintosh (MAC)"
# define OS OS_NAME
#else 
# error Unregistered Operating System 
#endif 

// Zero Fill missing OS macros
#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif
#if !defined(OS_WINDOWS_32)
# define OS_WINDOWS_32 0
#endif
#if !defined(OS_WINDOWS_64)
# define OS_WINDOWS_64 0
#endif
#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif
#if !defined(OS_UNIX)
# define OS_UNIX  0
#endif
#if !defined(OS_FREEBSD)
# define OS_FREEBSD 0
#endif
#if !defined(OS_MAC )
# define OS_MAC 0
#endif
#if !defined(OS_NAME)
# define OS_NAME "Unknown Operating System"
#endif

// NOTE(Husam Dababneh): The whole purpose of this defines are to make sure the user
//                       know his working environment to take action based on his situation.
#if !defined(OS)
#error Could not detect Operating System (You may diable this error if you know what you are doing)
#endif


#ifdef HD_TYPES

#include "base_types.h"



#endif // HD_TYPES


#if OS_WINDOWS
#define Assert_s(x, FILE, LINE)                                         \
	{																	\
        if (!x)                                                         \
        {                                                               \
            MessageBoxA(NULL,                                           \
                        #x,                                             \
                        "Assert in file:"##FILE" on Line:"##LINE,       \
                        MB_OK);                                         \
            __debugbreak();                                             \
            ExitProcess(1);                                             \
        }                                                               \
    }

#else

// @CleanUp(husamd): Clean this shit up
//#error please define Assert_s Macro
#define Assert_s(x, FILE, LINE)
#endif

#define Assert(x) Assert_s((x), __FILE__, LINE_STRING)

#endif
