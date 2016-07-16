#pragma once
#include <phi.h>
#include "diagnosticApi.h"

namespace phi
{
    class heapRecorder
    {
    private:

    public:
        DIAGNOSTIC_API heapRecorder();
        DIAGNOSTIC_API ~heapRecorder();

        DIAGNOSTIC_API void registerAllocation(void* address);
        DIAGNOSTIC_API void registerDeallocation(void* address);
    };
}