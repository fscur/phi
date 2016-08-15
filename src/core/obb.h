#pragma once

#include <phi.h>

#include "coreApi.h"
#include "plane.h"
#include "finitePlane.h"
#include "transform.h"

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

        CORE_API static bool intersects(const obb& a, const obb& b);

        CORE_API obb transform(phi::transform* transform) const;
        CORE_API vec3 findClosestNormalTo(vec3 direction) const;
        CORE_API vec3 getPositionAt(vec3 direction) const;
        CORE_API vector<vec3> getCorners() const;
        CORE_API vector<plane> getPlanes() const;
        CORE_API vector<finitePlane> getFinitePlanes() const;
    };
}