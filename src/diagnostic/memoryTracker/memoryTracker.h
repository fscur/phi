#pragma once
#include <phi.h>
#include <diagnostic\diagnosticApi.h>

#ifdef _DEBUG

namespace phi
{
    DIAGNOSTIC_API void* allocate(size_t);
    DIAGNOSTIC_API void deallocate(void*);
    DIAGNOSTIC_API void setShouldPrintMemoryAnalysisResult(bool);
}

//void* operator new(size_t mem)
//{
//    return phi::allocate(mem);
//}
//
//void* operator new[](size_t mem)
//{
//    return phi::allocate(mem);
//}
//
//void operator delete(void* ptr) noexcept
//{
//    phi::deallocate(ptr);
//}
//
//void operator delete[](void* ptr) noexcept
//{
//    phi::deallocate(ptr);
//}

#endif