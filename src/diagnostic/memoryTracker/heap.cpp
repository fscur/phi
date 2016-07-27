#include <precompiled.h>
#include "heap.h"

#include <diagnostic\platformProcLoader.h>

#include "detailedTrackingAllocator.h"

namespace phi
{
    heap::heap(trackingAllocator* allocator) :
        _allocator(allocator),
        _shouldPrintMemoryAnalysisResult(false)
    {
#ifdef DETAILED_MEMORY_TRACKING
        initializeStackTracer();
#endif
    }

    heap::~heap()
    {
        if (_shouldPrintMemoryAnalysisResult)
            _allocator->showResult();

        phi::destroy(_allocator);
    }

    void heap::initializeStackTracer()
    {
        char currentDirectoryPath[FILENAME_MAX];
        auto result = GetModuleFileName(NULL, currentDirectoryPath, FILENAME_MAX);
        assert(result);

        platformProcLoader::load(currentDirectoryPath);
    }

    void * heap::allocate(size_t size)
    {
        return _allocator->allocate(size);
    }

    void heap::deallocate(void * address)
    {
        _allocator->deallocate(address);
    }

    void heap::setShouldPrintMemoryAnalysisResult(bool value)
    {
        _shouldPrintMemoryAnalysisResult = value;
    }
}