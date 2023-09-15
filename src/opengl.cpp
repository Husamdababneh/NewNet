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
        MessageBox(NULL, #proc, "Error Loading OpenGL function", MB_OK);\
        return false;\
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
    LOAD_OPENG_PROC(glGenBuffers);
    LOAD_OPENG_PROC(glShaderSource);
    LOAD_OPENG_PROC(glCompileShader);
    LOAD_OPENG_PROC(glGetShaderiv);
    LOAD_OPENG_PROC(glAttachShader);

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

    HDC ourWindowHandleToDeviceContext = GetDC(window);

    int  letWindowsChooseThisPixelFormat;
    letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd); 
    SetPixelFormat(ourWindowHandleToDeviceContext,letWindowsChooseThisPixelFormat, &pfd);

    backend.context = wglCreateContext(ourWindowHandleToDeviceContext);
    wglMakeCurrent(ourWindowHandleToDeviceContext, backend.context);


    if (!LoadGLProcs())
    {
        ExitProcess(0);
    }

    // glCreateProgramProc glCreateProgram = TEST(glCreateProgram, glCreateProgramProc);
    // if (glCreateProgram == nullptr)
    // {
    //     print_string("SHITTTTT \n"_s);
    // }
    //LOAD_OPENG_PROC(glCreateProgram,  glCreateProgramProc);

    //MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
    return backend;
}
