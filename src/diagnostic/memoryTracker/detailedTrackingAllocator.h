#pragma once
#include <phi.h>

#include <diagnostic\stackSymbol.h>

#include "detailedAllocation.h"
#include "detailedDeallocation.h"
#include "trackingAllocator.h"

namespace phi
{
    class detailedTrackingAllocator :
        public trackingAllocator
    {
    private:
        heapRecorder<detailedAllocation, detailedDeallocation> _recorder;

    private:
        void printOverview();
        void printAnalysisResult();

    public:
        virtual void* allocate(size_t size) override;
        virtual void deallocate(void* address) override;
        virtual void showResult() override;
    };
}