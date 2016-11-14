#pragma once

#include <phi.h>

#include "../intersection/groupToSceneTest.h"
#include "sweepTestParameters.h"

namespace phi
{
    namespace sweep
    {
        struct groupToSceneTest :
            public intersection::groupToSceneTest
        {
            groupToSceneTest() :
                intersection::groupToSceneTest(),
                parameters(sweepTestParameters()),
                findOnlyClosestPerTarget(false)
            {
            }

            sweepTestParameters parameters;
            bool findOnlyClosestPerTarget;
        };
    }
}