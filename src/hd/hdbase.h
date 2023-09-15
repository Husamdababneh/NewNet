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

#define stringify_literal( x ) # x
#define stringify_expanded( x ) stringify_literal( x )
#define stringify_with_quotes( x ) stringify_expanded( stringify_expanded( x ) )
#define contact(x, y) #x ": " stringify_with_quotes(y)

// custom assert :)
// TODO: msg
#define assert(x, ...) { if (!(x)) (*((int*)(0)) = 1);}
#ifdef offsetof
#undef offsetof
   #define offsetof(a, b) (U64)(&((a*)0)->b)
#endif

#ifdef member_size
#undef member_size
   #define member_size(type, member) sizeof(((type *)0)->member) // this atually works :)
#endif

#ifdef countof
#undef countof
   #define countof(x) sizeof(x) / sizeof(x[0])
#endif


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

// we may need this in the future 
#if 0
template <typename F>
struct Defer
{
	Defer(F f) : f(f) {}
	~Defer() { f(); }
	F f;
};


#define CONCAT0(a, b) a##b
#define CONCAT(a, b) CONCAT0(a, b)
#define defer(body) Defer CONCAT(defer, __LINE__)([&]() { body; })
#endif

#if COMPILER_CL == 1
#pragma warning( disable : 4668 )
#endif

//~ Basic Types

#ifdef HD_TYPES

#include <stdint.h>

// Fixed Width singed integers
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef void*    PTR;
//typedef intptr_t PTR;

// Fixed Width unsinged integers
typedef uint8_t   U8;
typedef uint16_t  U16;
typedef uint32_t  U32;
typedef uint64_t  U64;

// This is actually not neccecry 
typedef float     F32;
typedef double    F64;

// Any Value but 0 is true
typedef uint8_t   B8;
typedef uint16_t  B16;
typedef uint32_t  B32;
typedef uint64_t  B64;

// Sizes
typedef size_t Size;

#define _BYTE_ 1
#define KB(x) (U64)(   x  * 1024)
#define MB(x) (U64)(KB(x) * 1024)
#define GB(x) (U64)(MB(x) * 1024)
#define TB(x) (U64)(GB(x) * 1024)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


// Casey Muratori way of differentiating between the meaning of static in different locations

// Static Local variable
#define local_presist static 
// Global variable
#define global_variable static 
// translation-unit scope function
#define internal static  

#ifdef HD_STRING

//~ Strings and String Views 
struct String {
	union {U64   length;  U64  size; };
	union {char* str_char; S8*  str;   };
};

#define SV_SPECIFIER "%.*s"
#define SV_PRINT(x)  (int)x.length, x.str

typedef   String StringView; // do i need this ?
constexpr String operator ""_s(const char* string, U64 length);
internal inline    String cstr_to_string(char* string, U64 length);
internal inline    String cstr_to_string(char* string);
internal inline    B64    is_null_terminated(const String str);
internal inline    S8     compare_strings(const String left, const String right);
internal inline    B8     is_strings_equal(const String left, const String right);
internal inline    String sub_str(const String str, U64 offset);


#endif // HD_STRING

#endif // HD_TYPES
#endif //BASE_H






