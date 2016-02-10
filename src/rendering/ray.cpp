#include "ray.h"

namespace phi
{
    ray::ray(vec3 origin, vec3 direction)
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

    bool ray::intersects(aabb* aabb, vec3* normal)
    {
        if (intersects(aabb))
        {
            auto min = aabb->getMin();
            auto max = aabb->getMax();
            auto lbb = vec3(min.x, min.y, min.z);
            auto lbf = vec3(min.x, min.y, max.z);
            auto ltf = vec3(min.x, max.y, max.z);
            auto ltb = vec3(min.x, max.y, min.z);
            auto rbb = vec3(max.x, min.y, min.z);
            auto rbf = vec3(max.x, min.y, max.z);
            auto rtf = vec3(max.x, max.y, max.z);
            auto rtb = vec3(max.x, max.y, min.z);

            auto minT = std::numeric_limits<float>::max();
            auto minNormal = vec3();
            float t;
            if (intersects(lbb, lbf, ltf, ltb, &t))
            {
                if (t < minT)
                {
                    minT = t;
                    minNormal = vec3(-1.0f, 0.0f, 0.0f);
                }
            }
            if (intersects(rbf, rbb, rtb, rtf, &t))
            {
                if (t < minT)
                {
                    minT = t;
                    minNormal = vec3(1.0f, 0.0f, 0.0f);
                }
            }
            if (intersects(lbf, rbf, rtf, ltf, &t))
            {
                if (t < minT)
                {
                    minT = t;
                    minNormal = vec3(0.0f, 0.0f, 1.0f);
                }
            }
            if (intersects(rbb, lbb, ltb, rtb, &t))
            {
                if (t < minT)
                {
                    minT = t;
                    minNormal = vec3(0.0f, 0.0f, -1.0f);
                }
            }
            if (intersects(ltf, rtf, rtb, ltb, &t))
            {
                if (t < minT)
                {
                    minT = t;
                    minNormal = vec3(0.0f, 1.0f, 0.0f);
                }
            }
            if (intersects(lbb, rbb, rbf, lbf, &t))
            {
                if (t < minT)
                {
                    minT = t;
                    minNormal = vec3(0.0f, -1.0f, 0.0f);
                }
            }

            *normal = minNormal;
            return true;
        }

        return false;
    }

    bool ray::intersects(vec3 bl, vec3 br, vec3 tr, vec3 tl, float* t)
    {
        auto planeNormal = normalize(cross(bl - br, br - tr));
        auto d = dot(planeNormal, bl);
        *t = (d - dot(planeNormal, _origin))/(dot(planeNormal, (_direction)));
        float nDotA = dot(planeNormal, _origin);
        float nDotBA = dot(planeNormal, _direction);
        auto point = _origin + (((d - nDotA)/nDotBA) * _direction);
        auto in = dot(bl, tl-bl) <= dot(point, tl-bl) &&
            dot(point, tl-bl) <= dot(tl, tl-bl) &&
            dot(bl, br-bl) <= dot(point, br-bl) &&
            dot(point, br-bl) <= dot(br, br-bl);

        return in;
    }
}