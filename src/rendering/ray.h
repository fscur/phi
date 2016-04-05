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

    public:
        RENDERING_API ray(vec3 origin = vec3(), vec3 direction = vec3());
        RENDERING_API ~ray();

        RENDERING_API bool intersects(aabb* aabb);
        RENDERING_API bool intersects(aabb* aabb, vec3* normal);
        RENDERING_API bool intersects(vec3 bl, vec3 tl, vec3 tr, vec3 br, float* t);

        vec3 getOrigin() const { return _origin; }
        vec3 getDirection() const { return _direction; }

    };
}