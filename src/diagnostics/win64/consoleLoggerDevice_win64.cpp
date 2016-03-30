#include <precompiled.h>
#include "..\consoleLoggerDevice.h"

namespace phi
{
    consoleLoggerDevice::consoleLoggerDevice() :
        _consoleHandle(GetStdHandle(STD_OUTPUT_HANDLE))
    {
    }

    consoleLoggerDevice::~consoleLoggerDevice()
    {
    }

    void consoleLoggerDevice::logError(string message)
    {
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cout << message << std::endl;
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    void consoleLoggerDevice::logWarning(string message)
    {
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << message << std::endl;
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    void consoleLoggerDevice::logInfo(string message)
    {
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << message << std::endl;
    }
}