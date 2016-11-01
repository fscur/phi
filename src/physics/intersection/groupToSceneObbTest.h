#pragma once

#include <phi.h>
#include <core/obb.h>
#include <core/transform.h>

namespace phi
{
    namespace intersection
    {
        struct groupToSceneObbTest
        {
            groupToSceneObbTest() :
                colliders(),
                group(0u)
            {
            }

            vector<const obb*>* colliders;
            uint16_t group;
        };
    }
}