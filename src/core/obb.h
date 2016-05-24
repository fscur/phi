#pragma once

#include <phi.h>
#include "coreApi.h"

namespace phi
{
    struct intersectionResult
    {
        intersectionResult() :
            time(0.0f),
            normal(vec3(0.0f)),
            colliding(true),
            intersecting(true),
            mtd(vec3(0.0f))
        {
        }

        float time;
        vec3 normal;
        bool colliding;
        bool intersecting;
        vec3 mtd;
    };

    class obb
    {
    public:
        vec3 center;
        vec3 axes[3];
        vec3 halfSizes;

    private:
        void project(const vec3& axis, float& min, float& max) const;

        static bool testAxisIntersection(const vec3& axis, float axis_length_squared, float d0, float d1, vec3& mtd, float& mtd2);

        static bool testAxisCollision(const vec3& axis, float axis_length_squared, float d0, float d1, float v,
            vec3& Nenter, vec3& Nleave, float& tenter, float& tleave);

        static bool testAxis(const vec3& axis, const obb& a, const obb& b, const vec3& vel,
            vec3& mtd, float& mtd2, bool& intersecting,
            vec3& Nenter, vec3& Nleave, float& tenter, float& tleave, bool& colliding);

    public:
        CORE_API obb(vec3 center, vec3 axisX, vec3 axisY, vec3 axisZ, vec3 halfSizes) :
            center(center),
            axes{ axisX, axisY, axisZ },
            halfSizes{ halfSizes.x, halfSizes.y, halfSizes.z }
        {
        }

        CORE_API ~obb() {}

        CORE_API static intersectionResult interesects(const obb& a, const vec3& va, const obb& b, const vec3& vb);

        CORE_API static bool intersects(const obb& a, const obb& b);
    };
}