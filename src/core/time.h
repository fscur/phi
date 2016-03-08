#pragma once
#include <phi.h>
#include "core.h"
#include <diagnostics\stopwatch.h>

namespace phi
{
    class time
    {
    private:
        static stopwatch _stopwatch;

    public:
        CORE_API static const double MILLISECONDS_PER_SECOND;

    public:
        CORE_API static double totalMilliseconds;
        CORE_API static double deltaMilliseconds;

    public:
        CORE_API static void start();
        CORE_API static void update();
    };
}