#include "aabb.h"

namespace phi
{
    aabb::aabb(glm::vec3 min, glm::vec3 max)
    {
        _min = min;
        _max = max;
    }

    aabb::aabb(glm::vec3 position, size<float> size)
    {
        glm::vec3 s = glm::vec3(size.width, size.height, size.depth) / 2.0f;
        _min = position - s;
        _max = position + s;

        _center = (_min + _max) / 2.0f;

        _radius = glm::length(_min - _center);
    }

    aabb::aabb(std::vector<glm::vec3> points)
    {
        update(points);   
    }

    aabb::~aabb(){}

    void aabb::update(std::vector<glm::vec3> &points)
    {
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float minZ = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float maxY = std::numeric_limits<float>::lowest();
        float maxZ = std::numeric_limits<float>::lowest();

        glm::vec3* tmpPoints = &points[0];

        for (unsigned int i = 0, n = points.size(); i < n; i++)
        {
            glm::vec3 pos = tmpPoints[i];

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

        _min = glm::vec3(minX, minY, minZ);
        _max = glm::vec3(maxX, maxY, maxZ);

        _center = (_min + _max) / 2.0f;

        _radius = glm::length(_min - _center);
    }
}