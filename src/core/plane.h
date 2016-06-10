#pragma once

#include <phi.h>

namespace phi
{
    class plane
    {
    private:
        vec3 _origin;
        vec3 _normal;

    public:
        plane(vec3 origin, vec3 normal) :
            _origin(origin),
            _normal(normal)
        {
        }

        vec3 getOrigin() { return _origin; }
        vec3 getNormal() { return _normal; }

        vec3 projectPoint(vec3 point)
        {
            auto dist = dot(point, _normal) - dot(_origin, _normal);
            return point - _normal * dist;
        }
    };
}