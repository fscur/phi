#pragma once
#include <phi.h>
#include "trackingAllocator.h"

namespace phi
{
    class heap
    {
    private:
        trackingAllocator* _allocator;
        bool _shouldPrintMemoryAnalysisResult;

    private:
        void initializeStackTracer();

    public:
        heap(trackingAllocator* allocator);
        ~heap();

        void* allocate(size_t size);
        void deallocate(void* address);
        void setShouldPrintMemoryAnalysisResult(bool value);
    };
}