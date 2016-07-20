#pragma once

#include <phi.h>

#include "sweepCollision.h"

namespace phi
{
    struct sweepCollisionResult
    {
        sweepCollisionResult() :
            collided(false),
            collisions()
        {
        }

        bool collided;
        vector<sweepCollision> collisions;
    };
}