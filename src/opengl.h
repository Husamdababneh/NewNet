/* ========================================================================
   $File: opengl.h
   $Date: 2023-09-15
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once

/*
 * Types Referend: https://www.khronos.org/opengl/wiki/OpenGL_Type

 */

extern "C" {

#define GL_FRAGMENT_SHADER      0x8B30
#define GL_VERTEX_SHADER        0x8B31
#define GL_ARRAY_BUFFER         0x8892
#define GL_STATIC_DRAW          0x88e4
#define GL_ELEMENT_ARRAY_BUFFER 0x8893 
#define GL_FLOAT                0x1406
#define GL_FALSE                0x0
#define GL_COLOR_BUFFER_BIT     0x4000
#define GL_TRIANGLES            0x4
#define GL_UNSIGNED_INT         0x1405
#define GL_VERSION              0x1f02


#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

/*
 * GLenum  -> u32 
 * GLuint  -> u32
 * GLsizei -> u32
 */



#define PROC_DECLARATION(returnType, name,...)\
    typedef returnType (*name##Proc)(__VA_ARGS__);\
    name##Proc name



// Shaders
PROC_DECLARATION(U32,  glCreateProgram);
PROC_DECLARATION(U32,  glCreateShader, U32);
PROC_DECLARATION(void, glShaderSource, U32, U32, const S8** , const S32*);
PROC_DECLARATION(void, glCompileShader, U32);
PROC_DECLARATION(void, glGetShaderiv, U32, U32, S32);
PROC_DECLARATION(void, glAttachShader, U32 program, U32 shader);
PROC_DECLARATION(void, glLinkProgram, U32 program);
PROC_DECLARATION(void, glUseProgram, U32 program);

// Buffers
PROC_DECLARATION(void, glBindBuffer, U32, U32);
PROC_DECLARATION(void, glGenBuffers, U32, U32*);
PROC_DECLARATION(void, glBufferData, U32, U32, U32*, U32);
PROC_DECLARATION(void, glNamedBufferData, U32, U32, const void*, U32);

PROC_DECLARATION(void, glBindVertexArray, U32);
PROC_DECLARATION(void, glGenVertexArrays, U32, U32*);
PROC_DECLARATION(void, glEnableVertexArrayAttrib, U32, U32);
PROC_DECLARATION(void, glVertexAttribPointer, U32, S32, U32, U8, U32, const void *);
PROC_DECLARATION(void, glDrawElements, U32, U32, U32, const void*);
PROC_DECLARATION(void, glClearColor, F32,F32,F32,F32);
PROC_DECLARATION(void, glClear, U32);
PROC_DECLARATION(char*,glGetString, U32);


typedef HGLRC (*wglCreateContextAttribsARBProc)(HDC hDC, HGLRC hshareContext, const int *attribList);

}
