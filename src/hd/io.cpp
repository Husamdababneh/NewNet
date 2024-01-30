/* ========================================================================
   $File: hdio.cpp
   $Date: 2023-09-27
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/


#ifdef HD_IO

#define _HD_VERBOS

inline internal
void print_string(const String str)
{
    //CONSOLE_SCREEN_BUFFER_INFO old;
    //GetConsoleScreenBufferInfo(console, &old);
    //SetConsoleTextAttribute(console, FOREGROUND_RED);


    if(str.size == 0)
    {
        return;
    }

    // Do i need to call this every time?? or what i dont know how expensive this is
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    int output_size;
    WriteConsole(console, str.str_char, (DWORD)str.length, (LPDWORD)&output_size, NULL);
    // GetLastError if something went wrong
    //SetConsoleTextAttribute(console, old.wAttributes);
}


inline internal
void print_verbos(const String str)
{
#ifdef _HD_VERBOS
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    char buffer[1024] = {};
    DWORD count = (DWORD) (is_null_terminated(str) ? str.length - 1 : str.length);
    memcpy(buffer, str.str_char, count);
    int output_size;
    WriteConsole(console, buffer, count, (LPDWORD)&output_size, NULL);
#else
    (str);
#endif // _HD_VERBOS
}


void* ConsumeSize(StreamingBuffer* buffer, U64 size)
{
    void* result = buffer->content;
    buffer->content += size;
    buffer->size    -= size;
    return result;
}


void FlushStreamingBufferBitBuffer(StreamingBuffer* buffer)
{
    buffer->bitBuffer = 0;
    buffer->bitCount = 0;
}

internal 
U32 PeekBits(StreamingBuffer* buffer, U32 bitCount)
{
    Assert(bitCount <= 32);
    // Fill our bitBuffer 
    while(buffer->bitCount < bitCount &&
          (buffer->size))
    {
        U8 byte = *ConsumeType(buffer, U8);
        buffer->bitBuffer |= (byte << buffer->bitCount);
        buffer->bitCount += 8;
    }
    
    U32 result = 0;
    if (buffer->bitCount >= bitCount)
    {
        U32 bitCountOnesCompliment = (U32)(( 1 << bitCount) -1);
        result = buffer->bitBuffer & bitCountOnesCompliment;
    }
   
    return result;
}
internal
void DiscardBits(StreamingBuffer* buffer, U32 bitCount)
{
    
    if (buffer->bitCount >= bitCount)
    {
        buffer->bitCount -= bitCount;
        buffer->bitBuffer >>= bitCount;
    }
}

U32 ConsumeBits(StreamingBuffer* buffer, U32 bitCount)
{
    U32 result = PeekBits(buffer, bitCount);
    DiscardBits(buffer, bitCount); 
    return result;
}

String ConsumeCString(StreamingBuffer* buffer)
{
    U8* result = buffer->content;
    while (*buffer->content != 0){
        buffer->content += 1;
        buffer->size    -= 1;
    }

    // The +1 here is to account for the null terminator
    String str = {(U64)(buffer->content - result)+1, (char*)result};

    // The +1&-1 here are to account for the null terminator
    buffer->content += 1;
    buffer->size    -= 1;
    
    return str;
}

String ConsumeString(StreamingBuffer* buffer, U64 size)
{
    char* text = (char*)ConsumeSize(buffer, size);
    String str = {size, (char*)text};
    return str;
}





File open_file(const S8* path)
{
    File result = {};
    OVERLAPPED overLapped = {0};
    
    // WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    HANDLE file = CreateFileA((LPSTR)path,
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
    
    if (INVALID_HANDLE_VALUE == file)
    {
        return result;
    }

    // Maybe in the furtue we want to get the user more information about the file
    // But for now I want it to be as small as possible since sometimes you need to make a lot of operations on
    // many files - husamd 29Sep2023
     
    LARGE_INTEGER fileSize;
    GetFileSizeEx(file, &fileSize);
        
    result.windows_file_handle = file;
    result.size = (U64)fileSize.QuadPart;
    return result;
}

File open_file(String path)
{
    if (is_null_terminated(path))
    {
        return open_file(path.str);
    }
    
    const char string[256] = {};
    memcpy((void*)string, (void*)path.str, (size_t)path.length);
                             
    return {};
}


StreamingBuffer read_entire_file(File file, void* buffer)
{
    StreamingBuffer result = {};
#ifdef OS_WINDOWS

    //U8* bufferCursor = buffer;
    BOOL readSuccess = false;
    //U64 totalByteRead;
    U32 byteRead;

    if (file.size <= 0xFFFFFFFF)
    {
        readSuccess = ReadFile(file.handle, buffer, (U32)file.size, (LPDWORD)&byteRead, NULL);
        result.content = (U8*)buffer;
        result.size = byteRead;
    }
    else {
        ExitProcess(1);
    }
    // TODO(husamd): test this
    // else
    //     while(file.size - totalByteRead > 0)
    //      {
    //          bytesToRead = file.size - 0xFFFFFFFF;
    //          readSuccess =  ReadFile(file.handle,
    //                                  bufferCursor,
    //                                  bytesToRead,
    //                                  (LPDWORD)&byteRead,
    //                                  NULL);
    //          totalByteRead += byteRead;
    //          bufferCursor += byteRead;
    //      }
    
#endif // OS_WINDOWS
    return result;
}



internal inline
void bswap_ip(U16* value)
{
    U16 v = *value;
    v =  U16((v << 8) | ((v >> 8)));
    *value = v;
};

internal inline
void bswap_ip(U32* value)
{
    constexpr auto MASK = 0xFF00;
    U32 v = *value;
    v = (
        ((v & 0x0000'00FF) << 24) |
        ((v & 0x0000'FF00) << 8)  |
        ((v & 0x00FF'0000) >> 8)  |
        ((v & 0xFF00'0000) >> 24)
        );
    *value = v;
}

void bswap_ip(U64* value)
{
    constexpr auto MASK = 0xFF00;
    U64 v = *value;
    v = (
        ((v & 0x0000'0000'0000'00FF) << 56) | 
        ((v & 0x0000'0000'0000'FF00) << 40) |
        ((v & 0x0000'0000'00FF'0000) << 24) |
        ((v & 0x0000'0000'FF00'0000) << 8)  |
        ((v & 0x0000'00FF'0000'0000) >> 8)  |
        ((v & 0x0000'FF00'0000'0000) >> 24) |
        ((v & 0x00FF'0000'0000'0000) >> 40) |
        ((v & 0xFF00'0000'0000'0000) >> 56) 
        );
    *value = v;
}

internal inline
U16 bswap(U16 value)
{
    U16 v = value;
    bswap_ip(&v);
    return v;
};

internal inline
U32 bswap(U32 value)
{
    U32 v = value;
    bswap_ip(&v);
    return v;
}

U64 bswap(U64 value)
{
    U64 v = value;
    bswap_ip(&v);
    return v;
}





#endif // HD_IO
