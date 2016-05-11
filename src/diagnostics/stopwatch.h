#pragma once
#include <phi.h>
#include "diagnosticsApi.h"

namespace phi
{
    class stopwatch
    {
    private:
        std::chrono::nanoseconds _initial;
        std::chrono::nanoseconds _stop;
        bool _isRunning = false;

    public:
        DIAGNOSTICS_API stopwatch();
        DIAGNOSTICS_API void start();
        DIAGNOSTICS_API void stop();
        DIAGNOSTICS_API void resume();
        DIAGNOSTICS_API double getElapsedSeconds();
        DIAGNOSTICS_API double getElapsedMilliseconds();
        DIAGNOSTICS_API static double measure(const std::function<void(void)> &function);
        DIAGNOSTICS_API static double measure(const std::function<void(void)> &function, const string &functionName);
        DIAGNOSTICS_API static double measureAverage(const std::function<void(void)> &function, int numSamples);
        DIAGNOSTICS_API static double measureAverage(const std::function<void(void)> &function, const string &functionName, int numSamples);
    };
}