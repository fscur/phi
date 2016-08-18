#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct sweepCollisionPairTest
    {
        sweepCollisionPairTest() :
            collider(nullptr),
            colliderTransform(nullptr),
            distance(0.0f),
            direction(vec3()),
            collidee(nullptr),
            collideeTransform(nullptr),
            inflation(0.0f),
            checkPenetration(false)
        {
        }

        boxCollider* collider;
        transform* colliderTransform;
        float distance;
        vec3 direction;
        boxCollider* collidee;
        transform* collideeTransform;
        float inflation;
        bool checkPenetration;
    };
}