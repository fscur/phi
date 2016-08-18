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
        clippingDistance::clippingDistance distance;

        clippingPlane() :
            plane(phi::plane()),
            distance(clippingDistance::positive)
        {}

        clippingPlane(phi::plane plane, clippingDistance::clippingDistance distance) :
            plane(plane),
            distance(distance)
        {}

        clippingPlane(const clippingPlane& original) :
            plane(original.plane),
            distance(original.distance)
        {
        }

        vec4 toVec4() const 
        { 
            float sign = distance == clippingDistance::positive ? 1.0f : -1.0f;
            vec3 normal = sign * plane.normal;
            return vec4(normal, -dot(plane.origin, normal));
        }

        bool operator==(const clippingPlane& value)
        {
            return
                plane == value.plane &&
                distance == value.distance;
        }

        bool operator!=(const clippingPlane& value)
        {
            return !(operator==(value));
        }
    };
}
