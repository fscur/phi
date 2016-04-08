#pragma once
#include <phi.h>
#include "coreApi.h"
#include <diagnostics\stopwatch.h>

namespace phi
{
    class time
    {
    private:
        static stopwatch _stopwatch;

    public:
        CORE_API static double totalSeconds;
        CORE_API static double deltaSeconds;

    public:
        CORE_API static void start();
        CORE_API static void update();
    };
}