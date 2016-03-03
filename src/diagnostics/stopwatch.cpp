#include <precompiled.h>
#include "diagnostics.h"
#include "stopwatch.h"
#include "console.h"

using namespace std::chrono;

namespace phi
{
    stopwatch::stopwatch() :
        _initial(nanoseconds()),
        _final(nanoseconds()),
        _currentNanoSeconds(nanoseconds()),
        _isRunning(false)
    {
    }
    
    stopwatch::~stopwatch()
    {
        
    }
    
    void stopwatch::start()
    {
        if(!_isRunning)
        {
            _isRunning = true;
            auto now = high_resolution_clock::now();
            _initial = duration_cast<nanoseconds>(now.time_since_epoch());
        }
    }

    void stopwatch::stop()
    {
        if(_isRunning)
        {
            auto now = high_resolution_clock::now();
            _final = duration_cast<nanoseconds>(now.time_since_epoch());
            _currentNanoSeconds += _final - _initial;
            _isRunning = false;
        }
    }

    void stopwatch::reset()
    {
        _currentNanoSeconds = nanoseconds::zero();
    }

    void stopwatch::restart()
    {
        reset();
        start();
    }

    const double stopwatch::getElapsedSeconds()
    {
        return duration_cast<duration<double>>(_currentNanoSeconds).count();
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