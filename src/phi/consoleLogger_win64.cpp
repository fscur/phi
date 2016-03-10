#include "precompiled.h"
#include "consoleLogger.h"

namespace phi
{
    consoleLogger::consoleLogger() :
        _consoleHandle(GetStdHandle(STD_OUTPUT_HANDLE))
    {
    }

    consoleLogger::~consoleLogger()
    {
    }

    void consoleLogger::logError(std::string message)
    {
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cout << message << std::endl;
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    void consoleLogger::logWarning(std::string message)
    {
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << message << std::endl;
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    void consoleLogger::logInfo(std::string message)
    {
        SetConsoleTextAttribute(_consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << message << std::endl;
    }
}