#pragma once
#include <phi.h>
#include "diagnostics.h"
#include "stackSymbol.h"

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
        static STACKFRAME64 buildStackFrame(CONTEXT context);
        static IMAGEHLP_SYMBOL64* buildSymbol64Pointer();
        static SYMBOL_INFO* buildSymbolPointer();
    public:
        DIAGNOSTICS_API static vector<stackSymbol> captureStackBackTrace();
        DIAGNOSTICS_API static vector<stackSymbol> stackWalk64();
        DIAGNOSTICS_API static stackSymbol captureAllocationSight();
        DIAGNOSTICS_API static void printStackTrace(vector<stackSymbol> stack);
    };
}
