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

    bool heapRecorder::isFirstDelete(void* address)
    {
        return phi::contains(_initiatedAllocations, address);
    }

    bool heapRecorder::hasAlreadyBeenDeleted(void* address)
    {
        return phi::contains(_completedOperations, address);
    }

    void heapRecorder::completeOperation(void* address)
    {
        auto operation = _initiatedAllocations[address];
        operation->add(make<phi::deallocation>());
        _completedOperations[address].push_back(operation);
        _initiatedAllocations.erase(address);
    }

    void heapRecorder::addUnnecessaryDeallocation(void* address)
    {
        auto addressOperations = _completedOperations[address];
        auto lastAddressOperation = phi::getLastElementOf(addressOperations);
        lastAddressOperation->add(make<phi::deallocation>());
    }

    void heapRecorder::addUnnalocatedDeallocation(void* address)
    {
        auto operation = make<memoryOperation>();
        operation->add(make<phi::deallocation>());
        _unallocatedDeallocations[address].push_back(operation);
    }

    void heapRecorder::initiateOperation(void* address)
    {
        _initiatedAllocations[address] = make<memoryOperation>(make<allocation>());
    }

    void heapRecorder::registerAllocation(void* address)
    {
        initiateOperation(address);
    }

    void heapRecorder::registerDeallocation(void* address)
    {
        if (isFirstDelete(address))
        {
            completeOperation(address);
        }
        else if (hasAlreadyBeenDeleted(address))
        {
            addUnnecessaryDeallocation(address);
        }
        else
        {
            addUnnalocatedDeallocation(address);
        }
    }

    vector<memoryOperation*> heapRecorder::getMemoryLeaks()
    {
        auto memoryLeaks = vector<memoryOperation*>();

        for (auto& pair : _initiatedAllocations)
        {
            memoryLeaks.push_back(pair.second);
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