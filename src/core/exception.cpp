#include <precompiled.h>
#include "exception.h"

#include <diagnostics\stackTracer.h>
#include <diagnostics\stopwatch.h>

namespace phi
{
    exception::exception(string message) :
        std::runtime_error(message)
    {
#if _DEBUG
        _stackTrace = stackTracer::stackWalk64();
        stackTracer::printStackTrace(_stackTrace);
#else
        _stackTrace = stackTracer::captureStackBackTrace();
#endif
    }
}
