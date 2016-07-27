#include <precompiled.h>

#include "trackingAllocator.h"

namespace phi
{
    void* trackingAllocator::allocate(size_t size)
    {
        auto address = malloc(size);
        _recorder.registerAllocation(address);

        return address;
    }

    void trackingAllocator::deallocate(void* address)
    {
        _recorder.registerDeallocation(address);
        free(address);
    }

    void trackingAllocator::showResult()
    {
        printOverview();
    }

    void trackingAllocator::printOverview()
    {
        if (_recorder.hasMemoryIssues())
        {
            _recorder.printAnalysisOverview([](char* string)
            {
                debug(string);
            });

            debug("Run in DebugWithMemoryTracking configuration for more information!!1\n");

            system("pause");
        }
    }
}