#pragma once
#include <phi.h>
#include "diagnosticApi.h"

#include "loggerDevice.h"

namespace phi
{
    //TODO: analize the need to log the stack trace
    //Windows: https://msdn.microsoft.com/en-us/library/windows/desktop/bb204633(v=vs.85).aspx
    //Unix: http://man7.org/linux/man-pages/man3/backtrace.3.html
    //Mac: https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/backtrace.3.html

    class logger
    {
    private:
        loggerDevice* _device;
    public:
        DIAGNOSTIC_API logger();
        DIAGNOSTIC_API logger(loggerDevice* device);
        DIAGNOSTIC_API ~logger();

        DIAGNOSTIC_API void logError(string message) const;
        DIAGNOSTIC_API void logWarning(string message) const;
        DIAGNOSTIC_API void logInfo(string message) const;

        void setDevice(loggerDevice* device) { _device = device; }
    };
}