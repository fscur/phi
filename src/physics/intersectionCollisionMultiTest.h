#pragma once

#include <phi.h>

#include <core\boxCollider.h>

namespace phi
{
    struct intersectionCollisionMultiTest
    {
        intersectionCollisionMultiTest() :
            colliders(),
            transforms(),
            group(0u)
        {
        }

        vector<boxCollider*>* colliders;
        vector<transform*>* transforms;
        uint16_t group;
    };
}