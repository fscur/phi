#include <precompiled.h>
#include <diagnostic\memoryTracker\memoryTracker.h>

#if _DEBUG
static class dumper
{
public:
    dumper()
    {
        phi::setShouldPrintMemoryAnalysisResult(true);
    }

    ~dumper()
    {
    }
} dump;
#endif