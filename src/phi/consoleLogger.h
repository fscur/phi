#pragma once
#include "logger.h"
#include <string>

namespace phi
{
    class consoleLogger : public logger
    {
    private:
        void* _consoleHandle;
    public:
        consoleLogger();
        ~consoleLogger();

        virtual void logError(std::string message) override;

        virtual void logWarning(std::string message) override;

        virtual void logInfo(std::string message) override;
    };
}