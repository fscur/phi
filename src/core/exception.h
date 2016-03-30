#pragma once
#include "core.h"
#include <phi.h>

#include <diagnostics\stackSymbol.h>

namespace phi
{
    class exception :
        public std::runtime_error
    {
    private:
        vector<stackSymbol> _stackTrace;
    public:
        CORE_API exception(string message);
        vector<stackSymbol> getStackTrace() { return _stackTrace; }
    };
}