#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct intersectionCollisionPairTest
    {
        intersectionCollisionPairTest() :
            collider(nullptr),
            colliderTransform(nullptr),
            collidee(nullptr),
            collideeTransform(nullptr),
            inflation(0.0f)
        {
        }

        boxCollider* collider;
        transform* colliderTransform;
        boxCollider* collidee;
        transform* collideeTransform;
        float inflation;
    };
}