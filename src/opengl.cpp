/* ========================================================================
   $File: opengl.cpp
   $Date: 2023-09-15
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/

#include "opengl.h"

#pragma warning(disable :4191)

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

inline internal
B8 LoadGLProcs()
{

    LOAD_OPENG_PROC(glCreateProgram);
    LOAD_OPENG_PROC(glCreateShader);
    LOAD_OPENG_PROC(glShaderSource);
    LOAD_OPENG_PROC(glCompileShader);
    LOAD_OPENG_PROC(glGetShaderiv);
    LOAD_OPENG_PROC(glAttachShader);
    LOAD_OPENG_PROC(glUseProgram);
    LOAD_OPENG_PROC(glLinkProgram);

    LOAD_OPENG_PROC(glBindBuffer);
    LOAD_OPENG_PROC(glGenBuffers);
    LOAD_OPENG_PROC(glBufferData);
    LOAD_OPENG_PROC(glNamedBufferData);


    LOAD_OPENG_PROC(glBindVertexArray);
    LOAD_OPENG_PROC(glGenVertexArrays);
    LOAD_OPENG_PROC(glEnableVertexArrayAttrib);
    LOAD_OPENG_PROC(glVertexAttribPointer);

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
        0
    };
    
    HGLRC newContext = wglCreateContextAttribsARB(deviceContext, backend.context, create_attribs);

    wglMakeCurrent(deviceContext, newContext);
    if (!LoadGLProcs())
    {
        ExitProcess(0);
    }

    MessageBoxA(0, glGetString(GL_VERSION), "OPENGL VERSION",0);
    // glCreateProgramProc glCreateProgram = TEST(glCreateProgram, glCreateProgramProc);
    // if (glCreateProgram == nullptr)
    // {
    //     print_string("SHITTTTT \n"_s);
    // }
    //LOAD_OPENG_PROC(glCreateProgram,  glCreateProgramProc);

    //MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
    return backend;
}
