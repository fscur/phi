#pragma once
#include <phi.h>
#include "renderingApi.h"

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
        RENDERING_API ray(vec3 origin, vec3 direction);

        RENDERING_API bool intersects(const aabb& aabb, float& t);
        RENDERING_API bool intersects(const aabb& aabb);
        RENDERING_API bool intersects(aabb& aabb, vec3*& position, vec3*& normal, size_t& size);
        RENDERING_API bool intersects(vec3 bl, vec3 tl, vec3 tr, vec3 br, float& t);

        vec3 getOrigin() const { return _origin; }
        vec3 getDirection() const { return _direction; }

        RENDERING_API ray operator*(mat4& matrix);
    };
}