#include <precompiled.h>
#include "memoryTracker.h"

#include "stackSymbol.h"
#include "stackTracer.h"
#include "windowsProcLoader.h"

#ifdef _DEBUG

namespace phi
{
    class linkedList
    {
    public:
        struct node
        {
            void* address;
            size_t size;
            stackSymbol allocationSight;
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
        bool shouldDump;
        size_t allocationCount;
    private:
        void initializeStackTracer()
        {
            char currentDirectoryPath[FILENAME_MAX];
            assert(GetModuleFileName(NULL, currentDirectoryPath, FILENAME_MAX));
            windowsProcLoader::load(currentDirectoryPath);
        }

        void dumpMemory()
        {
            char messageBuffer[1024];

            if (allocationCount == 0)
            {
                OutputDebugString("\nCongrats folk your software does not have any memory leaks!1 You rock =]\n\n");
            }
            else if (allocationCount < 0)
            {
                sprintf_s(
                    messageBuffer,
                    "\nHey bro, apparently you have deleted %zd pointers that had already been deleted.\nAre you high on crack?\n\n",
                    allocationCount);

                printf_s(messageBuffer);
            }
            else
            {
#ifdef DETAILED_MEMORY_TRACKING
                auto entry = _heapBuffer.getEntries();
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
                }
                OutputDebugString("\n\n");

                sprintf_s(messageBuffer,
                    "\n%s %zd %s",
                    "There are currently",
                    allocationCount,
                    "memory leaks.\nDid you smoke pot again last night?\nCheck the output window for more details\n\n");
#else
                sprintf_s(messageBuffer,
                    "\n%s %zd %s",
                    "There are currently",
                    allocationCount,
                    "memory leaks.\nDid you smoke pot again last night?\nRun in DebugWithMemoryTracking configuration for more information!!1\n\n");
#endif
                printf_s(messageBuffer);
                system("pause");
            }
        }
    public:
        HeapDumper() :
            allocationCount(0),
            shouldDump(false)
        {
#ifdef DETAILED_MEMORY_TRACKING
            initializeStackTracer();
#endif
        }

        ~HeapDumper()
        {
            if (shouldDump)
                dumpMemory();
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
        ++heap.allocationCount;
        auto address = malloc(size);

#ifdef DETAILED_MEMORY_TRACKING
        auto entry = static_cast<linkedList::node*>(malloc(sizeof(linkedList::node)));
        entry->address = address;
        entry->size = size;
        entry->allocationSight = stackTracer::optimizedCaptureAllocationSight();
        entry->next = nullptr;

        heap.addEntry(entry);
#endif
        return address;
    }

    void deallocate(void* address)
    {
#ifdef DETAILED_MEMORY_TRACKING
        heap.removeEntry(address);
#endif
        --heap.allocationCount;
        free(address);
    }

    void setDumpMemory(bool shouldDump)
    {
        heap.shouldDump = shouldDump;
    }
}
#endif