#pragma once
#include <phi.h>
#include <diagnostic\diagnosticApi.h>

#include "linkedList.h"
#include "allocation.h"
#include "deallocation.h"
#include "mallocAllocator.h"

namespace phi
{
    template<typename T>
    using mallocVector = vector<T, mallocAllocator<T>>;

    template<typename Allocation = phi::allocation, typename Deallocation = phi::deallocation>
    class heapRecorder
    {
    private:
        linkedList<void*, Allocation*> _allocations;
        mallocVector<Deallocation*> _redundantDeallocations;

    public:
        heapRecorder() {};
        ~heapRecorder() {};

        void registerAllocation(void* address)
        {
            _allocations.add(address, phi::make<Allocation>(address));
        }

        void registerDeallocation(void* address)
        {
            auto succeeded = _allocations.remove(address);
            if (!succeeded)
            {
                _redundantDeallocations.push_back(phi::make<Deallocation>(address));
            }
        }

        mallocVector<Allocation*> getMemoryLeaks()
        {
            auto memoryLeaks = mallocVector<Allocation*>();

            auto node = _allocations.getEntries();
            while (node)
            {
                memoryLeaks.push_back(node->value);
                node = node->next;
            }

            return memoryLeaks;
        }

        mallocVector<Deallocation*> getRedundantDeallocations()
        {
            return _redundantDeallocations;
        }

        bool hasMemoryIssues()
        {
            return _allocations.getEntriesCount() > 0 || _redundantDeallocations.size() > 0;
        }

        void printAnalysisOverview(action<char*> print)
        {
            char buffer[512];

            sprintf_s(buffer,
                "\nThere are %zd memory leaks and %zd redundant deallocations",
                _allocations.getEntriesCount(),
                _redundantDeallocations.size());

            print(buffer);
        }

        void printAnalysisResult(action<char*> print)
        {
            print((char*)"\nMemory Leaks:\n");

            for (auto& leak : getMemoryLeaks())
            {
                leak->print(print);
            }

            print((char*)"\nUnnecessary deallocations:\n");

            for (auto& deallocation : _redundantDeallocations)
            {
                deallocation->print(print);
            }

            print((char*)"\n");
        }
    };
}