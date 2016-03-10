#pragma once
#include <string>

namespace phi
{
    class logger
    {
    public:
        logger() { };
        ~logger() { };

        virtual void logError(std::string message) = 0;

        virtual void logWarning(std::string message) = 0;

        virtual void logInfo(std::string message) = 0;
    };
}