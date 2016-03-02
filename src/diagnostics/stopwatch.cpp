#include <precompiled.h>
#include "diagnostics.h"
#include "stopwatch.h"
#include "console.h"

namespace phi
{
    void stopwatch::start()
    {
        if(!_isRunning)
        {
            _isRunning = true;
            _initial = std::chrono::high_resolution_clock::now();
        }
    }

    void stopwatch::stop()
    {
        if(_isRunning)
        {
            _final = std::chrono::high_resolution_clock::now();
            _currentNanoSeconds += _final - _initial;
            _isRunning = false;
        }
    }

    void stopwatch::reset()
    {
        _currentNanoSeconds = std::chrono::nanoseconds::zero();
    }

    void stopwatch::restart()
    {
        reset();
        start();
    }

    const double stopwatch::getElapsedSeconds()
    {
        return std::chrono::duration_cast<std::chrono::duration<double>>(_currentNanoSeconds).count();
    }

    const double stopwatch::measure(const std::function<void(void)> &function)
    {
        auto watch = stopwatch();
        watch.start();
        function();
        watch.stop();

        return watch.getElapsedSeconds();
    }

    const double stopwatch::measure(const std::function<void(void)> &function, const string &functionName)
    {
        auto msg = functionName + " took: ";
        auto elapsedSeconds = stopwatch::measure(function);
        console::writeLine(msg + std::to_string(elapsedSeconds));

        return elapsedSeconds;
    }

    const double stopwatch::measureAverage(const std::function<void(void)> &function, int samples)
    {
        double average = 0;
        auto watch = stopwatch();

        for(auto i = 0; i < samples; i++)
        {
            watch.restart();
            function();
            watch.stop();
            average += watch.getElapsedSeconds();
        }

        return average / samples;
    }

    const double stopwatch::measureAverage(const std::function<void(void)> &function, const string &functionName, int samples)
    {
        double average = stopwatch::measureAverage(function, samples);

        auto msg = "average of " + std::to_string(samples) + " samples for " + functionName + " took: ";
        console::writeLine(msg + std::to_string(average));

        return average;
    }
}