#pragma once
#include <phi.h>
#include "coreApi.h"

#include <core\aabb.h>
#include <core\obb.h>
#include <core\plane.h>
#include <core\finitePlane.h>

namespace phi
{
    struct rayIntersection
    {
        vec3 position;
        vec3 normal;
        float t;
    };

    class ray
    {
    private:
        vec3 _origin;
        vec3 _direction;
        vec3 _p2;
        vec3 _inverseDirection;

    public:
        CORE_API ray(vec3 origin, vec3 direction);

        CORE_API bool intersects(const aabb& aabb, float& t);
        CORE_API bool intersects(const aabb& aabb);
        CORE_API bool intersects(aabb& aabb, vector<rayIntersection>& intersections);
        CORE_API bool intersects(obb& obb, vector<rayIntersection>& intersections);
        CORE_API bool intersects(plane& plane, float& t);
        CORE_API bool intersects(finitePlane& plane, float& t);

        vec3 getOrigin() const { return _origin; }
        vec3 getDirection() const { return _direction; }

        CORE_API ray operator*(mat4& matrix);
    };
}