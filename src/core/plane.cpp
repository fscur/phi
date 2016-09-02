#include <precompiled.h>

#include "plane.h"

namespace phi
{
    vec3 plane::projectPoint(const vec3 point) const
    {
        auto distance = distanceFrom(point);
        return point - normal * distance;
    }

    float plane::distanceFrom(const vec3 point) const
    {
        return dot(point, normal) - dot(origin, normal);
    }

    bool plane::isParallel(const plane other) const
    {
        return mathUtils::isClose(glm::dot(other.normal, normal), 1.0f);
    }

    bool plane::intersectsLine(vec3 lineOrigin, vec3 lineDirection, float& t) const
    {
        auto lineDirectionOnNormal = glm::dot(normal, lineDirection);
        if (mathUtils::isClose(lineDirectionOnNormal, 0.0f))
            return false;

        t = glm::dot(normal, (origin - lineOrigin)) / lineDirectionOnNormal;
        return true;
    }

    vec4 plane::toVec4() const
    {
        return vec4(normal, -dot(origin, normal));
    }
}