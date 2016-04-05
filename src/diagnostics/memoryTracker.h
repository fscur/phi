#pragma once
#include <phi.h>
#include "diagnosticsApi.h"

#ifdef _DEBUG
namespace phi
{
    DIAGNOSTICS_API void* allocate(size_t size);
    DIAGNOSTICS_API void deallocate(void* ptr);
}

void* operator new(size_t size)
{
    return phi::allocate(size);
}

void* operator new[](size_t size)
{
    return phi::allocate(size);
}

void operator delete(void* ptr)
{
    phi::deallocate(ptr);
}

void operator delete[](void* ptr)
{
    phi::deallocate(ptr);
}

#endif