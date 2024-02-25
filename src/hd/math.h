/* ========================================================================
   $File: math.h
   $Date: 2024-02-17
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once


#define MATH_MUL_DIV_OPS 1

template<typename type>
struct base_vec2 {
    union {
        struct { type a,b; };
        struct { type x,y; };
    };
};

template<typename type>
struct base_vec3 {
    union {
        struct { type x,y,z; };
        struct { type r,g,b; };
    };
};

template<typename type>
struct base_vec4 {
    union {
        struct { type x,y,z,w; };
        struct { type r,g,b,a; };
    };
};

typedef base_vec2<U8>  Vec2U8;
typedef base_vec2<U16> Vec2U16;
typedef base_vec2<U32> Vec2U32;
typedef base_vec2<U64> Vec2U64;

typedef base_vec2<S8>  Vec2S8;
typedef base_vec2<S16> Vec2S16;
typedef base_vec2<S32> Vec2S32;
typedef base_vec2<S64> Vec2S64;

typedef base_vec2<F32> Vec2F32;
typedef base_vec2<F64> Vec2F64;


typedef base_vec3<U8>  Vec3U8;
typedef base_vec3<U16> Vec3U16;
typedef base_vec3<U32> Vec3U32;
typedef base_vec3<U64> Vec3U64;

typedef base_vec3<S8>  Vec3S8;
typedef base_vec3<S16> Vec3S16;
typedef base_vec3<S32> Vec3S32;
typedef base_vec3<S64> Vec3S64;

typedef base_vec3<F32> Vec3F32;
typedef base_vec3<F64> Vec3F64;


typedef base_vec4<U8>  Vec4U8;
typedef base_vec4<U16> Vec4U16;
typedef base_vec4<U32> Vec4U32;
typedef base_vec4<U64> Vec4U64;

typedef base_vec4<S8>  Vec4S8;
typedef base_vec4<S16> Vec4S16;
typedef base_vec4<S32> Vec4S32;
typedef base_vec4<S64> Vec4S64;

typedef base_vec4<F32> Vec4F32;
typedef base_vec4<F64> Vec4F64;
