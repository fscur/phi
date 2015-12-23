#ifndef _PHI_STOPWATCH_H_
#define _PHI_STOPWATCH_H_

#include "diagnostics.h"
#include "console.h"

#include <chrono>
#include <functional>

namespace phi
{
    class stopwatch
    {
    private:
        std::chrono::time_point<std::chrono::steady_clock> _initial;
        std::chrono::time_point<std::chrono::steady_clock> _final;
        std::chrono::nanoseconds _currentNanoSeconds = std::chrono::nanoseconds::zero();
        bool _isRunning = false;
    public:
        DIAGNOSTICS_API stopwatch() { };
        DIAGNOSTICS_API ~stopwatch() { };
        DIAGNOSTICS_API void start();
        DIAGNOSTICS_API void stop();
        DIAGNOSTICS_API void reset();
        DIAGNOSTICS_API void restart();
        DIAGNOSTICS_API const double getElapsedSeconds();
        DIAGNOSTICS_API static const double measure(const std::function<void(void)> &function);
        DIAGNOSTICS_API static const double measure(const std::function<void(void)> &function, const std::string &functionName);
        DIAGNOSTICS_API static const double measureAverage(const std::function<void(void)> &function, int numSamples);
        DIAGNOSTICS_API static const double measureAverage(const std::function<void(void)> &function, const std::string &functionName, int numSamples);
    };
}

#endif