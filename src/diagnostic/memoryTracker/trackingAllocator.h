#pragma once
#include <phi.h>

#include "allocation.h"
#include "deallocation.h"
#include "heapRecorder.h"

namespace phi
{
    class trackingAllocator
    {
    private:
        heapRecorder<allocation, deallocation> _recorder;

    private:
        void printOverview();

    public:
        virtual void* allocate(size_t size);
        virtual void deallocate(void* address);

        virtual void showResult();
    };
}