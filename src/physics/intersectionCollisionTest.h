#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct intersectionCollisionTest
    {
        intersectionCollisionTest() :
            collider(nullptr),
            transform(nullptr),
            group(0u),
            inflation(0.0f)
        {
        }

        boxCollider* collider;
        transform* transform;
        uint16_t group;
        float inflation;
    };
}