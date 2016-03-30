#include <precompiled.h>
#include "memoryTracker.h"

#include "stackSymbol.h"
#include "stackTracer.h"

#ifdef MEMORY_DUMP

class linkedList
{
public:
    struct node
    {
        void* address;
        size_t size;
        phi::stackSymbol allocationSight;
        node* next;
    };
private:
    node* _root;
public:
    linkedList() :
        _root(nullptr)
    {
    }

    ~linkedList()
    {
        node* currentNode = _root;
        node* lastNode = nullptr;
        while (currentNode)
        {
            lastNode = currentNode;
            currentNode = currentNode->next;
            free(lastNode);
        }
    }

    void addEntry(node* entry)
    {
        if (!_root)
        {
            _root = entry;
            return;
        }

        auto currentNode = _root;
        while (currentNode->next != nullptr)
        {
            currentNode = currentNode->next;
        }

        currentNode->next = entry;
    }

    void removeEntry(void* address)
    {
        if (_root->address == address)
        {
            if (!_root->next)
            {
                free(_root);
                _root = nullptr;
            }
            else
            {
                auto entry = _root;
                _root = _root->next;
                free(entry);
            }
            return;
        }

        node* currentNode = _root;
        node* previousNode = nullptr;

        while (currentNode)
        {
            if (currentNode->address == address)
            {
                if (previousNode)
                    previousNode->next = currentNode->next;

                free(currentNode);
                break;
            }

            previousNode = currentNode;
            currentNode = currentNode->next;
        }
    }

    node* getEntries()
    {
        return _root;
    };
};

static class HeapDumper
{
private:
    linkedList _heapBuffer;
public:
    HeapDumper()
    {
    }

    ~HeapDumper()
    {
        auto entry = _heapBuffer.getEntries();

        if (!entry)
        {
            OutputDebugString("\nCongrats folk your software does not have any memory leaks =]\n\n");
        }
        else
        {
            char messageBuffer[1024];
            int count = 0;

            OutputDebugString("\n\n");
            while (entry)
            {
                sprintf_s(messageBuffer,
                    "%s(%d): memory leak error: allocated %zd bytes at %s\n",
                    entry->allocationSight.file.name,
                    entry->allocationSight.file.line,
                    entry->size,
                    entry->allocationSight.name);

                OutputDebugString(messageBuffer);
                entry = entry->next;
                ++count;
            }
            OutputDebugString("\n\n");

            sprintf_s(messageBuffer, "\n%s %d %s", "There are currently", count, "memory leaks.\nCheck the output window for more details\n\n");
            printf_s(messageBuffer);
            system("pause");
        }
    }

    void addEntry(linkedList::node* entry)
    {
        _heapBuffer.addEntry(entry);
    }

    void removeEntry(void* address)
    {
        _heapBuffer.removeEntry(address);
    }

} heap;

void* allocate(size_t size)
{
    auto allocationSight = phi::stackTracer::captureAllocationSight();
    auto address = malloc(size);

    auto entry = static_cast<linkedList::node*>(malloc(sizeof(linkedList::node)));
    entry->address = address;
    entry->size = size;
    entry->allocationSight = allocationSight;
    entry->next = nullptr;

    heap.addEntry(entry);
    return address;
}

void deallocate(void* address)
{
    heap.removeEntry(address);
    free(address);
}

#endif