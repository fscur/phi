#include <precompiled.h>
#include <diagnostics\memoryTracker.h>

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