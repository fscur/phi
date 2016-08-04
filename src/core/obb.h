#pragma once

#include <phi.h>

#include "coreApi.h"

#include <core\transform.h>

namespace phi
{
    class obb
    {
    public:
        vec3 center;
        vec3 axes[3];
        vec3 halfSizes;

    public:
        obb(vec3 center, vec3 axisX, vec3 axisY, vec3 axisZ, vec3 halfSizes) :
            center(center),
            axes{ axisX, axisY, axisZ },
            halfSizes{ halfSizes.x, halfSizes.y, halfSizes.z }
        {
        }

        ~obb() {}

        CORE_API phi::obb transform(phi::transform* transform);

        CORE_API static bool intersects(const obb& a, const obb& b);

        vec3 findClosestNormal(vec3 direction)
        {
            auto highestDot = -1.0f;
            auto closestNormal = direction;

            auto checkClosest = [&highestDot, &closestNormal, direction](vec3 normal)
            {
                auto dot = glm::dot(normal, direction);
                if (dot > highestDot)
                {
                    highestDot = dot;
                    closestNormal = normal;
                }
            };

            checkClosest(axes[0]);
            checkClosest(-axes[0]);
            checkClosest(axes[1]);
            checkClosest(-axes[1]);
            checkClosest(axes[2]);
            checkClosest(-axes[2]);

            //checkClosest(glm::normalize(axes[0] + axes[1]));
            //checkClosest(glm::normalize(axes[0] + axes[2]));
            //checkClosest(glm::normalize(axes[0] + -axes[1]));
            //checkClosest(glm::normalize(axes[0] + -axes[2]));
            //checkClosest(glm::normalize(-axes[0] + axes[1]));
            //checkClosest(glm::normalize(-axes[0] + axes[2]));
            //checkClosest(glm::normalize(-axes[0] + -axes[1]));
            //checkClosest(glm::normalize(-axes[0] + -axes[2]));
            //checkClosest(glm::normalize(axes[2] + axes[1]));
            //checkClosest(glm::normalize(axes[2] + -axes[1]));
            //checkClosest(glm::normalize(-axes[2] + axes[1]));
            //checkClosest(glm::normalize(-axes[2] + -axes[1]));

            return closestNormal;
        }

        vec3 getPositionAt(vec3 direction)
        {
            return center + direction *
                (glm::abs(glm::dot(axes[0] * halfSizes.x, direction)) +
                 glm::abs(glm::dot(axes[1] * halfSizes.y, direction)) +
                 glm::abs(glm::dot(axes[2] * halfSizes.z, direction)));
        }
    };
}