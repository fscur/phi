#pragma once
#include <phi.h>
#include "diagnosticsApi.h"

#ifdef _DEBUG

namespace phi
{
    DIAGNOSTICS_API void* allocate(size_t size);
    DIAGNOSTICS_API void deallocate(void* ptr);
    DIAGNOSTICS_API void setDumpMemory(bool shouldDump);
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