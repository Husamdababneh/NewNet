/* ========================================================================
   $File: math.cpp
   $Date: 2024-02-17
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/


#include "math.h"


internal F32 q_rsqrt(F32 number)
{
    U32 i;
    F32 x2, y;
    constexpr F32 threehalfs = 1.5f;
    constexpr U32 MAGIC = 0x5F3759DF;

    x2 = number * .5f;
    y  = number;
    i  = *(U32 *)&y;
    i  =  MAGIC - ( i >> 1);
    y  = * (F32 *)&i;
    // Newton Method for approx?? 
    y  = y * (threehalfs - (x2 * y * y)) ;
    // y  = y * (threehalfs - (x2 * y * y)) ; 
    return y;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///////////         Vector 2          //////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
#define basic_math_op(op) {vec1.x op vec2.x, vec1.y op vec2.y}
#define vectype base_vec2<type>

template<typename type>
internal vectype operator+(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_math_op(+);
}

template<typename type>
internal vectype operator-(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_math_op(-);
}

template<typename type>
internal vectype operator*(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_math_op(*);
}

#if MATH_MUL_DIV_OPS
template<typename type>
internal vectype operator/(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_math_op(/);
}

template<typename type>
internal vectype operator-(const vectype& vec1)
{
    return {-vec1.x, -vec1.y};
}
#endif// MATH_MUL_DIV_OPS

#undef vectype
#undef basic_binary_op



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///////////         Vector 3          //////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
#define basic_binary_op(op) {vec1.x op vec2.x, vec1.y op vec2.y, vec1.z op vec2.z}
#define vectype base_vec3<type>

template<typename type>
internal vectype operator+(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_binary_op(+);
}

template<typename type>
internal vectype operator-(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_binary_op(-);
}

#if MATH_MUL_DIV_OPS
template<typename type>
internal vectype operator*(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_binary_op(*);
}

template<typename type>
internal vectype operator/(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_binary_op(/);
}
#endif //MATH_MUL_DIV_OPS

template<typename type>
internal type dot(const vectype& vec1,
                  const vectype& vec2)
{
    type result = vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
    return result;
}

template<typename type>
internal vectype cross(const vectype& vec1,
                    const vectype& vec2)
{
    vectype result =
        {
            .x = vec1.y * vec2.z - vec1.z * vec2.y,
            .y = vec1.z * vec2.x - vec1.x * vec2.z,
            .z = vec1.x * vec2.y - vec1.y * vec2.x,          
        };
    return result;
}

template<typename type>
internal vectype operator-(const vectype& vec1)
{
    return {-vec1.x, -vec1.y};
}

#undef vectype
#undef basic_binary_op



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///////////         Vector 4          //////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
#define basic_binary_op(op) {vec1.x op vec2.x, vec1.y op vec2.y, vec1.z op vec2.z, vec1.w op vec2.w}
#define vectype base_vec4<type>

template<typename type>
internal vectype operator+(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_binary_op(+);
}

template<typename type>
internal vectype operator-(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_binary_op(-);
}

#if MATH_MUL_DIV_OPS
template<typename type>
internal vectype operator*(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_binary_op(*);
}

template<typename type>
internal vectype operator/(const vectype& vec1,
                           const vectype& vec2)
{
    return basic_binary_op(/);
}
#endif // MATH_MUL_DIV_OPS

#undef vectype
#undef basic_binary_op


internal Vec4F32 mul(const Mat4F32& mat, const Vec3F32& vec)
{
    return {
        (mat.m00 * vec.x) + (mat.m01 * vec.y) + (mat.m02 * vec.z) + (mat.m03 * 0),
        (mat.m10 * vec.x) + (mat.m11 * vec.y) + (mat.m12 * vec.z) + (mat.m13 * 0),
        (mat.m20 * vec.x) + (mat.m21 * vec.y) + (mat.m22 * vec.z) + (mat.m23 * 0),
        (mat.m20 * vec.x) + (mat.m21 * vec.y) + (mat.m22 * vec.z) + (mat.m23 * 0),
    };
}

internal Vec4F32 mul(const Mat4F32& mat, const Vec4F32& vec)
{
    return {
        (mat.m00 * vec.x) + (mat.m01 * vec.y) + (mat.m02 * vec.z) + (mat.m03 * vec.w),
        (mat.m10 * vec.x) + (mat.m11 * vec.y) + (mat.m12 * vec.z) + (mat.m13 * vec.w),
        (mat.m20 * vec.x) + (mat.m21 * vec.y) + (mat.m22 * vec.z) + (mat.m23 * vec.w),
        (mat.m30 * vec.x) + (mat.m31 * vec.y) + (mat.m32 * vec.z) + (mat.m33 * vec.w),
    };
}

internal Mat4F32 translate(const Mat4F32& mat, const Vec3F32& translation)
{
    Mat4F32 result = mat;
    result.m03 = translation.x;
    result.m13 = translation.y;
    result.m23 = translation.z;
    return result;
}

internal Mat4F32 create_ortho(const F32 left,   const F32 right,
                              const F32 bottom, const F32 top,
                              const F32 n,      const F32 far)
{
    auto rlm = right - left;
    auto rlp = right + left;

    auto tbm = bottom - top;
    auto tbp = bottom + top;

    auto fnm = far - n;
    auto fnp = far + n;
    
    return {
        2.f/rlm,     .0f,      .0f, -(rlp/rlm),
            .0f, 2.f/tbm,      .0f, -(tbp/tbm),
            .0f,     .0f, -2.f/fnm,   -(n/fnm),
            .0f,     .0f,      .0f,       1.0f,
    };
}


internal Mat4F32 create_ortho(const F32 width, const F32 height, const F32 depth)
{
    F32 w = 2.f/width;
    F32 h = 2.f/height;
    F32 d = 2.f/depth;
    return {
          w, .0f, .0f, -1.0f,
        .0f,   h, .0f, -1.0f,
        .0f, .0f,  -d,  1.0f,
        .0f, .0f, .0f,  1.0f,
    };
}


internal Mat4F32 create_ortho_new(const F32 left,   const F32 right,
                                  const F32 bottom, const F32 top,  
                                  const F32 n,      const F32 far)
{
    auto rlm = right - left;     
    auto rlp = right + left;

    auto tbm = bottom - top;
    auto tbp = bottom + top;

    auto fnm = far - n;
    auto fnp = far + n;
    
    return {
        2.f/rlm,     .0f,      .0f, .0f,
        .0f, 2.f/tbm,      .0f,  .0f,
        .0f,     .0f, -2.f/fnm,   .0f,
        -(rlp/rlm),    -(tbp/tbm),      -(n/fnm),       1.0f,
    };
}



#if 0
constexpr Vec2U8 Vec2U8_0 = {0};
constexpr Vec2U16 Vec2U16_0 = {0};
constexpr Vec2U32 Vec2U32_0 = {0};
constexpr Vec2U64 Vec2U64_0 = {0};
constexpr Vec2S8 Vec2S8_0 = {0};
constexpr Vec2S16 Vec2S16_0 = {0};
constexpr Vec2S32 Vec2S32_0 = {0};
constexpr Vec2S64 Vec2S64_0 = {0};
constexpr Vec2F32 Vec2F32_0 = {0};
constexpr Vec2F64 Vec2F64_0 = {0};
constexpr Vec3U8 Vec3U8_0 = {0};
constexpr Vec3U16 Vec3U16_0 = {0};
constexpr Vec3U32 Vec3U32_0 = {0};
constexpr Vec3U64 Vec3U64_0 = {0};
constexpr Vec3S8 Vec3S8_0 = {0};
constexpr Vec3S16 Vec3S16_0 = {0};
constexpr Vec3S32 Vec3S32_0 = {0};
constexpr Vec3S64 Vec3S64_0 = {0};
constexpr Vec3F32 Vec3F32_0 = {0};
constexpr Vec3F64 Vec3F64_0 = {0};
constexpr Vec4U8 Vec4U8_0 = {0};
constexpr Vec4U16 Vec4U16_0 = {0};
constexpr Vec4U32 Vec4U32_0 = {0};
constexpr Vec4U64 Vec4U64_0 = {0};
constexpr Vec4S8 Vec4S8_0 = {0};
constexpr Vec4S16 Vec4S16_0 = {0};
constexpr Vec4S32 Vec4S32_0 = {0};
constexpr Vec4S64 Vec4S64_0 = {0};
constexpr Vec4F32 Vec4F32_0 = {0};
constexpr Vec4F64 Vec4F64_0 = {0};
#endif
