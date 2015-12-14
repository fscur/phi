#ifndef _PHI_STOPWATCH_H_
#define _PHI_STOPWATCH_H_

#include <phi/diagnostics/diagnostics.h>
#include <phi/diagnostics/console.h>

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
        DIAGNOSTICS_API void Start();
        DIAGNOSTICS_API void Stop();
        DIAGNOSTICS_API void Reset();
        DIAGNOSTICS_API void Restart();
        DIAGNOSTICS_API const double GetElapsedSeconds();

        DIAGNOSTICS_API static const double Measure(const std::function<void(void)> &function)
        {
            auto watch = stopwatch();
            watch.Start();
            function();
            watch.Stop();

            return watch.GetElapsedSeconds();
        }

        DIAGNOSTICS_API static const double Measure(const std::function<void(void)> &function, const std::string &functionName)
        {
            auto watch = stopwatch();
            watch.Start();
            function();
            watch.Stop();

            auto msg = functionName + " took: ";
            auto elapsedSeconds = watch.GetElapsedSeconds();
            console::WriteLine(msg + std::to_string(elapsedSeconds));

            return elapsedSeconds;
        }
    };
}

#endif