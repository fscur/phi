#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct intersectionCollisionGroupTest
    {
        intersectionCollisionGroupTest() :
            colliders(),
            transforms(),
            againstColliders(),
            group(0u)
        {
        }

        vector<boxCollider*>* colliders;
        vector<transform*>* transforms;
        vector<boxCollider*>* againstColliders;
        uint16_t group;
    };
}