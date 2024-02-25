/* ========================================================================
   $File: memory.h
   $Date: 2024-02-03
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once


struct Buffer {
    U8* content;
    U64 size;    
};

struct StreamingBuffer {
    U8* content;
    U64 size;

    StreamingBuffer* next;
    
    U32 bitBuffer = 0;
    U32 bitCount  = 0;
};


struct StreamingBufferLL {
    StreamingBuffer* first;
    StreamingBuffer* last;
};


struct Allocator {
    HANDLE heap;
    U64    requested_address;
    void*  start;
    void*  cursor;
    Size   size;
};

struct LinearAllocator {
    HANDLE heap;
    void*  start;
    void*  current;
    Size   size;
};

struct ApplicationContext {
    Allocator allocator;
};

struct MemoryBlock {
    void* memory;
    Size size;
};

#define AllocateType(allocator, type) (type*)AllocateSize(allocator, sizeof(type));
void*   AllocateSize(LinearAllocator* allocator, Size size);
void    Initalize_LinearAllocate(LinearAllocator* allocator, Size size);
void    Free_LinearAllocate(LinearAllocator* allocator);

internal MemoryBlock RequestMemoryBlock(Size size);
internal B8          FreeMemoryBlock(MemoryBlock);
