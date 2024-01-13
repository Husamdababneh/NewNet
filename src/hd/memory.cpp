/* ========================================================================
   $File: memory.cpp
   $Date: 2023-09-29
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/


#pragma function(memset)
void* memset(void* dest, int c, size_t count)
{
    char* bytes = (char*)dest;
    while(count--)
    {
        *bytes++ = (char)c;
    }

    return dest;
}

#pragma function(memcpy)
void* memcpy(void* dest, const void* src, size_t count)
{
    char* dest8 = (char*)dest;
    const char* src8 = (const char*)src;
    while(count--)
    {
        *dest8++ = *src8++;
    }
    return dest;
}

#pragma function(memcmp)
S32 memcmp(const void *str1, const void *str2, size_t n)
{
    U8 u1, u2;
    U8* s1 = (U8*)str1;
    U8* s2 = (U8*)str2;
    for ( ; n-- ; s1++, s2++) {
        u1 = *(U8*)s1;
        u2 = *(U8*)s2;
        if (u1 != u2) {
            return (u1-u2);
        }
    }
    return 0;
}


void* AllocateSize(LinearAllocator* allocator, Size size)
{
    U64 availableSize = allocator->size - U64 ((U8*)allocator->current - (U8*)allocator->start);
    if (availableSize < size)
    {
        // Assert here 
        ExitProcess(1);
    }

    void* result = allocator->current;
    
    allocator->current = (U8*)allocator->current +  size;
    return result;
}


//- Linear Allocator
void Initalize_LinearAllocate(LinearAllocator* allocator, Size size)
{
    allocator->heap    = GetProcessHeap();
    allocator->start   = HeapAlloc(allocator->heap, HEAP_ZERO_MEMORY, size);
    allocator->current = allocator->start;
    allocator->size    = size;
    
}


void Free_LinearAllocate(LinearAllocator* allocator)
{
    HeapFree(allocator->heap,
             NULL,
             allocator->start);
}

StreamingBuffer* AllocateStreamingChunk(LinearAllocator *allocator)
{
    return AllocateType(allocator, StreamingBuffer);
}
