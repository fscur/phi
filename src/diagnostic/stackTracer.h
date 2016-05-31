#pragma once
#include <phi.h>
#include "diagnosticsApi.h"
#include "stackSymbol.h"
#include "win64DataDefinitions.h"

namespace phi
{
    class stackTracer
    {
    private:
        static const uint16_t MAX_SYMBOL_NAME_LENGTH = 1024;
    private:
        stackTracer();
        ~stackTracer();
        static symbolFile symbolFileFromAddress(HANDLE process, uintptr_t symbolAddress);
        static symbolModule symbolModuleFromAddress(HANDLE process, uintptr_t symbolAddress);
        static void takeSnapshot(HANDLE process);
        static DWORD getPcImageType();
        static win64DataDefinitions::stackFrame buildStackFrame(CONTEXT context);
        static win64DataDefinitions::symbol64* buildSymbolPointer();
        static stackSymbol buildStackSymbolFromAddress(HANDLE process, uintptr_t callAddress);
    public:
        DIAGNOSTICS_API static vector<stackSymbol> captureStackBackTrace();
        DIAGNOSTICS_API static vector<stackSymbol> stackWalk64();
        DIAGNOSTICS_API static stackSymbol captureAllocationSight();
        DIAGNOSTICS_API static stackSymbol optimizedCaptureAllocationSight();
        DIAGNOSTICS_API static void printStackTrace(vector<stackSymbol> stack);
    };
}
