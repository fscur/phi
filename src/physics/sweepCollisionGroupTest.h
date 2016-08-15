#pragma once

#include <phi.h>

#include "intersectionCollisionGroupTest.h"

namespace phi
{
    struct sweepCollisionGroupTest :
        public intersectionCollisionGroupTest
    {
        sweepCollisionGroupTest() :
            intersectionCollisionGroupTest(),
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