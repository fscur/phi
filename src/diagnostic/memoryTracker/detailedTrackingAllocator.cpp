#include <precompiled.h>
#include "detailedTrackingAllocator.h"

#include <diagnostic\stackTracer.h>

namespace phi
{
    void* detailedTrackingAllocator::allocate(size_t size)
    {
        auto address = malloc(size);
        _recorder.registerAllocation(address);

        return address;
    }

    void detailedTrackingAllocator::deallocate(void* address)
    {
        _recorder.registerDeallocation(address);
        free(address);
    }

    void detailedTrackingAllocator::showResult()
    {
        if (_recorder.hasMemoryIssues())
        {
            printOverview();
            printAnalysisResult();
        }
    }

    void detailedTrackingAllocator::printOverview()
    {
        if (_recorder.hasMemoryIssues())
        {
            _recorder.printAnalysisOverview([](char* string)
            {
                debug(string);
            });

            debug("Check the output window for more details.");

            system("pause");
        }
    }

    void detailedTrackingAllocator::printAnalysisResult()
    {
        _recorder.printAnalysisResult([](char* string)
        {
            OutputDebugString(string);
        });
    }
}