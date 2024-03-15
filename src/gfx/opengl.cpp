/* ========================================================================
   $File: opengl.cpp
   $Date: 2023-09-15
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/

#include "opengl.h"

//#pragma warning(disable :4191)

// @CleanUp(husamd): Clean this shit up
#define LOAD_OPENG_PROC(proc)\
    proc = reinterpret_cast<proc##Proc>(wglGetProcAddress(#proc));  \
    if (proc == nullptr)\
    {\
        DWORD error_code = GetLastError();      \
        char buffer[256] = {};                                          \
        sprintf(buffer, #proc"->[%d]\n",error_code);                             \
        MessageBox(NULL, buffer, "Error While Loading OpenGL Proc", MB_OK); \
        return false;                                                   \
    }

#define LOAD_OPENG_PROC_1_1(lib, proc)                            \
    proc = reinterpret_cast<proc##Proc>(GetProcAddress(lib, #proc)); \
    if (proc == nullptr)\
    {\
        DWORD error_code = GetLastError();      \
        char buffer[256] = {};                                          \
        sprintf(buffer, #proc"->[%d]\n",error_code);                             \
        MessageBox(NULL, buffer, "Error While Loading OpenGL Proc", MB_OK); \
        return false;                                                   \
    }

template<typename Signature>
inline internal
Signature Load_OPENGL_PROC(const char* name)
{
    return reinterpret_cast<Signature>(wglGetProcAddress(name));
}


#define TEST(x,y) Load_OPENGL_PROC<y>(#x)

/*
GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR	0x824d
GL_DEBUG_TYPE_ERROR	0x824c
GL_DEBUG_TYPE_MARKER	0x8268
GL_DEBUG_TYPE_OTHER	0x8251
GL_DEBUG_TYPE_PERFORMANCE	0x8250
GL_DEBUG_TYPE_POP_GROUP	0x826a
GL_DEBUG_TYPE_PORTABILITY	0x824f
GL_DEBUG_TYPE_PUSH_GROUP	0x8269
GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR	0x824e
*/


char* getMessageType(U32 type)
{
    char* message = (char*)"UNKNOWN";
    switch(type)
    {
      case 0x824d:
      {
          message = (char*)"Deprecated Behavior";
      }break;
      case 0x824c:
      {
          message = (char*)"Error";
      }break;
      case 0x8268:
      {
          message = (char*)"Marker";
      }break;
      case 0x8251:
      {
          message = (char*)"Other";
      }break;
      case 0x8250:
      {
          message = (char*)"Performance";
      }break;
      case 0x826a:
      {
          message = (char*)"Pop Group";
      }break;
      case 0x824f:
      {
          message = (char*)"Portability";
      }break;
      case 0x8269:
      {
          message = (char*)"Push Group";
      }break;
      case 0x824e:
      {
          message = (char*)"Undefined Behavior";
      }break;
    }
    return message;
}

char* getMessageSeverity(U32 severity)
{
    if (severity == 0x826b)
    {
        return (char*)"Notification";
    }
    char* message = (char*)"UNKNOWN";
    switch(severity)
    {
      case 37190:
      {
          message = (char*)"High";
      }break;
      case 37191:
      {
          message = (char*)"Medium";
      }break;
      case 37193:
      {
          message = (char*)"Low";
      }break;
    }
    return message;
}

void __stdcall
MessageCallback( U32 source,
                 U32 type,
                 U32 id,
                 U32 severity,
                 U32 length,
                 const char* message,
                 const void* userParam )
{
    // GL_DEBUG_TYPE_ERROR 0x824c
    if (0x826b == severity) return;
    const char formatString[] = "GL CALLBACK: [%s][%s]: %s\n";
    constexpr U32 buff_size = 1024;
    char buffer[buff_size] = {};
    auto size = snprintf(buffer, buff_size, formatString,
                         getMessageSeverity(severity),
                         getMessageType(type),
                         message );
    String str = cstr_to_string(buffer, (U64)size);
    print_string(str);
    ExitProcess(0);
}

internal B8 load_uniform_procs();

inline internal
B8 LoadGLProcs()
{

    LOAD_OPENG_PROC(glCreateProgram);
    LOAD_OPENG_PROC(glCreateShader);
    LOAD_OPENG_PROC(glShaderSource);
    LOAD_OPENG_PROC(glCompileShader);
    LOAD_OPENG_PROC(glGetShaderiv);
    LOAD_OPENG_PROC(glGetShaderInfoLog);
    LOAD_OPENG_PROC(glGetProgramiv);
    LOAD_OPENG_PROC(glGetProgramInfoLog);
    LOAD_OPENG_PROC(glAttachShader);
    LOAD_OPENG_PROC(glUseProgram);
    LOAD_OPENG_PROC(glDeleteProgram);
    LOAD_OPENG_PROC(glLinkProgram);
    LOAD_OPENG_PROC(glDeleteShader);


    
    LOAD_OPENG_PROC(glBindBuffer);
    LOAD_OPENG_PROC(glGenBuffers);
    LOAD_OPENG_PROC(glBufferData);
    LOAD_OPENG_PROC(glNamedBufferData);
    LOAD_OPENG_PROC(glNamedBufferSubData);
    LOAD_OPENG_PROC(glDeleteVertexArrays);
    LOAD_OPENG_PROC(glDeleteBuffers);


    LOAD_OPENG_PROC(glBindVertexArray);
    LOAD_OPENG_PROC(glGenVertexArrays);
    LOAD_OPENG_PROC(glEnableVertexArrayAttrib);
    LOAD_OPENG_PROC(glEnableVertexAttribArray);
    LOAD_OPENG_PROC(glVertexAttribPointer);


    LOAD_OPENG_PROC(glGenTextures);
    LOAD_OPENG_PROC(glBindTexture);

    
    LOAD_OPENG_PROC(glGenerateMipmap);
    LOAD_OPENG_PROC(glActiveTexture);


    LOAD_OPENG_PROC(glGetUniformLocation);
    load_uniform_procs();
    LOAD_OPENG_PROC(glDebugMessageCallback); 
    
    HMODULE lib = LoadLibraryA("opengl32.dll");
    if (lib == NULL)
    {
        DWORD error_code = GetLastError();                              
        char buffer[256] = {};                                          
        sprintf(buffer, "LoadGlProcs->[%d]\n",error_code);                    
        MessageBox(NULL, buffer, "Error While Loading OpenGL Proc", MB_OK);
        return false;

    }
    LOAD_OPENG_PROC_1_1(lib, glDrawElements);
    LOAD_OPENG_PROC_1_1(lib, glClearColor);
    LOAD_OPENG_PROC_1_1(lib, glClear);
    LOAD_OPENG_PROC_1_1(lib, glGetString);


    
    LOAD_OPENG_PROC_1_1(lib, glTexParameteri);
    LOAD_OPENG_PROC_1_1(lib, glTexImage2D);

    LOAD_OPENG_PROC_1_1(lib, glFrontFace);
    LOAD_OPENG_PROC_1_1(lib, glEnable);
    LOAD_OPENG_PROC_1_1(lib, glViewport);

    
    return true;
}

internal 
OpenGLBackend createOpenGLBackend(HWND window)
{
    OpenGLBackend backend;
    PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
            PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
            32,                   // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                   // Number of bits for the depthbuffer
            8,                    // Number of bits for the stencilbuffer
            0,                    // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

    HDC deviceContext = GetDC(window);

    int  letWindowsChooseThisPixelFormat = ChoosePixelFormat(deviceContext, &pfd);
    SetPixelFormat(deviceContext, letWindowsChooseThisPixelFormat, &pfd);

    backend.context = wglCreateContext(deviceContext);
    wglMakeCurrent(deviceContext, backend.context);

    auto wglCreateContextAttribsARB = (wglCreateContextAttribsARBProc)
        wglGetProcAddress("wglCreateContextAttribsARB");

    int const create_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_FLAGS_ARB, 0,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    
    HGLRC newContext = wglCreateContextAttribsARB(deviceContext, backend.context, create_attribs);

    wglMakeCurrent(deviceContext, newContext);
    if (!LoadGLProcs())
    {
        ExitProcess(0);
    }


    // 	GL_DEBUG_OUTPUT 0x92e0
    // 	GL_DEBUG_OUTPUT 0x92e0
    glEnable( 0x92e0 );
    glDebugMessageCallback( MessageCallback, 0 );

    //MessageBoxA(0, glGetString(GL_VERSION), "OPENGL VERSION",0);
    // glCreateProgramProc glCreateProgram = TEST(glCreateProgram, glCreateProgramProc);
    // if (glCreateProgram == nullptr)
    // {
    //     print_string("SHITTTTT \n"_s);
    // }
    //LOAD_OPENG_PROC(glCreateProgram,  glCreateProgramProc);

    //MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
    return backend;
}


internal B8 load_uniform_procs()
{
    LOAD_OPENG_PROC(glUniform1f);
    LOAD_OPENG_PROC(glUniform2f);
    LOAD_OPENG_PROC(glUniform3f);
    LOAD_OPENG_PROC(glUniform4f);
    LOAD_OPENG_PROC(glUniform1i);
    LOAD_OPENG_PROC(glUniform2i);
    LOAD_OPENG_PROC(glUniform3i);
    LOAD_OPENG_PROC(glUniform4i);
    LOAD_OPENG_PROC(glUniform1ui);
    LOAD_OPENG_PROC(glUniform2ui);
    LOAD_OPENG_PROC(glUniform3ui);
    LOAD_OPENG_PROC(glUniform4ui);
    LOAD_OPENG_PROC(glUniform1fv);
    LOAD_OPENG_PROC(glUniform2fv);
    LOAD_OPENG_PROC(glUniform3fv);
    LOAD_OPENG_PROC(glUniform4fv);
    LOAD_OPENG_PROC(glUniform1iv);
    LOAD_OPENG_PROC(glUniform2iv);
    LOAD_OPENG_PROC(glUniform3iv);
    LOAD_OPENG_PROC(glUniform4iv);
    LOAD_OPENG_PROC(glUniform1uiv);
    LOAD_OPENG_PROC(glUniform2uiv);
    LOAD_OPENG_PROC(glUniform3uiv);
    LOAD_OPENG_PROC(glUniform4uiv);
    LOAD_OPENG_PROC(glUniformMatrix2fv);
    LOAD_OPENG_PROC(glUniformMatrix3fv);
    LOAD_OPENG_PROC(glUniformMatrix4fv);
    LOAD_OPENG_PROC(glUniformMatrix2x3fv);
    LOAD_OPENG_PROC(glUniformMatrix3x2fv);
    LOAD_OPENG_PROC(glUniformMatrix2x4fv);
    LOAD_OPENG_PROC(glUniformMatrix4x2fv);
    LOAD_OPENG_PROC(glUniformMatrix3x4fv);
    LOAD_OPENG_PROC(glUniformMatrix4x3fv);
    return true;
}
