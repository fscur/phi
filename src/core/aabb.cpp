#include <precompiled.h>
#include "aabb.h"

namespace phi
{
    aabb::aabb() :
        min(vec3()),
        max(vec3()),
        width(0.0f),
        height(0.0f),
        depth(0.0f),
        halfWidth(0.0f),
        halfHeight(0.0f),
        halfDepth(0.0f),
        center(vec3()),
        radius(0.0f)
    {
    }

    aabb::aabb(vec3 min, vec3 max) :
        min(min),
        max(max),
        width(max.x - min.x),
        height(max.y - min.y),
        depth(max.z - min.z),
        halfWidth((max.x - min.x) * 0.5f),
        halfHeight((max.y - min.y) * 0.5f),
        halfDepth((max.z - min.z) * 0.5f),
        center((min + max) * 0.5f),
        radius(glm::length(min - center))
    {
    }

    aabb::aabb(const aabb& original) :
        aabb(original.min, original.max)
    {
    }

    aabb aabb::fromPoints(const vector<vec3> &points)
    {
        auto minX = std::numeric_limits<float>::max();
        auto minY = std::numeric_limits<float>::max();
        auto minZ = std::numeric_limits<float>::max();
        auto maxX = std::numeric_limits<float>::lowest();
        auto maxY = std::numeric_limits<float>::lowest();
        auto maxZ = std::numeric_limits<float>::lowest();

        for (auto pos : points)
        {
            if (pos.x < minX)
                minX = pos.x;

            if (pos.y < minY)
                minY = pos.y;

            if (pos.z < minZ)
                minZ = pos.z;

            if (pos.x > maxX)
                maxX = pos.x;

            if (pos.y > maxY)
                maxY = pos.y;

            if (pos.z > maxZ)
                maxZ = pos.z;
        }

        auto min = vec3(minX, minY, minZ);
        auto max = vec3(maxX, maxY, maxZ);

        return aabb(min, max);
    }

    bool aabb::contains(vec3 pos) const
    {
        return
            min.x <= pos.x && pos.x < max.x &&
            min.y <= pos.y && pos.y < max.y &&
            min.z <= pos.z && pos.z < max.z;
    }

    aabb aabb::add(const aabb& a, const aabb& b)
    {
        auto min = vec3(
            glm::min(a.min.x, b.min.x),
            glm::min(a.min.y, b.min.y),
            glm::min(a.min.z, b.min.z));
        auto max = vec3(
            glm::max(a.max.x, b.max.x),
            glm::max(a.max.y, b.max.y),
            glm::max(a.max.z, b.max.z));

        return aabb(min, max);
    }
}