#include <precompiled.h>
#include <diagnostic\memoryTracker.h>

static class dumper
{
public:
    dumper()
    {
#if _DEBUG
        phi::setDumpMemory(true);
#endif
    }

    ~dumper()
    {
    }
} dump;