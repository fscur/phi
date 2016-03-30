#pragma once
#include <phi.h>
#include "diagnostics.h"

//#define MEMORY_DUMP
#ifdef MEMORY_DUMP

DIAGNOSTICS_API void* allocate(size_t size);
DIAGNOSTICS_API void deallocate(void* ptr);

void* operator new(size_t size)
{
    return allocate(size);
}

void operator delete(void* ptr)
{
    deallocate(ptr);
}

#endif