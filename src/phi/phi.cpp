#include "precompiled.h"
#include "phi.h"

#include "consoleLogger.h"

namespace phi
{
    logger* _logger = new consoleLogger();

    void logError(string value)
    {
        _logger->logError(value);
    }

    void logWarning(string value)
    {
        _logger->logWarning(value);
    }

    void logInfo(string value)
    {
        _logger->logInfo(value);
    }
}