#pragma once
#include <phi.h>
#include "diagnosticApi.h"

namespace phi
{
    class stopwatch
    {
    private:
        std::chrono::nanoseconds _initial;
        std::chrono::nanoseconds _stop;
        bool _isRunning = false;

    public:
        DIAGNOSTIC_API stopwatch();
        DIAGNOSTIC_API void start();
        DIAGNOSTIC_API void stop();
        DIAGNOSTIC_API void resume();
        DIAGNOSTIC_API double getElapsedSeconds();
        DIAGNOSTIC_API double getElapsedMilliseconds();
        DIAGNOSTIC_API static double measure(const std::function<void(void)> &function);
        DIAGNOSTIC_API static double measure(const std::function<void(void)> &function, const string &functionName);
        DIAGNOSTIC_API static double measureAverage(const std::function<void(void)> &function, int numSamples);
        DIAGNOSTIC_API static double measureAverage(const std::function<void(void)> &function, const string &functionName, int numSamples);
    };
}