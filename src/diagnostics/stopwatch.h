#pragma once
#include <phi.h>
#include "diagnostics.h"

namespace phi
{
    class stopwatch
    {
    private:
        std::chrono::nanoseconds _initial;
        std::chrono::nanoseconds _stop;
        //std::chrono::nanoseconds _currentNanoSeconds = std::chrono::nanoseconds::zero();
        bool _isRunning = false;

    public:
        DIAGNOSTICS_API stopwatch();
        DIAGNOSTICS_API ~stopwatch();

        DIAGNOSTICS_API void start();
        DIAGNOSTICS_API void stop();
        DIAGNOSTICS_API void resume();
        DIAGNOSTICS_API const double getElapsedSeconds();
        DIAGNOSTICS_API const double getElapsedMilliseconds();
        DIAGNOSTICS_API static const double measure(const std::function<void(void)> &function);
        DIAGNOSTICS_API static const double measure(const std::function<void(void)> &function, const string &functionName);
        DIAGNOSTICS_API static const double measureAverage(const std::function<void(void)> &function, int numSamples);
        DIAGNOSTICS_API static const double measureAverage(const std::function<void(void)> &function, const string &functionName, int numSamples);
    };
}