#pragma once

#include <phi.h>

#include <core/boxCollider.h>

namespace phi
{
    namespace intersection
    {
        struct singleToSceneTest
        {
            singleToSceneTest() :
                collider(nullptr),
                transform(nullptr),
                group(0u)
            {
            }

            boxCollider* collider;
            transform* transform;
            uint16_t group;
        };
    }
}