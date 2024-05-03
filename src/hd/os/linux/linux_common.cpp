/* ========================================================================
   $File: linux_common.cpp
   $Date: 2024-05-03
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
   ========================================================================*/

#include <unistd.h>


internal B32
os_large_pages_enabled()
{
	// TODO: NOT implemented yet
	return false;
}

int os_large_page_size()
{
	return getpagesize();
}

internal
B32 os_commit_large(void* memory, Size size)
{
	// TODO: does mmap support reserve then commit ?
	return memory != NULL; // ?? 
}

internal
B32 os_commit(void* memory, Size size)
{
	// TODO: does mmap support reserve then commit ?
	return memory != NULL; // ?? 
}

internal
void* os_simple_largepage_alloc(Size size)
{
	constexpr int prot  = PROT_READ | PROT_WRITE;
    constexpr int flags = MAP_PRIVATE | MAP_ANONYMOUS;
	
	return mmap(NULL, size, prot, flags, -1, 0);
}

internal
void* os_simple_alloc(Size size)
{
	constexpr int prot  = PROT_READ | PROT_WRITE;
    constexpr int flags = MAP_PRIVATE | MAP_ANONYMOUS;
	
	return mmap(NULL, size, prot, flags, -1, 0);
}

internal
void os_release(Arena* arena, Size size)
{
	munmap((void*)arena, size);
}
