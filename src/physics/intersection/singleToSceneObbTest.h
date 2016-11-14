#pragma once

#include <phi.h>

#include <core/obb.h>

namespace phi
{
    namespace intersection
    {
        struct singleToSceneObbTest
        {
            singleToSceneObbTest() :
                collider(nullptr),
                group(0u)
            {
            }

            const obb* collider;
            uint16_t group;
        };
    }
}