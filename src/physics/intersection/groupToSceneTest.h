#pragma once

#include <phi.h>

#include <core/boxCollider.h>

namespace phi
{
    namespace intersection
    {
        struct groupToSceneTest
        {
            groupToSceneTest() :
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
}