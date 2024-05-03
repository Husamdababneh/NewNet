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
    _HANDLE heap;
    U64    requested_address;
    void*  start;
    void*  cursor;
    Size   size;
};



struct LinearAllocator {
    _HANDLE heap;
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

// Taken from
// https://github.com/EpicGamesExt/raddebugger/blob/master/src/base/base_arena.h
////////////////////////////////
//~ rjf: Constants

#define ARENA_HEADER_SIZE 128

#ifndef ARENA_RESERVE_SIZE
# define ARENA_RESERVE_SIZE MB(64)
#endif
#ifndef ARENA_COMMIT_SIZE
# define ARENA_COMMIT_SIZE KB(64)
#endif

#ifndef ARENA_RESERVE_SIZE_LARGE_PAGES
# define ARENA_RESERVE_SIZE_LARGE_PAGES MB(8)
#endif
#ifndef ARENA_COMMIT_SIZE_LARGE_PAGES
# define ARENA_COMMIT_SIZE_LARGE_PAGES MB(2)
#endif

////////////////////////////////
//~ rjf: Arena Types

typedef struct Arena Arena;
struct Arena
{
  struct Arena *prev;
  struct Arena *current;
  U64 base_pos;
  U64 pos;
  U64 cmt;
  U64 res;
  U64 align;
  struct ArenaDev *dev;
  B8 grow;
  B8 large_pages;
};

typedef struct Temp Temp;
struct Temp
{
  Arena *arena;
  U64 pos;
};

////////////////////////////////
// Implementation

internal Arena* arena_alloc__sized(U64 init_res, U64 init_cmt);

internal Arena* arena_alloc(void);
internal void   arena_release(Arena *arena);

internal void*  arena_push__impl(Arena *arena, U64 size);
internal U64    arena_pos(Arena *arena);
internal void   arena_pop_to(Arena *arena, U64 pos);

internal void   arena_absorb(Arena *arena, Arena *sub);

////////////////////////////////
// Wrappers

internal void* arena_push(Arena *arena, U64 size);
internal void* arena_push_contiguous(Arena *arena, U64 size);
internal void  arena_clear(Arena *arena);
internal void  arena_push_align(Arena *arena, U64 align);
internal void  arena_put_back(Arena *arena, U64 amt);

internal Temp  temp_begin(Arena *arena);
internal void  temp_end(Temp temp);

////////////////////////////////
//~ NOTE(allen): "Mini-Arena" Helper

internal B32 ensure_commit(void **cmt, void *pos, U64 cmt_block_size);

////////////////////////////////
//~ NOTE(allen): Main API Macros

#if !ENABLE_DEV
# define push_array_no_zero(a,T,c) (T*)arena_push((a), sizeof(T)*(c))
#else
# define push_array_no_zero(a,T,c) (tctx_write_this_srcloc(), (T*)arena_push((a), sizeof(T)*(c)))
#endif
#define push_array_no_zero__no_annotation(a,T,c) (T*)arena_push__impl((a), sizeof(T)*(c))

#define push_array(a,T,c) (T*)MemoryZero(push_array_no_zero(a,T,c), sizeof(T)*(c))




// TODO(husamd): Read this https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning
// User code should use macros instead of functions.
#if 0
    #if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
    #define ASAN_POISON_MEMORY_REGION(addr, size)   \
        __asan_poison_memory_region((addr), (size))
    #define ASAN_UNPOISON_MEMORY_REGION(addr, size)     \
        __asan_unpoison_memory_region((addr), (size))
    #else
    #define ASAN_POISON_MEMORY_REGION(addr, size)   \
        ((void)(addr), (void)(size))
    #define ASAN_UNPOISON_MEMORY_REGION(addr, size) \
        ((void)(addr), (void)(size))
    #endif 
#else
    #define AsanPoisonMemoryRegion(addr, size)   ((void)(addr), (void)(size))
    #define AsanUnpoisonMemoryRegion(addr, size) ((void)(addr), (void)(size))
#endif 






// TODO(husamd): Move this

#define ClampTop(A,X) MIN(A,X)
#define ClampBot(X,B) MAX(X,B)
#define Clamp(A,X,B) ( ((X)<(A))?(A):((X)>(B))?(B):(X) )


//- rjf: Base Singly-Linked-List Stack Macros
#define SLLStackPush_N(f,n,next) ((n)->next=(f), (f)=(n))
#define SLLStackPop_N(f,next) ((f)=(f)->next)
//- rjf: Singly-Linked-List Stack Wrappers
#define SLLStackPush(f,n) SLLStackPush_N(f,n,next)
#define SLLStackPop(f) SLLStackPop_N(f,next)
