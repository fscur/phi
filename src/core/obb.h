#pragma once

#include <phi.h>

#include "coreApi.h"

#include <core\transform.h>


namespace phi
{
    class obb
    {
    public:
        vec3 center;
        vec3 axes[3];
        vec3 halfSizes;

    public:
        obb(vec3 center, vec3 axisX, vec3 axisY, vec3 axisZ, vec3 halfSizes) :
            center(center),
            axes{ axisX, axisY, axisZ },
            halfSizes{ halfSizes.x, halfSizes.y, halfSizes.z }
        {
        }

        ~obb() {}

        CORE_API phi::obb transform(phi::transform* transform);

        CORE_API static bool intersects(const obb& a, const obb& b);
    };
}