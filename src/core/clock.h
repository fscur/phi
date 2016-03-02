#pragma once

#include "core.h"

namespace phi
{
    class clock
    {
    public:
        CORE_API static int totalMillisecondsElapsed;
        CORE_API static int millisecondsElapsed;
    };
}