#pragma once

#include <phi.h>

#include <core/boxCollider.h>

namespace phi
{
    struct groupToGroupTest
    {
        intersectionCollisionGroupToGroupTest() :
            colliders(),
            transforms(),
            collidees(),
            group(0u)
        {
        }

        vector<boxCollider*>* colliders;
        vector<transform*>* transforms;
        vector<boxCollider*>* collidees;
        uint16_t group;
    };
}