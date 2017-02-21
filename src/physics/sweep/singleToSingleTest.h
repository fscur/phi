#pragma once

#include <phi.h>

#include "../intersection/singleToSingleTest.h"

namespace phi
{
    namespace sweep
    {
        struct singleToSingleTest :
            public intersection::singleToSingleTest
        {
            singleToSingleTest() :
                intersection::singleToSingleTest(),
                parameters(sweepTestParameters())
            {
            }

            sweepTestParameters parameters;
        };
    }
}