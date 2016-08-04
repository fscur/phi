#pragma once

#include <phi.h>

#include "intersectionCollisionTest.h"

namespace phi
{
    struct sweepCollisionTest :
        public intersectionCollisionTest
    {
        sweepCollisionTest() :
            intersectionCollisionTest(),
            distance(0.0f),
            direction(vec3()),
            disregardDivergentNormals(true),
            maximumHits(32u)
        {
        }

        float distance;
        vec3 direction;
        bool disregardDivergentNormals;
        uint32_t maximumHits;
    };
}