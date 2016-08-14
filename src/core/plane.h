#pragma once

#include <phi.h>

#include "coreApi.h"
#include "mathUtils.h"

namespace phi
{
    class plane
    {
    public:
        vec3 origin;
        vec3 normal;

    public:
        plane() :
            origin(vec3()),
            normal(vec3(0.0f, 0.0f, 1.0f))
        {
        }

        plane(vec3 origin, vec3 normal) :
            origin(origin),
            normal(normal)
        {
        }

        CORE_API vec3 projectPoint(const vec3 point) const;
        CORE_API float distanceFrom(const vec3 point) const;
        CORE_API bool intersectsLine(vec3 lineOrigin, vec3 lineDirection, float& t) const;

        bool operator==(const plane& plane)
        {
            return
                origin == plane.origin &&
                normal == plane.normal;
        }
    };
}