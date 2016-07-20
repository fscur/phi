#include <precompiled.h>
#include "heapRecorder.h"

namespace phi
{
    template<typename T, typename ...Args>
    T* make(Args&&... args)
    {
        auto address = malloc(sizeof(T));
        return new(address)T(args...);
    };

    template<typename T>
    void destroy(T* obj)
    {
        obj->T::~T();
        free(obj);
    }

    heapRecorder::heapRecorder()
    {
    }

    heapRecorder::~heapRecorder()
    {
    }

    void heapRecorder::registerAllocation(void* address)
    {
        if (phi::contains(_initiatedAllocations, address))
            _incompleteOperations[address].push_back(_initiatedAllocations[address]);

        _initiatedAllocations[address] = make<memoryOperation>(make<allocation>());
    }

    void heapRecorder::registerDeallocation(void* address)
    {
        auto deallocation = make<phi::deallocation>();

        if (phi::contains(_initiatedAllocations, address))
        {
            auto operation = _initiatedAllocations[address];
            operation->add(deallocation);
            _completedOperations[address].push_back(operation);
            _initiatedAllocations.erase(address);
        }
        else if (phi::contains(_completedOperations, address))
        {
            auto addressOperations = _completedOperations[address];
            auto lastAddressOperation = phi::getLastElementOf(addressOperations);
            lastAddressOperation->add(deallocation);
        }
        else
        {
            auto operation = make<memoryOperation>();
            operation->add(deallocation);
            _unallocatedDeallocations[address].push_back(operation);
        }
    }

    vector<memoryOperation*> heapRecorder::getMemoryLeaks()
    {
        auto memoryLeaks = vector<memoryOperation*>();

        for (auto& pair : _initiatedAllocations)
        {
            memoryLeaks.push_back(pair.second);
        }

        for (auto& pair : _incompleteOperations)
        {
            auto address = pair.first;
            auto operations = pair.second;

            for (auto& operation : operations)
                memoryLeaks.push_back(operation);
        }

        return memoryLeaks;
    }

    vector<memoryOperation*> heapRecorder::getUnnecessaryDeallocations()
    {
        auto unnecessaryDeallocations = vector<memoryOperation*>();

        for (auto& pair : _completedOperations)
        {
            auto address = pair.first;
            auto operations = pair.second;

            for (auto& operation : operations)
            {
                if (operation->hasUnnecessaryDeallocations())
                    unnecessaryDeallocations.push_back(operation);
            }
        }

        for (auto& pair : _unallocatedDeallocations)
        {
            auto address = pair.first;
            auto operations = pair.second;

            for (auto& operation : operations)
                unnecessaryDeallocations.push_back(operation);
        }

        return unnecessaryDeallocations;
    }
}