#pragma once
#include <phi.h>
#include "coreApi.h"

#include <core\aabb.h>

namespace phi
{
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
        CORE_API bool intersects(aabb& aabb, vec3*& position, vec3*& normal, size_t& size);
        CORE_API bool intersects(vec3 bl, vec3 tl, vec3 tr, vec3 br, float& t);

        vec3 getOrigin() const { return _origin; }
        vec3 getDirection() const { return _direction; }

        CORE_API ray operator*(mat4& matrix);
    };
}