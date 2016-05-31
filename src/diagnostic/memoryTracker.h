#pragma once
#include <phi.h>
#include "diagnosticApi.h"

#ifdef _DEBUG

namespace phi
{
    DIAGNOSTIC_API void* allocate(size_t size);
    DIAGNOSTIC_API void deallocate(void* ptr);
    DIAGNOSTIC_API void setDumpMemory(bool shouldDump);
}

void* operator new(size_t mem)
{
    return phi::allocate(mem);
}

void* operator new[](size_t mem)
{
    return phi::allocate(mem);
}

void operator delete(void* ptr) noexcept
{
    phi::deallocate(ptr);
}

void operator delete[](void* ptr) noexcept
{
    phi::deallocate(ptr);
}

#endif