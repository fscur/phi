#include <precompiled.h>
#include "memoryTracker.h"

#include "trackingAllocator.h"
#include "detailedTrackingAllocator.h"
#include "heap.h"

#ifdef _DEBUG

namespace phi
{

#ifdef DETAILED_MEMORY_TRACKING
    heap _heap = heap(phi::make<detailedTrackingAllocator>());
#else
    heap _heap = heap(phi::make<trackingAllocator>());
#endif

    void* allocate(size_t size)
    {
        return _heap.allocate(size);
    }

    void deallocate(void* address)
    {
        _heap.deallocate(address);
    }

    void setShouldPrintMemoryAnalysisResult(bool value)
    {
        _heap.setShouldPrintMemoryAnalysisResult(value);
    }
}

#endif