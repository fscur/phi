#pragma once
#include <phi.h>

#include "loggerDevice.h"

namespace phi
{
    class consoleLoggerDevice : public loggerDevice
    {
    private:
        void* _consoleHandle;
    public:
        consoleLoggerDevice();
        ~consoleLoggerDevice();

        virtual void logError(string message) override;

        virtual void logWarning(string message) override;

        virtual void logInfo(string message) override;
    };
}