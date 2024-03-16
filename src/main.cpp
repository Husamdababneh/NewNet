/* ========================================================================
   $File: main.cpp
   $Date: 2023-02-27
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Entry point
   ========================================================================*/

#include "main.h"
#include "gfx/opengl.cpp"
#include "hd/image.cpp"

#include <immintrin.h>

global_variable B8 running = true;
global_variable OpenGLBackend backEnd = {};
struct Win32_OffscreenBuffer {
    BITMAPINFO info;
    char       padding[4];
    void*      memory;
    U64        width, height;
    U64        stride, bytePerPixel;
};


struct WindowDimentions
{
    U64 width, height;
};

global_variable Mat4F32 proj;
global_variable Win32_OffscreenBuffer offscreenBuffer;

B32 checkShaderProgramStatus(GLShaderProgram program, GLShaderProgramQueryParam param);
B32 checkShaderStatus(GLShaderId shader, GLShaderQueryParam param);

BOOL ctrl_handler(DWORD event)
{
    if (event == CTRL_CLOSE_EVENT || event == CTRL_C_EVENT || event == CTRL_BREAK_EVENT ) {
        // @CleanUp(husamd): This will shutdown the proces gracfully fornow,
        //                   in the future we need to detach ourself from the console only
        //                   so we can re open it when we want (ex: command to open/close console window)
        running = false;
        return TRUE;
    }
    return FALSE;
}

inline internal
B8 InializeWin32Console()
{
    BOOL success = AllocConsole();
    if (!success)
    {
        return false;
    }

    HANDLE hConOut = CreateFile("CONOUT$",
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)(ctrl_handler), TRUE);
    return true;
}

//- 

struct Pixel{
    U32 b,g,r,padding;
};

inline internal 
WindowDimentions win32_get_window_dimentions(HWND window)
{
    WindowDimentions result = {};
    RECT windowRect;
    GetWindowRect(window, &windowRect);
    
    result.width  = (U64)windowRect.right  - (U64)windowRect.left;
    result.height = (U64)windowRect.bottom - (U64)windowRect.top;

    return result;
}




LRESULT CALLBACK
MainWindowCallback(HWND   hWindow,
                   UINT   message,
                   WPARAM wParam,
                   LPARAM lParam)
{
    (void)wParam;
    (void)lParam;
    (void)hWindow;

    // char buffer[255] = {0};
    // int size = wsprintfA(buffer, "Message Code: %d\n", message);
    // print_string(String{(U64)size, buffer});
    

    //bool handled = false;
    LRESULT result = 0;
    // TODO: Handle each case as documented in win32 documentation since for example,
    //       WM_GETICON must return a handle to the icon
    //       WM_CREATE  must return 0 to continue creaing the window, -1 will destroy the window... etc
    switch(message)
    {
        /*case WM_CREATE:
      {
          
      }break;*/
      case WM_CLOSE:
      {
          print_string("WM_CLOSE\n"_s);
          //handled = true;
          running = false;
      } break;
      case WM_PAINT:
      {
          result = DefWindowProc(hWindow, message, wParam, lParam);
      } break;
      case WM_SIZE:
      {
          if (backEnd.context) glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
          proj = create_ortho_new(0.0f, LOWORD(lParam),
                                  HIWORD(lParam), 0.0f,
                                  -1.0f, 1.0f);
      }break;
      default:
      {
          result = DefWindowProc(hWindow, message, wParam, lParam);
      }break;
    }

    //result = DefWindowProc(hWindow, message, wParam, lParam);
    return result;
}



void title_fps_counter(
    HWND window,
    LARGE_INTEGER& start_counter,
    LARGE_INTEGER& frequency)
{
    LARGE_INTEGER end_counter, counts, fps, ms;
    QueryPerformanceCounter(&end_counter);
  
    counts.QuadPart = end_counter.QuadPart - start_counter.QuadPart;
    start_counter = end_counter;
        
    fps.QuadPart = frequency.QuadPart / counts.QuadPart;
    ms.QuadPart = ((1000 * counts.QuadPart) / frequency.QuadPart);
        
    U8 buf[100] = {};    
    DWORD_PTR pArgs[] = {
        (DWORD_PTR)counts.QuadPart,
        (DWORD_PTR)frequency.QuadPart,
        (DWORD_PTR)fps.QuadPart,
        (DWORD_PTR)ms.QuadPart        
    };

    const char* formatString = "counts:%lld| frequency:%lld| fps:%lld| ms:%lld";

    snprintf((char*)buf, 100, formatString,
             counts.QuadPart,   
             frequency.QuadPart,
             fps.QuadPart,      
             ms.QuadPart);

        
     
    SetWindowTextA(window, (LPCSTR)buf);
}

const char *vertexShaderSource = R"str(
    #version 430 core
    layout (location = 0) in vec3 aPos;

    layout(location = 0) uniform mat4 proj;
    layout(location = 1) uniform mat4 view;

    void main()
    {
       gl_Position = proj*vec4(aPos.xyz, 1.0);
    }
)str";

const char *vertexShaderSource2 = R"str(
    #version 430 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 uv;

    layout(location = 0) uniform mat4 proj;
    layout(location = 1) uniform mat4 view;

    void main()
    {
       gl_Position = proj*vec4(aPos.xyz, 1.0);
    }
)str";

const char *fragmentShaderSource = R"str(
    #version 430 core
    out vec4 FragColor;
    void main()
    {
       FragColor = vec4(.5f, 0.336f, 0.25f, 1.0f);
    }
)str";


struct Item
{
    GLBufferId vbo, ebo;
    GLVertexArray vao;
    GLShaderProgram shaderProgram;
};

#include "gfx/ui.h"

#define FORI(x) for (U64 i = 0; i < x; ++i)

GLGui init_gl_gui()
{
    constexpr U64 MAX_RECT_COUNT       = 2000;
    constexpr U64 VERTICIES_ARRAY_SIZE = sizeof(F32) * 3 * 4 * MAX_RECT_COUNT;// 5 comps, 4 vetecies, 2000 RECT
    constexpr U64 INDECIES_ARRAY_SIZE  = sizeof(U32) * 3 * 2 * MAX_RECT_COUNT;// 3 comp,  2 faces, 2000 RECT
    constexpr U64 RECT_SIZE            = sizeof(BackEndRect);
    constexpr U64 MEMORY_NEEDED        = RECT_SIZE * MAX_RECT_COUNT;
    GLGui gui;  
    gui.data.rect_count = 0;
    gui.data.rects      = arena_alloc__sized(MEMORY_NEEDED, MEMORY_NEEDED);

    glGenVertexArrays(1, &gui.vao);
    glGenBuffers(1, &gui.vbo);
    glGenBuffers(1, &gui.ebo);

    glBindVertexArray(gui.vao);

    glBindBuffer(GL_ARRAY_BUFFER, gui.vbo);
    glNamedBufferData(gui.vbo, VERTICIES_ARRAY_SIZE, NULL, GLBufferUsage::GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui.ebo);
    glNamedBufferData(gui.ebo, INDECIES_ARRAY_SIZE, NULL, GLBufferUsage::GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(F32), (void*)0);
    glEnableVertexAttribArray(0);
    
    /*
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)(3 * sizeof(F32)));
      glEnableVertexAttribArray(1);
    */

    // I can fill all the indecies data from the start since they will follow the same pattern no matter what
    // @Cleanup(husamd): Genereate the following data on the GPU using a compute shader
    {
        U32 tempIndecies[MAX_RECT_COUNT * 6];
        for (U64 i = 0, u = 0, r=0; r < MAX_RECT_COUNT; u += 4, i += 6, ++r)
        {
            tempIndecies[i + 0] = 0 + u;
            tempIndecies[i + 1] = 1 + u;
            tempIndecies[i + 2] = 2 + u;
            tempIndecies[i + 3] = 2 + u;
            tempIndecies[i + 4] = 3 + u;
            tempIndecies[i + 5] = 0 + u;
        }
        glNamedBufferSubData(gui.ebo, 0, INDECIES_ARRAY_SIZE, tempIndecies);
    }
    
    // Shaders
    // vertex shader
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const S8**)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    auto result = checkShaderStatus(vertexShader, GLShaderQueryParam::GL_COMPILE_STATUS);
    if (result == GL_FALSE){
        ExitProcess(1);
    }
    // fragment shader
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const S8**)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    result = checkShaderStatus(fragmentShader, GLShaderQueryParam::GL_COMPILE_STATUS);

    
    gui.shaderProgram  = glCreateProgram();
    glAttachShader(gui.shaderProgram, vertexShader);
    glAttachShader(gui.shaderProgram, fragmentShader);
    glLinkProgram(gui.shaderProgram);
    result = checkShaderProgramStatus(gui.shaderProgram, GLShaderProgramQueryParam::GL_LINK_STATUS);
    
    if (result == false)
    {
        ExitProcess(1);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    
    return gui;
};

void push_rect(GLGui* gui, F32 w, F32 h, F32 x, F32 y)
{
    // @NOTE(husamd): In fact i do not need to save the data of the verticies on the cpu-ram
    //                I can get the data if i want using glGetNamedBufferSubData

    F32 vertices[] = {
          x,   y+h, 0.0f, // top left 
        x+w,   y+h, 0.0f, // top right
        x+w,     y, 0.0f, // bottom right
          x,     y, 0.0f, // bottom left
    };
    
    constexpr U64 vertices_count = countof(vertices);
    //gui->data.vertices_count += vertices_count;
    
    const U32 i = gui->data.rect_count * 4;

    // SIMD?
    /*
    U32 indices[] = {  // note that we start from 0!
        0 + i, 1 + i, 2 + i,   // first triangle
        2 + i, 3 + i, 0 + i    // second triangle
    };
    */   
    //constexpr U64 indices_count = 6;//countof(indices);

    //gui->data.indices_count += indices_count;

    // Update the VBO and IBO data
    const U64 v_idx = gui->data.rect_count * sizeof(vertices);
    //const U64 i_idx = gui->data.rect_count * sizeof(indices);
    
    glNamedBufferSubData(gui->vbo, v_idx, sizeof(vertices), vertices);

    
    // glNamedBufferSubData(gui->ebo, i_idx, sizeof(indices), indices);

    gui->data.rect_count += 1;

    //gui
};


B32 checkShaderStatus(GLShaderId shader, GLShaderQueryParam param)
{
    S32 success;
    glGetShaderiv(shader, param, &success);

    if (GL_TRUE == success)
    {
        return true;
    }

    constexpr U32 buffer_len = 256;
    U32 length;
    U8 buffer[buffer_len] = {};
    glGetShaderInfoLog(shader, buffer_len, &length, buffer);

    String str = cstr_to_string((char*)buffer, (U64)length);
    print_string(str);
    return false;
}

B32 checkShaderProgramStatus(GLShaderProgram program, GLShaderProgramQueryParam param)
{
    S32 success;
    glGetProgramiv(program, param, &success);

    if (success == GL_TRUE)
    {
        return true;
    }

    constexpr U32 buffer_len = 256;
    U32 length;
    U8 buffer[buffer_len] = {};
    glGetProgramInfoLog(program, buffer_len, &length, buffer);

    String str = cstr_to_string((char*)buffer, (U64)length);
    print_string(str);
    return false;
}


//inline
#pragma auto_inline(off)
internal
Item init_object()
{
    Item obj = {};

    /*
    F32 vertices[] = {
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top right
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f   // top left 
    };
    */

    F32 vertices[] = {
             0.f, 100.f, 0.0f, 1.0f, 0.0f,  // top left 
           100.f, 100.f, 0.0f, 1.0f, 1.0f,  // top right
           100.f,   0.f, 0.0f, 1.0f, 0.0f,  // bottom right
             0.f,   0.f, 0.0f, 1.0f, 0.0f,  // bottom left
    };


    
    U32 indices[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
        2, 3, 0    // second triangle
    };


    // Generate Vertex Arrays 
    glGenVertexArrays(1, &obj.vao);
    // Generate Normal(memory space) Buffers
    glGenBuffers(1, &obj.vbo);
    glGenBuffers(1, &obj.ebo);

    // Populate Generated buffers 
    glBindVertexArray(obj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glNamedBufferData(obj.vbo, sizeof(vertices), vertices, GLBufferUsage::GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo);
    glNamedBufferData(obj.ebo, sizeof(indices), indices, GLBufferUsage::GL_STATIC_DRAW);
    
    // We don't have glNamedVertexAttribPointer so we need to bind it before using it 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)(3 * sizeof(F32)));
    glEnableVertexAttribArray(1);


    // Shaders

    // vertex shader
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const S8**)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    auto result = checkShaderStatus(vertexShader, GLShaderQueryParam::GL_COMPILE_STATUS);
    if (result == GL_FALSE){
        ExitProcess(1);
    }
    // fragment shader
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const S8**)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    result = checkShaderStatus(fragmentShader, GLShaderQueryParam::GL_COMPILE_STATUS);

    
    obj.shaderProgram  = glCreateProgram();
    glAttachShader(obj.shaderProgram, vertexShader);
    glAttachShader(obj.shaderProgram, fragmentShader);
    glLinkProgram(obj.shaderProgram);
    result = checkShaderProgramStatus(obj.shaderProgram, GLShaderProgramQueryParam::GL_LINK_STATUS);
    
    if (result == false)
    {
        ExitProcess(1);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    
    return obj;
}
#pragma auto_inline(on)

void free_buffers(Item obj)
{
    glDeleteProgram(obj.shaderProgram);
    
    glDeleteBuffers(1, &obj.vbo);
    glDeleteBuffers(1, &obj.ebo);
    glDeleteVertexArrays(1, &obj.vao);
      
}


U32 compute_requested_memory_size(PNG_FILE* image)
{
    // TODO: 
    constexpr auto channels = 4;
    return (image->width * image->height * (image->bit_depth) * channels);
}


int  main()
{
    Arena* frame_arena;
    global_arena = arena_alloc__sized(MB(256), KB(64));
    frame_arena  =  arena_alloc__sized(MB(16), KB(8));
    
    AddVectoredExceptionHandler(1, ExceptionHandler);
    
    HMODULE instance = GetModuleHandleA(0);
    LPSTR commandLineArguments = GetCommandLineA();
    InializeWin32Console();
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    // @CleanUp(husamd):
    (commandLineArguments);
    (console);


    // Register Class
    WNDCLASS wc      = {0};
    wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW /*Redraw if size changes*/;
    wc.lpfnWndProc   = MainWindowCallback;
    wc.hInstance     = instance;
    wc.lpszClassName = "NewNetWindowClass";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); // override WM_SETCURSOR to change the shape of the cursor  
    
    RegisterClassA(&wc);
    // Create the window 
    HWND window = CreateWindowExA(0,
                                  wc.lpszClassName,
                                  "Window Name",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  1920/2, //CW_USEDEFAULT,
                                  1080/2, //CW_USEDEFAULT,
                                  0,
                                  0,
                                  instance,
                                  0);

    if(!window)
    {
        // TODO(Husam):Log Here
        ExitProcess(0);
    }

    // Choose a graphics backend

    // NOTE(husamd): OpenGL For now 
    // Create openGL context
    
    backEnd = createOpenGLBackend(window);
    WindowDimentions dimentions = win32_get_window_dimentions(window);
    glViewport(0, 0, dimentions.width, dimentions.height);
    // TODO: Treat this as a scratch buffer 
        
    LARGE_INTEGER start_counter, frequency;
    
    QueryPerformanceCounter(&start_counter);
    QueryPerformanceFrequency(&frequency);
    HDC windowContext = GetDC(window);
    
    //Item obj = init_object();
    proj = create_ortho_new(0.0f, dimentions.width,
                            dimentions.height, 0.0f,
                            -1.0f, 1.0f);
    Mat4F32 view =  {
        .m00 = 1.f,
        .m11 = 1.f,
        .m22 = 1.f,
        .m33 = 1.f,
    };

    //view = translate(view, {0.f, 0.f, 0.f});
    
    (view);
    
    U32 texture1;
    S32 uniformLocation;
    //glFrontFace(GL_CW);

    GLGui gui = init_gl_gui();
    push_rect(&gui, 100.f, 100.f, 200.f, 200.f);
    push_rect(&gui, 200.f, 200.f, 0.f, 0.f);
    push_rect(&gui, 200.f, 200.f, 300.f, 300.f);
    
    S32 projMatrixLocation = glGetUniformLocation(gui.shaderProgram, "proj");
    S32 viewMatrixLocation = glGetUniformLocation(gui.shaderProgram, "view");
    
    while(running)
    {
        MSG message;
        while(PeekMessageA(&message, window, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&message); 
            DispatchMessage(&message);
        }

        //title_fps_counter(window, start_counter, frequency);

        //render(window, &offscreenBuffer);
        glClearColor(0.5f, 0.8f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, texture1);

        // seeing as we only have a single VAO there's no need to bind it every time,
        // but we'll do so to keep things a bit more organized
        /*
        glBindVertexArray(obj.vao);
        glUseProgram(obj.shaderProgram);
        glUniformMatrix4fv(projMatrixLocation, 1, false, &proj.m00);
        glUniformMatrix4fv(viewMatrixLocation, 1, false, &view.m00);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); */

        glBindVertexArray(gui.vao);
        glUseProgram(gui.shaderProgram);
        glUniformMatrix4fv(projMatrixLocation, 1, false, &proj.m00);
        glUniformMatrix4fv(viewMatrixLocation, 1, false, &view.m00);
        
        glDrawElements(GL_TRIANGLES, 6*gui.data.rect_count, GL_UNSIGNED_INT, nullptr);
        
        
        SwapBuffers(windowContext);
        arena_clear(frame_arena);
    }
    
    // delete our buffers
    //free_buffers(obj);

    
    // NOTE(husamd): I wish defer is a thing in c/c++
    {
        
        // Unnecessary; wglDeleteContext will make the context not current
        //wglMakeCurrent(backEnd.context, NULL);

        wglDeleteContext(backEnd.context);
        PostQuitMessage(0);
    }

    DestroyWindow(window);
    FreeConsole();

    arena_release(global_arena);
    arena_release(frame_arena);
    ExitProcess(0);
}


#if 0
    {
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // set the texture wrapping parameters
        // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        File file = open_file("../assets/tests/8x8_uncompressed-no-interlace.png"_s8);
        if (!file.handle)
        {
            print_string("Cannot find file\n"_s);
            ExitProcess(1);
        }
        
        void* memory = VirtualAlloc(0, (SIZE_T)file.size, MEM_COMMIT, PAGE_READWRITE);
        StreamingBuffer entire_file = read_entire_file(file, memory);
        PNG_FILE png = parse_png_file(entire_file, &allocator);
        // @CleanUp(husamd): for now
        // png.width*png.height pixel
        // each pixel is 4 bytes
        // + 1 for the png calculation
        U64 neededMemory = png.width * (png.height + 1) * 4; 

        
        // TODO:
        constexpr U32 image_size = 1920 * 1080 * 4 * 8;
        void* image_data = VirtualAlloc(0,
                                        image_size,// (SIZE_T)compute_requested_memory_size(&image_info),
                                        MEM_COMMIT,
                                        PAGE_READWRITE);
             
        {
            U64 pitch = 1920 * 4;
            U8* pointer = (U8*)image_data;
            
            for(U32 y = 0; y < 1080; y++)
            {
                U32* pixel = (U32*) pointer;
                for(U32 x = 0; x < 1920; x++)
                {
                    pixel[x] = (U32)(y << 8 | x);
                    //pixel[x] = 0xFFFFFF;
                    //0x00FFCCFF;//(U32)((y << 8) | x);
                }
                pointer += pitch;
            }
        }
        
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     1920,//image_info.width,
                     1080,//image_info.height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     image_data);
        
        glGenerateMipmap(GL_TEXTURE_2D);


        glUseProgram(obj.shaderProgram);
        
        uniformLocation = glGetUniformLocation(obj.shaderProgram, "texture1");
        glUniform1i(uniformLocation, 0);

        scaleFactorUniform = glGetUniformLocation(obj.shaderProgram, "scaleFactor");
        glUniform1f(scaleFactorUniform, 1.5);
    }
#endif
