#include <precompiled.h>
#include "time.h"

namespace phi
{
    const double time::MILLISECONDS_PER_SECOND = 1000.0;
    double time::totalMilliseconds = 0;
    double time::deltaMilliseconds = 0;
    stopwatch time::_stopwatch = stopwatch();

    void time::start()
    {
        _stopwatch.start();
    }

    void time::update()
    {
        auto totalMillisecondsLastFrame = totalMilliseconds;
        auto totalMilliseconds = _stopwatch.getElapsedMilliseconds();
        deltaMilliseconds = totalMilliseconds - totalMillisecondsLastFrame;
    }
}