#pragma once

#include <phi.h>

#include "intersectionCollisionMultiTest.h"

namespace phi
{
    struct sweepCollisionMultiTest :
        public intersectionCollisionMultiTest
    {
        sweepCollisionMultiTest() :
            intersectionCollisionMultiTest(),
            distance(0.0f),
            direction(vec3()),
            disregardDivergentNormals(true),
            findOnlyClosestPerTarget(false),
            maximumHits(32u),
            inflation(0.0f)
        {
        }

        float distance;
        vec3 direction;
        bool disregardDivergentNormals;
        bool findOnlyClosestPerTarget;
        uint32_t maximumHits;
        float inflation;
    };
}