#pragma once
#include <phi.h>

namespace phi
{
    class loggerDevice
    {
    public:
        loggerDevice() {};
        ~loggerDevice() {};

        virtual void logError(string message) = 0;
        virtual void logWarning(string message) = 0;
        virtual void logInfo(string message) = 0;
    };
}