/* ========================================================================
   $File: common.h
   $Date: 2024-05-03
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: 
    ========================================================================*/
#pragma once

internal B32
os_large_pages_enabled()
{
	// TODO: NOT implemented yet
	return false;
}

internal
int os_large_page_size()
{
	return GetLargePageMinimum();
}

internal
void* os_commit(void* memory, Size size)
{
	(VirtualAlloc(current + current->cmt,
				  cmt_new_size,
				  MEM_COMMIT,
				  PAGE_READWRITE) != 0);
}

internal
void* os_commit_large(void* memory, Size size)
{
	(VirtualAlloc(current + current->cmt,
				  cmt_new_size,
				  MEM_COMMIT|MEM_LARGE_PAGES,
				  PAGE_READWRITE) != 0);
}

internal
void* os_simple_largepage_alloc(Size size)
{

	constexpr flags = MEM_RESERVE|MEM_COMMIT|MEM_LARGE_PAGES;
	
	return VirtualAlloc(0,
						size,
						flags,
						PAGE_READWRITE);
}

internal
void* os_simple_alloc(Size size)
{

	constexpr flags = MEM_RESERVE|MEM_COMMIT;
	
	return VirtualAlloc(0,
						size,
						flags,
						PAGE_READWRITE);
}

internal
void os_release(Arena arena, Size size)
{
	VirtualFree(node, size, MEM_RELEASE);
}
