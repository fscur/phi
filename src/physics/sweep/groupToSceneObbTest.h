#pragma once

#include <phi.h>

#include "../intersection/groupToSceneObbTest.h"
#include "sweepTestParameters.h"

namespace phi
{
    namespace sweep
    {
        struct groupToSceneObbTest :
            public intersection::groupToSceneObbTest
        {
            groupToSceneObbTest() :
                intersection::groupToSceneObbTest(),
                parameters(sweepTestParameters()),
                findOnlyClosestPerTarget(false)
            {
            }

            sweepTestParameters parameters;
            bool findOnlyClosestPerTarget;
        };
    }
}