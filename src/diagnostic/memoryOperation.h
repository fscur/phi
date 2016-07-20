#pragma once
#include <phi.h>

#include "mallocAllocator.h"

namespace phi
{
    struct allocation
    {
    };

    struct deallocation
    {
    };

    class memoryOperation
    {
    private:
        allocation* _allocation;
        vector<deallocation*, mallocAllocator<deallocation*>> _deallocations;

    public:
        memoryOperation()
        {
        }

        memoryOperation(allocation* allocation) :
            _allocation(allocation)
        {
        }

        ~memoryOperation() 
        {
            safeDelete(_allocation);

            for (auto& deallocation : _deallocations)
                safeDelete(deallocation);
        }

        void setAllocation(allocation* allocation)
        {
            _allocation = allocation;
        }

        void add(deallocation* deallocation)
        {
            _deallocations.push_back(deallocation);
        }

        bool hasUnnecessaryDeallocations()
        {
            return _deallocations.size() > 1;
        }

        bool isMemoryLeak()
        {
            return _deallocations.size() < 1;
        }
    };
}