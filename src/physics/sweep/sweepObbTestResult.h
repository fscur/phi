#pragma once

#include <phi.h>

#include "sweepObbCollision.h"

namespace phi
{
    namespace sweep
    {
        struct sweepObbTestResult
        {
            sweepObbTestResult() :
                collided(false),
                collisions()
            {
            }

            bool collided;
            vector<sweepObbCollision> collisions;
        };
    }
}