#pragma once

#include <phi.h>

#include "../intersection/singleToSceneTest.h"
#include "sweepTestParameters.h"

namespace phi
{
    namespace sweep
    {
        struct singleToSceneTest :
            public intersection::singleToSceneTest
        {
            singleToSceneTest() :
                intersection::singleToSceneTest(),
                parameters(sweepTestParameters())
            {
            }

            sweepTestParameters parameters;
        };
    }
}