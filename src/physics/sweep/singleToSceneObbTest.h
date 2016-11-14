#pragma once

#include <phi.h>

#include "../intersection/singleToSceneObbTest.h"
#include "sweepTestParameters.h"

namespace phi
{
    namespace sweep
    {
        struct singleToSceneObbTest :
            public intersection::singleToSceneObbTest
        {
            singleToSceneObbTest() :
                intersection::singleToSceneObbTest(),
                parameters(sweepTestParameters())
            {
            }

            sweepTestParameters parameters;
        };
    }
}