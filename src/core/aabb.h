#pragma once
#include <phi.h>
#include "coreApi.h"

namespace phi
{
    struct aabb
    {
    public:
        const vec3 min;
        const vec3 max;
        const float width;
        const float height;
        const float depth;
        const float halfWidth;
        const float halfHeight;
        const float halfDepth;
        const vec3 center;
        const float radius;

    public:
        CORE_API aabb();
        CORE_API aabb(vec3 min, vec3 max);
        CORE_API aabb(const aabb &original);

        CORE_API static aabb fromPoints(const vector<vec3> &points);
        CORE_API static aabb add(const aabb a, const aabb b);

        CORE_API bool contains(const vec3 position) const;
    };
}