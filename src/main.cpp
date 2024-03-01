/* ========================================================================
   $File: main.cpp
   $Date: 2023-02-27
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Entry point
   ========================================================================*/



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

global_variable Win32_OffscreenBuffer offscreenBuffer;

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
    GetWindowRect(window,&windowRect);
    
    result.width  = (U64)windowRect.right  - (U64)windowRect.left;
    result.height = (U64)windowRect.bottom - (U64)windowRect.top;

    return result;
}

inline internal
void update_offscreen_buffer(HWND window, Win32_OffscreenBuffer* buffer)
{
    WindowDimentions dimentions = win32_get_window_dimentions(window);
    
    if (buffer->memory)
    {
        // TODO(Husam): Check the return of this;
        BOOL result = VirtualFree(buffer->memory, 0, MEM_RELEASE);
        (result);
    }
    
    buffer->width        = dimentions.width;
    buffer->height       = dimentions.height;
    buffer->bytePerPixel = 4;

    buffer->info.bmiHeader = {};
    buffer->info.bmiHeader.biSize        =  sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth       =  (LONG)buffer->width;
    buffer->info.bmiHeader.biHeight      = -(LONG)buffer->height;
    buffer->info.bmiHeader.biPlanes      =  1;
    buffer->info.bmiHeader.biBitCount    =  32;
    buffer->info.bmiHeader.biCompression =  BI_RGB;


    U64 bitMapMemorySize = buffer->width * buffer->height * buffer->bytePerPixel;

    
    buffer->memory = VirtualAlloc(0, bitMapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    if(!buffer->memory)
    {
        print_string("Could not allocate memory for some reason\n"_s);
        ExitProcess(0);
    }

    U64 pitch = buffer->width * buffer->bytePerPixel;
    U8* pointer = (U8*)buffer->memory;
    
    for(U32 y = 0; y < buffer->height; y++)
    {
        U32* pixel = (U32*) pointer;
        for(U32 x = 0; x < buffer->width; x++)
        {
            pixel[x] = (U32)(y << 8 | x);
                //0x00FFCCFF;//(U32)((y << 8) | x);
        }
        pointer += pitch;
    }
}



inline 
internal
void render(HWND window, Win32_OffscreenBuffer* buffer)
{
    HDC windowContext = GetDC(window);
    
    int result =  StretchDIBits(windowContext,
                                0,                   // x dest
                                0,                   // y dest
                                (int)buffer->width,  // width dest
                                (int)buffer->height, // height dest
                                0,                   // x src     
                                0,                   // y src     
                                (int)buffer->width,  // width src 
                                (int)buffer->height, // height src
                                buffer->memory, 
                                &buffer->info,  
                                DIB_RGB_COLORS,
                                SRCCOPY);

    
    
    (result);
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
          //update_offscreen_buffer(hWindow, &offscreenBuffer);         
          //render(hWindow, &offscreenBuffer);
          result = DefWindowProc(hWindow, message, wParam, lParam);
          //(handled);
          //handled = true;
      } break;
      case WM_SIZE:
      {
          if (backEnd.context) glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
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
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 uv;
    void main()
    {
       gl_Position = vec4(aPos.xyz, 1.0);
    }
)str";

const char *fragmentShaderSource = R"str(
    #version 330 core
    out vec4 FragColor;
    void main()
    {
       FragColor = vec4(1.0f, 0.0f, 0.94f, 1.0f);
    }
)str";


struct Item
{
    U32 vbo, ebo, vao;
    U32 shaderProgram;
};

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
          0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // top right
          0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
         -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom left
         -0.5f,  0.5f, 0.0f, 1.0f, 0.0f   // top left 
    };


    
    U32 indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };


    // Generate Vertex Arrays 
    glGenVertexArrays(1, &obj.vao);
    // Generate Normal(memory space) Buffers
    glGenBuffers(1, &obj.vbo);
    glGenBuffers(1, &obj.ebo);

    // Populate Generated buffers 
    glBindVertexArray(obj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glNamedBufferData(obj.vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo);
    glNamedBufferData(obj.ebo, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // We don't have glNamedVertexAttribPointer so we need to bind it before using it 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)0);
    glEnableVertexAttribArray(0);
    //glEnableVertexArrayAttrib(obj.vao, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)(3 * sizeof(F32)));
    glEnableVertexAttribArray(1);
    //glEnableVertexArrayAttrib(obj.vao, 1);


    // Shaders

    // vertex shader
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const S8**)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);


    // fragment shader
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const S8**)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    obj.shaderProgram  = glCreateProgram();
    glAttachShader(obj.shaderProgram, vertexShader);
    glAttachShader(obj.shaderProgram, fragmentShader);
    glLinkProgram(obj.shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return obj;
}
#pragma auto_inline(on)

void free_buffers(Item obj)
{
    /*
      glDeleteVertexArrays(obj.vao);
      glDeleteBuffers(obj.vbo);
      glDeleteBuffers(obj.ebo);
      glDeleteProgram(obj.shaderProgram);
    */
}


U32 compute_requested_memory_size(PNG_FILE* image)
{
    // TODO: 
    constexpr auto channels = 4;
    return (image->width * image->height * (image->bit_depth) * channels);
}


int  main()
{
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
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
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
    LinearAllocator allocator;
    Initalize_LinearAllocate(&allocator, MB(1));

    
    LARGE_INTEGER start_counter, frequency; //, end_counter, counts, fps, ms;
    
    QueryPerformanceCounter(&start_counter);
    QueryPerformanceFrequency(&frequency);
    HDC windowContext = GetDC(window);

    Item obj = init_object();
    Vec4F32 vec = {1.0f, 0.f, 0.f, 1.f};
    Mat4F32 transformationMat = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f,
    };


    
    U32 texture1;
    S32 uniformLocation;
    S32 scaleFactorUniform;
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

    //scaleFactorUniform = glGetUniformLocation(obj.shaderProgram, "scaleFactor");

    //glFrontFace(GL_CW);
    
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
        glClearColor(0.8f, 0.8f, 0.5f, 1.0f);
        //glClearColor(1.f, 0.0f, 0.94f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, texture1);

        // seeing as we only have a single VAO there's no need to bind it every time,
        // but we'll do so to keep things a bit more organized
        glBindVertexArray(obj.vao);
        glUseProgram(obj.shaderProgram);
        //glUniform1f(scaleFactorUniform, 0.5);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        
        
        SwapBuffers(windowContext );
    }
    
    // delete our buffers
    free_buffers(obj);

    
    // NOTE(husamd): I wish defer is a thing in c/c++
    {
        
        // Unnecessary; wglDeleteContext will make the context not current
        //wglMakeCurrent(backEnd.context, NULL);

        wglDeleteContext(backEnd.context);
        PostQuitMessage(0);
    }
    Free_LinearAllocate(&allocator);           
    DestroyWindow(window);
    FreeConsole();
    ExitProcess(0);
}


