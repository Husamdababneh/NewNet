/* ========================================================================
   $File: io.h
   $Date: 2024-02-03
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once

#if OS_WINDOWS == 1
typedef HANDLE FileHandle;
#endif



struct File {
    _HANDLE h;
    U64 size;
    U8* content; 
};

/*
 * I want this API to be able to
 * 1 - Open a file Without Reading it, just get a handle to the file 
 * 2 - Read entire file to memory, either by giving it a memory block, or giving it a memory allocator 
 * 3 - Write to file, i would prefer to write a the whole buffer at once
 *     * Maybe i want to have the ability to write at a specific location (insert)
 * 4 - Close the file
 * 5 - Get file size without getting handle? 
 */


File open_file(const S8* path);
File open_file(String path);
void close_file(File file);

Size get_file_size(File file); // i dont think we need this 
StreamingBuffer read_entire_file(File file);
StreamingBuffer read_entire_file(File file, void* block);

B8 write_file(File file, StreamingBuffer content);


internal inline void bswap_ip(U16* value);
internal inline void bswap_ip(U32* value);
internal inline void bswap_ip(U64* value);

internal inline U16 bswap_ip(U16 value);
internal inline U32 bswap_ip(U32 value);
internal inline U64 bswap_ip(U64 value);

#define ConsumeType(buffer, type) (type*)ConsumeSize(buffer, sizeof(type))
void*   ConsumeSize(StreamingBuffer* buffer, U64 size);
void    FlushStreamingBufferBitBuffer(StreamingBuffer* buffer);
U32     ConsumeBits(StreamingBuffer* buffer, U32 bitCount);

String  ConsumeCString(StreamingBuffer* buffer);
String  ConsumeString(StreamingBuffer* buffer, U64 size);

// @CleanUp(husamd):
inline internal void print_string(const String str);
inline internal void print_verbos(const String str);
