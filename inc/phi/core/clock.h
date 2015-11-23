#ifndef _PHI_CLOCK_H_
#define _PHI_CLOCK_H_

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

#endif