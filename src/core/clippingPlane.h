#pragma once
#include <phi.h>

#include "coreApi.h"
#include "plane.h"

namespace phi
{
    namespace clippingDistance
    {
        enum clippingDistance
        {
            positive,
            negative
        };
    }

    struct clippingPlane
    {
        phi::plane plane;

        clippingPlane() :
            plane(phi::plane(vec3(0.0f), vec3(0.0f)))
        {}

        clippingPlane(phi::plane plane) : //, clippingDistance::clippingDistance distance) :
            plane(plane)
        {}

        clippingPlane(const clippingPlane& original) :
            plane(original.plane)
        {
        }

        vec4 toVec4(clippingDistance::clippingDistance distance) const
        { 
            float sign = distance == clippingDistance::positive ? 1.0f : -1.0f;
            vec3 normal = sign * plane.normal;
            return vec4(normal, -dot(plane.origin, normal));
        }

        bool operator==(const clippingPlane& value)
        {
            return
                plane == value.plane;
        }

        bool operator!=(const clippingPlane& value)
        {
            return !(operator==(value));
        }
    };
}
