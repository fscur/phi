#pragma once
#include <phi.h>
#include "coreApi.h"

namespace phi
{
    struct aabb
    {
    public:
        vec3 min;
        vec3 max;
        float width;
        float height;
        float depth;
        float halfWidth;
        float halfHeight;
        float halfDepth;
        vec3 center;
        float radius;

    public:
        CORE_API aabb();
        CORE_API aabb(vec3 min, vec3 max);
        CORE_API aabb(const aabb &original);

        CORE_API static aabb fromPoints(const vector<vec3> &points);
        CORE_API static aabb add(const aabb& a, const aabb& b);

        CORE_API bool contains(const vec3 position) const;
    };
}