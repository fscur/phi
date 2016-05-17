#include <precompiled.h>
#include "ray.h"

#include <core\mathUtils.h>

namespace phi
{
    ray::ray(vec3 origin, vec3 direction) :
        _origin(origin),
        _direction(direction),
        _p2(origin + direction),
        _inverseDirection(1.0f / direction)
    {
    }

    bool ray::intersects(const aabb& aabb, float& t)
    {
        auto t0 = (aabb.min[0] - _origin[0]) * _inverseDirection[0];
        auto t1 = (aabb.max[0] - _origin[0]) * _inverseDirection[0];

        auto tmin = glm::min(t0, t1);
        auto tmax = glm::max(t0, t1);

        for (auto i = 1; i < 3; ++i) 
        {
            t0 = (aabb.min[i] - _origin[i]) * _inverseDirection[i];
            t1 = (aabb.max[i] - _origin[i]) * _inverseDirection[i];

            tmin = glm::max(tmin, glm::min(t0, t1));
            tmax = glm::min(tmax, glm::max(t0, t1));
        }

        t = tmin;
        return tmax > glm::max(tmin, 0.0f);
    }

    bool ray::intersects(const aabb& aabb)
    {
        auto t0 = (aabb.min[0] - _origin[0]) * _inverseDirection[0];
        auto t1 = (aabb.max[0] - _origin[0]) * _inverseDirection[0];

        auto tmin = glm::min(t0, t1);
        auto tmax = glm::max(t0, t1);

        for (auto i = 1; i < 3; ++i)
        {
            t0 = (aabb.min[i] - _origin[i]) * _inverseDirection[i];
            t1 = (aabb.max[i] - _origin[i]) * _inverseDirection[i];

            tmin = glm::max(tmin, glm::min(t0, t1));
            tmax = glm::min(tmax, glm::max(t0, t1));
        }

        return tmax > glm::max(tmin, 0.0f);
    }

    bool ray::intersects(aabb& aabb, vec3*& positions, vec3*& normals, size_t& count)
    {
        if (intersects(aabb))
        {
            auto min = aabb.min;
            auto max = aabb.max;
            auto lbb = vec3(min.x, min.y, min.z);
            auto lbf = vec3(min.x, min.y, max.z);
            auto ltf = vec3(min.x, max.y, max.z);
            auto ltb = vec3(min.x, max.y, min.z);
            auto rbb = vec3(max.x, min.y, min.z);
            auto rbf = vec3(max.x, min.y, max.z);
            auto rtf = vec3(max.x, max.y, max.z);
            auto rtb = vec3(max.x, max.y, min.z);

            std::map<float, vec3> intersections;
            float t;
            if (intersects(lbb, lbf, ltf, ltb, t))
                intersections[t] = vec3(-1.0f, 0.0f, 0.0f);
            if (intersects(rbf, rbb, rtb, rtf, t))
                intersections[t] = vec3(1.0f, 0.0f, 0.0f);
            if (intersects(lbf, rbf, rtf, ltf, t))
                intersections[t] = vec3(0.0f, 0.0f, 1.0f);
            if (intersects(rbb, lbb, ltb, rtb, t))
                intersections[t] = vec3(0.0f, 0.0f, -1.0f);
            if (intersects(ltf, rtf, rtb, ltb, t))
                intersections[t] = vec3(0.0f, 1.0f, 0.0f);
            if (intersects(lbb, rbb, rbf, lbf, t))
                intersections[t] = vec3(0.0f, -1.0f, 0.0f);

            count = intersections.size();
            positions = new vec3[count];
            normals = new vec3[count];

            auto i = 0;
            for (auto const &iter : intersections)
            {
                positions[i] = _origin + _direction * iter.first;
                normals[i] = iter.second;
                ++i;
            }

            return true;
        }

        return false;
    }

    bool ray::intersects(vec3 bl, vec3 br, vec3 tr, vec3 tl, float& t)
    {
        auto planeNormal = normalize(cross(bl - br, br - tr));
        auto d = dot(planeNormal, bl);
        t = (d - dot(planeNormal, _origin)) / (dot(planeNormal, (_direction)));

        float nDotA = dot(planeNormal, _origin);
        float nDotBA = dot(planeNormal, _direction);

        auto point = _origin + (((d - nDotA) / nDotBA) * _direction);

        return  dot(bl, tl - bl) <= dot(point, tl - bl) &&
            dot(point, tl - bl) <= dot(tl, tl - bl) &&
            dot(bl, br - bl) <= dot(point, br - bl) &&
            dot(point, br - bl) <= dot(br, br - bl);
    }

    ray ray::operator*(mat4& matrix)
    {
        auto start = mathUtils::multiply(matrix, _origin);
        auto end = mathUtils::multiply(matrix, _origin + _direction);
        auto direction = glm::normalize(end - start);

        return ray(start, direction);
    }
}