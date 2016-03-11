#include <precompiled.h>
#include "diagnostics.h"
#include "stopwatch.h"
#include "console.h"

using namespace std::chrono;

namespace phi
{
    stopwatch::stopwatch() :
        _initial(std::chrono::nanoseconds()),
        _stop(std::chrono::nanoseconds()),
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
            _initial = _stop = duration_cast<nanoseconds>(now.time_since_epoch());
        }
    }

    void stopwatch::stop()
    {
        if (!_isRunning)
            throw "idiot";

        auto now = high_resolution_clock::now();
        _stop = duration_cast<nanoseconds>(now.time_since_epoch());
        _isRunning = false;
    }

    void stopwatch::resume()
    {
        if (_isRunning)
            throw "idiot";

        auto now = high_resolution_clock::now().time_since_epoch();
        _initial += now - _stop;
        _isRunning = true;
    }

    double stopwatch::getElapsedSeconds()
    {
        auto now = high_resolution_clock::now().time_since_epoch();
        return duration_cast<duration<double>>(now - _initial).count();
    }

    double stopwatch::getElapsedMilliseconds()
    {
        auto now = high_resolution_clock::now().time_since_epoch();
        return duration_cast<duration<double>>(now - _initial).count() * 1000;
    }

    double stopwatch::measure(const std::function<void(void)> &function)
    {
        auto watch = stopwatch();
        watch.start();
        function();
        watch.stop();

        return watch.getElapsedSeconds();
    }

    double stopwatch::measure(const std::function<void(void)> &function, const string &functionName)
    {
        auto msg = functionName + " took: ";
        auto elapsedSeconds = stopwatch::measure(function);
        console::writeLine(msg + std::to_string(elapsedSeconds));

        return elapsedSeconds;
    }

    double stopwatch::measureAverage(const std::function<void(void)> &function, int samples)
    {
        double average = 0;
        auto watch = stopwatch();

        for(auto i = 0; i < samples; i++)
        {
            watch.resume();
            function();
            watch.stop();
            average += watch.getElapsedSeconds();
        }

        return average / samples;
    }

    double stopwatch::measureAverage(const std::function<void(void)> &function, const string &functionName, int samples)
    {
        double average = stopwatch::measureAverage(function, samples);

        auto msg = "average of " + std::to_string(samples) + " samples for " + functionName + " took: ";
        console::writeLine(msg + std::to_string(average));

        return average;
    }
}