#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct sweepCollisionPairTest
    {
        sweepCollisionPairTest() :
            colliderSource(nullptr),
            transformSource(nullptr),
            offset(vec3()),
            colliderTarget(nullptr),
            transformTarget(nullptr)
        {
        }

        boxCollider* colliderSource;
        transform* transformSource;
        vec3 offset;
        boxCollider* colliderTarget;
        transform* transformTarget;
    };
}