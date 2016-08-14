#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct sweepCollisionPairTest
    {
        sweepCollisionPairTest() :
            sourceCollider(nullptr),
            sourceTransform(nullptr),
            distance(0.0f),
            direction(vec3()),
            targetCollider(nullptr),
            targetTransform(nullptr),
            inflation(0.0f),
            checkPenetration(false)
        {
        }

        boxCollider* sourceCollider;
        transform* sourceTransform;
        float distance;
        vec3 direction;
        boxCollider* targetCollider;
        transform* targetTransform;
        float inflation;
        bool checkPenetration;
    };
}