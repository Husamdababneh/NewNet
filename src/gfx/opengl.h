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
#define GL_ELEMENT_ARRAY_BUFFER 0x8893 
#define GL_FLOAT                0x1406
#define GL_FALSE                0x0
#define GL_TRUE                 0x1
#define GL_COLOR_BUFFER_BIT     0x4000
#define GL_VERSION              0x1f02

#define GL_TEXTURE_2D           0xde1
#define GL_TEXTURE_MAG_FILTER   0x2800
#define GL_TEXTURE_MIN_FILTER   0x2801
#define GL_TEXTURE_WRAP_S   	0x2802
#define GL_TEXTURE_WRAP_T   	0x2803

#define GL_REPEAT            	0x2901
#define GL_LINEAR               0x2601

#define GL_TEXTURE0             0x84c0

#define GL_RGB                   0x1907
#define GL_RGBA                  0x1908

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001


// GLenum(GLDrawElementsMode)
enum GLDrawElementsMode : U32
{
    GL_POINTS     = 0x0,
    GL_LINE_STRIP = 0x3,
    GL_TRIANGLES  = 0x4
    /*
      GL_LINE_LOOP ; 
      GL_LINES ; 
      GL_LINE_STRIP_ADJACENCY ; 
      GL_LINES_ADJACENCY ; 
      GL_TRIANGLE_STRIP ; 
      GL_TRIANGLE_FAN ; 
      GL_TRIANGLE_STRIP_ADJACENCY ; 
      GL_TRIANGLES_ADJACENCY ;
      GL_PATCHES ;
    */
};

enum GLDrawElementsType : U32
{
    GL_UNSIGNED_BYTE  = 0x1401,
    GL_UNSIGNED_SHORT = 0x1403,
    GL_UNSIGNED_INT   = 0x1405,
};

enum GLFrontFaceMode : U32
{
    GL_CW  = 0x900,
    GL_CCW = 0x901,
};


enum class GLShaderQueryParam : U32
{
    GL_SHADER_TYPE          = 0x8b4f,
    GL_DELETE_STATUS        = 0x8b80,
    GL_COMPILE_STATUS       = 0x8b81,
    GL_INFO_LOG_LENGTH      = 0x8b84, 
    GL_SHADER_SOURCE_LENGTH = 0x8b88,
};

enum class GLShaderProgramQueryParam : U32
{
    GL_DELETE_STATUS               = 0x8b80,
    GL_LINK_STATUS                 = 0x8b82,
    GL_VALIDATE_STATUS             = 0x8b83,
    GL_INFO_LOG_LENGTH             = 0x8b84, 
    GL_ATTACHED_SHADERS            = 0x8b85,
    GL_ACTIVE_UNIFORMS             = 0x8b86,
    GL_ACTIVE_UNIFORM_MAX_LENGTH   = 0x8b87,
    GL_ACTIVE_ATTRIBUTES           = 0x8b89,
    GL_ACTIVE_ATTRIBUTE_MAX_LENGTH = 0x8b8a,
};

enum class GLBufferUsage : U32
{
    GL_STREAM_DRAW = 0x88e0,
    GL_STREAM_READ = 0x88e1,
    GL_STREAM_COPY = 0x88e2,
    
    GL_STATIC_DRAW = 0x88e4,
    GL_STATIC_READ = 0x88e5,
    GL_STATIC_COPY = 0x88e6,
    
    GL_DYNAMIC_DRAW = 0x88e8,
    GL_DYNAMIC_READ = 0x88e9,
    GL_DYNAMIC_COPY = 0x88ea,
};

#define PROC_DECLARATION(returnType, name,...)\
    typedef returnType (*name##Proc)(__VA_ARGS__);\
    name##Proc name


typedef U32 GLShaderId;
typedef U32 GLShaderProgram;

typedef U32 GLBufferId;
typedef U32 GLVertexArray;


// Shaders
PROC_DECLARATION(GLShaderProgram, glCreateProgram);
PROC_DECLARATION(GLShaderId,      glCreateShader, U32);
PROC_DECLARATION(void, glDeleteShader, GLShaderProgram );
PROC_DECLARATION(void, glShaderSource, GLShaderId, U32, const S8** , const S32*);
PROC_DECLARATION(void, glCompileShader, GLShaderId);
PROC_DECLARATION(void, glGetShaderiv, GLShaderId, GLShaderQueryParam, S32*);
PROC_DECLARATION(void, glGetProgramiv, GLShaderProgram, GLShaderProgramQueryParam, S32*);
PROC_DECLARATION(void, glGetShaderInfoLog, GLShaderId, U32, U32*, U8*);
PROC_DECLARATION(void, glGetProgramInfoLog, GLShaderProgram, U32, U32*, U8*);
PROC_DECLARATION(void, glAttachShader, GLShaderProgram program, GLShaderId shader);
PROC_DECLARATION(void, glLinkProgram, GLShaderProgram program);
PROC_DECLARATION(void, glUseProgram, GLShaderProgram program);
PROC_DECLARATION(void, glDeleteProgram, GLShaderProgram program);


// Buffers
PROC_DECLARATION(void,  glBindBuffer, U32, GLBufferId);
PROC_DECLARATION(void,  glGenBuffers, U32, GLBufferId*);
PROC_DECLARATION(void,  glBufferData, U32, U32, U32*, U32);
PROC_DECLARATION(void,  glNamedBufferData, GLBufferId, U32, const void*, GLBufferUsage);
PROC_DECLARATION(void,  glNamedBufferSubData, GLBufferId, U64, U64, const void*);
PROC_DECLARATION(void,  glGetNamedBufferSubData, GLBufferId, U64, U64, void*);

PROC_DECLARATION(void,  glDeleteVertexArrays, U32, const GLVertexArray*);
PROC_DECLARATION(void,  glDeleteBuffers, U32, const GLVertexArray*);


PROC_DECLARATION(void,  glBindVertexArray, U32);
PROC_DECLARATION(void,  glGenVertexArrays, U32, U32*);
PROC_DECLARATION(void,  glEnableVertexArrayAttrib, U32, U32);
PROC_DECLARATION(void,  glEnableVertexAttribArray, U32);
PROC_DECLARATION(void,  glVertexAttribPointer, U32, S32, U32, U8, U32, const void *);
PROC_DECLARATION(void,  glDrawElements, GLDrawElementsMode, U32, GLDrawElementsType, const void*);
PROC_DECLARATION(void,  glClearColor, F32,F32,F32,F32);
PROC_DECLARATION(void,  glClear, U32);
PROC_DECLARATION(char*, glGetString, U32);

PROC_DECLARATION(void,  glGenTextures, U32, U32*);
PROC_DECLARATION(void,  glBindTexture, U32, U32);
PROC_DECLARATION(void,  glTexParameteri, U32, U32, U32);
PROC_DECLARATION(void,  glTexImage2D, U32, S32, S32, U32, U32, S32, U32, U32, void*);
PROC_DECLARATION(void,  glGenerateMipmap, U32);
PROC_DECLARATION(void,  glActiveTexture, U32);
PROC_DECLARATION(S32 ,  glGetUniformLocation, U32, const char*);

PROC_DECLARATION(void, glUniform1f, S32, F32);
PROC_DECLARATION(void, glUniform2f, S32, F32, F32);
PROC_DECLARATION(void, glUniform3f, S32, F32, F32, F32);
PROC_DECLARATION(void, glUniform4f, S32, F32, F32, F32, F32);
PROC_DECLARATION(void, glUniform1i, S32, S32);
PROC_DECLARATION(void, glUniform2i, S32, S32, S32);
PROC_DECLARATION(void, glUniform3i, S32, S32, S32, S32);
PROC_DECLARATION(void, glUniform4i, S32, S32, S32, S32, S32);
PROC_DECLARATION(void, glUniform1ui, S32, U32);
PROC_DECLARATION(void, glUniform2ui, S32, U32, U32);
PROC_DECLARATION(void, glUniform3ui, S32, U32, U32, U32);
PROC_DECLARATION(void, glUniform4ui, S32, U32, U32, U32, U32);
PROC_DECLARATION(void, glUniform1fv, S32, U32 count, const F32 *);
PROC_DECLARATION(void, glUniform2fv, S32, U32 count, const F32 *);
PROC_DECLARATION(void, glUniform3fv, S32, U32 count, const F32 *);
PROC_DECLARATION(void, glUniform4fv, S32, U32 count, const F32 *);
PROC_DECLARATION(void, glUniform1iv, S32, U32 count, const S32 *);
PROC_DECLARATION(void, glUniform2iv, S32, U32 count, const S32 *);
PROC_DECLARATION(void, glUniform3iv, S32, U32 count, const S32 *);
PROC_DECLARATION(void, glUniform4iv, S32, U32 count, const S32 *);
PROC_DECLARATION(void, glUniform1uiv, S32, U32 count, const U32);
PROC_DECLARATION(void, glUniform2uiv, S32, U32 count, const U32);
PROC_DECLARATION(void, glUniform3uiv, S32, U32 count, const U32);
PROC_DECLARATION(void, glUniform4uiv, S32, U32 count, const U32);
PROC_DECLARATION(void, glUniformMatrix2fv, S32, U32 count, B32 transpose, const F32 *);
PROC_DECLARATION(void, glUniformMatrix3fv, S32, U32 count, B32 transpose, const F32 *);
PROC_DECLARATION(void, glUniformMatrix4fv, S32, U32 count, B32 transpose, const F32 *);
PROC_DECLARATION(void, glUniformMatrix2x3fv, S32, U32 count, B32 transpose, const F32 *);
PROC_DECLARATION(void, glUniformMatrix3x2fv, S32, U32 count, B32 transpose, const F32 *);
PROC_DECLARATION(void, glUniformMatrix2x4fv, S32, U32 count, B32 transpose, const F32 *);
PROC_DECLARATION(void, glUniformMatrix4x2fv, S32, U32 count, B32 transpose, const F32 *);
PROC_DECLARATION(void, glUniformMatrix3x4fv, S32, U32 count, B32 transpose, const F32 *);
PROC_DECLARATION(void, glUniformMatrix4x3fv, S32, U32 count, B32 transpose, const F32 *);

PROC_DECLARATION(void,  glFrontFace, GLFrontFaceMode);
PROC_DECLARATION(void,  glEnable, U32);
PROC_DECLARATION(void,  glViewport, S32, S32, U32, U32);



typedef void (__stdcall *DEBUGPROC)(U32 source,
            U32 type,
            U32 id,
            U32 severity,
            U32  length,
            const char *message,
            const void *userParam);
PROC_DECLARATION(void,  glDebugMessageCallback, DEBUGPROC, const void*);


typedef HGLRC (*wglCreateContextAttribsARBProc)(HDC hDC, HGLRC hshareContext, const int *attribList);

}

/*
 * GLenum  -> u32 
 * GLuint  -> u32
 * GLsizei -> u32
 */
