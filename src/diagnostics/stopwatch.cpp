#include <phi\diagnostics\stopwatch.h>

namespace phi
{
    void stopwatch::Start()
    {
        if(!_isRunning)
        {
            _isRunning = true;
            _initial = std::chrono::high_resolution_clock::now();
        }
    }

    void stopwatch::Stop()
    {
        if(_isRunning)
        {
            _final = std::chrono::high_resolution_clock::now();
            _currentNanoSeconds += _final - _initial;
            _isRunning = false;
        }
    }

    void stopwatch::Reset()
    {
        _currentNanoSeconds = std::chrono::nanoseconds::zero();
    }

    void stopwatch::Restart()
    {
        Reset();
        Start();
    }

    const double stopwatch::GetElapsedSeconds()
    {
        return std::chrono::duration_cast<std::chrono::duration<double>>(_currentNanoSeconds).count();
    }
}