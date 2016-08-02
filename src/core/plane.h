#pragma once

#include <phi.h>

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

        vec3 projectPoint(const vec3 point) const
        {
            auto dist = dot(point, normal) - dot(origin, normal);
            return point - normal * dist;
        }

        bool operator==(const plane& plane)
        {
            return
                origin == plane.origin &&
                normal == plane.normal;
        }
    };
}