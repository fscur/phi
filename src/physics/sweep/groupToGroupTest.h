#pragma once

#include <phi.h>

#include "../intersection/groupToGroupTest.h"

namespace phi
{
    namespace sweep
    {
        struct groupToGroupTest :
            public intersection::groupToGroupTest
        {
            groupToGroupTest() :
                intersection::groupToGroupTest(),
                parameters(sweepTestParameters()),
                findOnlyClosestPerTarget(false)
            {
            }

            sweepTestParameters parameters;
            bool findOnlyClosestPerTarget;
        };
    }
}