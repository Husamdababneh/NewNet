/* ========================================================================
   $File: base_types.h
   $Date: 2024-03-10
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once


// Find msvc stddef.h impl
//~ Basic Types

#if defined(_MSC_VER)
typedef  __int8  S8;
typedef  __int16 S16;
typedef  __int32 S32;
typedef  __int64 S64;

typedef  unsigned __int8   U8;
typedef  unsigned __int16  U16;
typedef  unsigned __int32  U32;
typedef  unsigned __int64  U64;

typedef unsigned __int8   B8;
typedef unsigned __int16  B16;
typedef unsigned __int32  B32;
typedef unsigned __int64  B64;

typedef size_t Size;

typedef float     F32;
typedef double    F64;

#elif defined(__clang__) 
#include <stdint.h>
typedef   int8_t  S8;
typedef  int16_t S16;
typedef  int32_t S32;
typedef  int64_t S64;

typedef   uint8_t  U8;
typedef  uint16_t U16;
typedef  uint32_t U32;
typedef  uint64_t U64;

typedef   uint8_t B8;
typedef  uint16_t B16;
typedef  uint32_t B32;
typedef  uint64_t B64;

// CLANG ??
typedef __SIZE_TYPE__ size_t;
typedef size_t Size;

typedef __PTRDIFF_TYPE__ ptrdiff_t;


typedef float     F32;
typedef double    F64;
#else
   #error Define Types for current compiler
#endif 



typedef U8  Padding8;
typedef U16 Padding16;
typedef U32 Padding32;
typedef U64 Padding64;

#define NULL 0

#define S8_MIN (-128)
#define S16_MIN (-32768)
#define S32_MIN (-2147483647 - 1)
#define S64_MIN  (-9223372036854775807LL - 1)
#define S8_MAX 127
#define S16_MAX 32767
#define S32_MAX 2147483647
#define S64_MAX 9223372036854775807LL

#define U8_MAX  0xffU
#define U16_MAX 0xffffU
#define U32_MAX 0xffffffffU  /* 4294967295U */
#define U64_MAX 0xffffffffffffffffULL /* 18446744073709551615ULL */

#define _BYTE_ 1
#define BYTE(x) (U64)x
#define KB(x)   (U64)(   x  * 1024)
#define MB(x)   (U64)(KB(x) * 1024)
#define GB(x)   (U64)(MB(x) * 1024)
#define TB(x)   (U64)(GB(x) * 1024)


#define _BYTE_ 1
#define KB(x) (U64)(   x  * 1024)
#define MB(x) (U64)(KB(x) * 1024)
#define GB(x) (U64)(MB(x) * 1024)
#define TB(x) (U64)(GB(x) * 1024)

#define DBL_DECIMAL_DIG  17                      // # of decimal digits of rounding precision
#define DBL_DIG          15                      // # of decimal digits of precision
#define DBL_EPSILON      2.2204460492503131e-016 // smallest such that 1.0+DBL_EPSILON != 1.0
#define DBL_HAS_SUBNORM  1                       // type does support subnormal numbers
#define DBL_MANT_DIG     53                      // # of bits in mantissa
#define DBL_MAX          1.7976931348623158e+308 // max value
#define DBL_MAX_10_EXP   308                     // max decimal exponent
#define DBL_MAX_EXP      1024                    // max binary exponent
#define DBL_MIN          2.2250738585072014e-308 // min positive value
#define DBL_MIN_10_EXP   (-307)                  // min decimal exponent
#define DBL_MIN_EXP      (-1021)                 // min binary exponent
#define _DBL_RADIX       2                       // exponent radix
#define DBL_TRUE_MIN     4.9406564584124654e-324 // min positive value

#define FLT_DECIMAL_DIG  9                       // # of decimal digits of rounding precision
#define FLT_DIG          6                       // # of decimal digits of precision
#define FLT_EPSILON      1.192092896e-07F        // smallest such that 1.0+FLT_EPSILON != 1.0
#define FLT_HAS_SUBNORM  1                       // type does support subnormal numbers
#define FLT_GUARD        0
#define FLT_MANT_DIG     24                      // # of bits in mantissa
#define FLT_MAX          3.402823466e+38F        // max value
#define FLT_MAX_10_EXP   38                      // max decimal exponent
#define FLT_MAX_EXP      128                     // max binary exponent
#define FLT_MIN          1.175494351e-38F        // min normalized positive value
#define FLT_MIN_10_EXP   (-37)                   // min decimal exponent
#define FLT_MIN_EXP      (-125)                  // min binary exponent
#define FLT_NORMALIZE    0
#define FLT_RADIX        2                       // exponent radix
#define FLT_TRUE_MIN     1.401298464e-45F        // min positive value

#define LDBL_DIG         DBL_DIG                 // # of decimal digits of precision
#define LDBL_EPSILON     DBL_EPSILON             // smallest such that 1.0+LDBL_EPSILON != 1.0
#define LDBL_HAS_SUBNORM DBL_HAS_SUBNORM         // type does support subnormal numbers
#define LDBL_MANT_DIG    DBL_MANT_DIG            // # of bits in mantissa
#define LDBL_MAX         DBL_MAX                 // max value
#define LDBL_MAX_10_EXP  DBL_MAX_10_EXP          // max decimal exponent
#define LDBL_MAX_EXP     DBL_MAX_EXP             // max binary exponent
#define LDBL_MIN         DBL_MIN                 // min normalized positive value
#define LDBL_MIN_10_EXP  DBL_MIN_10_EXP          // min decimal exponent
#define LDBL_MIN_EXP     DBL_MIN_EXP             // min binary exponent
#define _LDBL_RADIX      _DBL_RADIX              // exponent radix
#define LDBL_TRUE_MIN    DBL_TRUE_MIN            // min positive value

#define DECIMAL_DIG      DBL_DECIMAL_DIG



// Move this to 
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


#define buffer(x, size) char x[KB(size)];

union _HANDLE {
	struct {
#if OS_WINDOWS
		HANDLE win32_h;
#endif
		void* vp;
	};
};
