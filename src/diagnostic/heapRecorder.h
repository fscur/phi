#pragma once
#include <phi.h>
#include "diagnosticApi.h"

#include "memoryOperation.h"
#include "mallocAllocator.h"

namespace phi
{
    template<typename T>
    using mallocVector = vector<T, mallocAllocator<T>>;

    template<typename Key, typename Value>
    using mallocMap = unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>, mallocAllocator<std::pair<const Key, Value>>>;

    class heapRecorder
    {
    private:
        mallocMap<void*, memoryOperation*> _initiatedAllocations;
        mallocMap<void*, mallocVector<memoryOperation*>> _completedOperations;
        mallocMap<void*, mallocVector<memoryOperation*>> _incompleteOperations;
        mallocMap<void*, mallocVector<memoryOperation*>> _unallocatedDeallocations;

    public:
        DIAGNOSTIC_API heapRecorder();
        DIAGNOSTIC_API ~heapRecorder();

        DIAGNOSTIC_API void registerAllocation(void* address);
        DIAGNOSTIC_API void registerDeallocation(void* address);
        DIAGNOSTIC_API vector<memoryOperation*> getMemoryLeaks();
        DIAGNOSTIC_API vector<memoryOperation*> getUnnecessaryDeallocations();
    };
}