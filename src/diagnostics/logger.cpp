#include <precompiled.h>
#include "logger.h"

#include "consoleLoggerDevice.h"

namespace phi
{
    logger::logger() :
        _device(new consoleLoggerDevice())
        //logger::logger(new consoleLoggerDevice())
    {
    }

    //logger::logger(loggerDevice * device) :
    //    _device(device)
    //{
    //}

    logger::~logger()
    {
        delete _device;
    }

    void logger::logError(string message) const
    {
        _device->logError(message);
    }

    void logger::logWarning(string message) const
    {
        _device->logWarning(message);
    }

    void logger::logInfo(string message) const
    {
        _device->logInfo(message);
    }
}