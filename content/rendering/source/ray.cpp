#include "ray.h"

namespace phi
{
    ray::ray(glm::vec3 origin, glm::vec3 direction)
    {
        _origin = origin;
        _direction = direction;
    }

    ray::~ray(){}

    bool ray::intersects(aabb* aabb)
    {
        float minX = aabb->getMin().x;
        float minY = aabb->getMin().y;
        float minZ = aabb->getMin().z;
        float maxX = aabb->getMax().x;
        float maxY = aabb->getMax().y;
        float maxZ = aabb->getMax().z;
        float x0 = _origin.x;
        float y0 = _origin.y;
        float z0 = _origin.z;
        float xd = _direction.x;
        float yd = _direction.y;
        float zd = _direction.z;

        float tx0 = std::numeric_limits<float>::min();
        float tx1 = std::numeric_limits<float>::max();
        float ty0 = std::numeric_limits<float>::min();
        float ty1 = std::numeric_limits<float>::max();
        float tz0 = std::numeric_limits<float>::min();
        float tz1 = std::numeric_limits<float>::max();

        float tMin = std::numeric_limits<float>::min();
        float tMax = std::numeric_limits<float>::max();
        float temp;

        if (xd == 0 && (x0 < minX || x0 > maxX))
            return false;
        else if (xd != 0)
        {
            tx0 = (minX - x0) / xd;
            tx1 = (maxX - x0) / xd;

            if (tx0 > tx1)
            {
                temp = tx0;
                tx0 = tx1;
                tx1 = temp;
            }

            if (tx0 > tMin)
                tMin = tx0;
            if (tx1 < tMax)
                tMax = tx1;

            if (tMax < 0 || tMin > tMax)
                return false;
        }

        if (yd == 0 && (y0 < minY || y0 > maxY))
            return false;
        else if (yd != 0)
        {
            ty0 = (minY - y0) / yd;
            ty1 = (maxY - y0) / yd;

            if (ty0 > ty1)
            {
                temp = ty0;
                ty0 = ty1;
                ty1 = temp;
            }

            if (ty0 > tMin)
                tMin = ty0;
            if (ty1 < tMax)
                tMax = ty1;

            if (tMax < 0 || tMin > tMax)
                return false;
        }

        if (zd == 0 && (z0 < minZ || z0 > maxZ))
            return false;
        else if (zd != 0)
        {
            tz0 = (minZ - z0) / zd;
            tz1 = (maxZ - z0) / zd;

            if (tz0 > tz1)
            {
                temp = tz0;
                tz0 = tz1;
                tz1 = temp;
            }

            if (tz0 > tMin)
                tMin = tz0;
            if (tz1 < tMax)
                tMax = tz1;

            if (tMax < 0 || tMin > tMax)
                return false;
        }

        if (tMax < 0 || tMin > tMax)
            return false;

        return true;
    }
}