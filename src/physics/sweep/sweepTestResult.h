#pragma once

#include <phi.h>

#include "sweepCollision.h"

namespace phi
{
    namespace sweep
    {
        struct sweepTestResult
        {
            sweepTestResult() :
                collided(false),
                collisions()
            {
            }

            bool collided;
            vector<sweepCollision> collisions;
        };
    }
}