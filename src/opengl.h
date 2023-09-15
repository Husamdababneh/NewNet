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



#define GL_FRAGMENT_SHADER 0x8B30;
#define GL_VERTEX_SHADER   0x8B31;

/*
 * GLenum  -> u32 
 * GLuint  -> u32
 * GLsizei -> u32
 */



#define PROC_DECLARATION(returnType, name,...)\
    typedef returnType (*name##Proc)(__VA_ARGS__);\
    name##Proc name


PROC_DECLARATION(U32,  glCreateProgram);
PROC_DECLARATION(U32,  glCreateShader, U32);
PROC_DECLARATION(void, glGenBuffers, U32, U32*);
PROC_DECLARATION(void, glShaderSource, U32, U32, const S8, const S32); 
PROC_DECLARATION(void, glCompileShader, U32);
PROC_DECLARATION(void, glGetShaderiv, U32, U32, S32);
PROC_DECLARATION(void, glAttachShader, GLuint program, GLuint shader);

