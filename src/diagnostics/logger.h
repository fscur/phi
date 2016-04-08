#pragma once
#include <phi.h>
#include "diagnosticsApi.h"

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
        DIAGNOSTICS_API logger();
        DIAGNOSTICS_API logger(loggerDevice* device);
        DIAGNOSTICS_API ~logger();

        DIAGNOSTICS_API void logError(string message) const;
        DIAGNOSTICS_API void logWarning(string message) const;
        DIAGNOSTICS_API void logInfo(string message) const;

        void setDevice(loggerDevice* device) { _device = device; }
    };
}