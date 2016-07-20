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
            findOnlyClosestPerTarget(false)
        {
        }

        float distance;
        vec3 direction;
        bool disregardDivergentNormals;
        bool findOnlyClosestPerTarget;
    };
}