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
            if (intersects(finitePlane(lbb, lbf, ltb), t))
                intersections[t] = vec3(-1.0f, 0.0f, 0.0f);
            if (intersects(finitePlane(rbf, rbb, rtf), t))
                intersections[t] = vec3(1.0f, 0.0f, 0.0f);
            if (intersects(finitePlane(lbf, rbf, ltf), t))
                intersections[t] = vec3(0.0f, 0.0f, 1.0f);
            if (intersects(finitePlane(rbb, lbb, rtb), t))
                intersections[t] = vec3(0.0f, 0.0f, -1.0f);
            if (intersects(finitePlane(ltf, rtf, ltb), t))
                intersections[t] = vec3(0.0f, 1.0f, 0.0f);
            if (intersects(finitePlane(lbb, rbb, lbf), t))
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

    bool ray::intersects(obb& obb, vec3*& positions, vec3*& normals, size_t& count)
    {
        auto getPoint = [&obb](vec3 pos) -> vec3
        {
            return obb.center + obb.axes[0] * obb.halfSizes.x * pos.x + obb.axes[1] * obb.halfSizes.y * pos.y + obb.axes[2] * obb.halfSizes.z * pos.z;
        };

        auto lbb = getPoint(vec3(-1.0f, -1.0f, -1.0f));
        auto lbf = getPoint(vec3(-1.0f, -1.0f, 1.0f));
        auto ltf = getPoint(vec3(-1.0f, 1.0f, 1.0f));
        auto ltb = getPoint(vec3(-1.0f, 1.0f, -1.0f));
        auto rbb = getPoint(vec3(1.0f, -1.0f, -1.0f));
        auto rbf = getPoint(vec3(1.0f, -1.0f, 1.0f));
        auto rtf = getPoint(vec3(1.0f, 1.0f, 1.0f));
        auto rtb = getPoint(vec3(1.0f, 1.0f, -1.0f));

        std::map<float, vec3> intersections;
        float t;
        if (intersects(finitePlane(lbb, lbf, ltb), t))
            intersections[t] = -obb.axes[0];
        if (intersects(finitePlane(rbf, rbb, rtf), t))
            intersections[t] = obb.axes[0];
        if (intersects(finitePlane(lbf, rbf, ltf), t))
            intersections[t] = obb.axes[2];
        if (intersects(finitePlane(rbb, lbb, rtb), t))
            intersections[t] = -obb.axes[2];
        if (intersects(finitePlane(ltf, rtf, ltb), t))
            intersections[t] = obb.axes[1];
        if (intersects(finitePlane(lbb, rbb, lbf), t))
            intersections[t] = -obb.axes[1];

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

        return count > 0;
    }

    bool ray::intersects(plane& plane, float& t)
    {
        auto planeNormal = plane.getNormal();
        auto planeOrigin = plane.getOrigin();
        auto planeNormalOnOrigin = dot(planeNormal, _origin);
        auto planeNormalOnDirection = dot(planeNormal, _direction);

        auto planeNormalOnBl = dot(planeNormal, planeOrigin);
        t = (planeNormalOnBl - planeNormalOnOrigin) / planeNormalOnDirection;

        return !mathUtils::isClose(planeNormalOnDirection, 0.0f);
    }

    bool ray::intersects(finitePlane& plane, float& t)
    {
        auto planeNormal = plane.getNormal();
        auto planeOrigin = plane.getOrigin();
        auto planeNormalOnOrigin = dot(planeNormal, _origin);
        auto planeNormalOnDirection = dot(planeNormal, _direction);

        auto planeNormalOnBl = dot(planeNormal, planeOrigin);
        t = (planeNormalOnBl - planeNormalOnOrigin) / planeNormalOnDirection;

        auto point = _origin + t * _direction;
        auto xVector = plane.getRightPoint() - planeOrigin;
        auto yVector = plane.getUpPoint() - planeOrigin;

        return
            dot(planeOrigin, yVector) <= dot(point, yVector) &&
            dot(point, yVector) <= dot(planeOrigin + yVector, yVector) &&
            dot(planeOrigin, xVector) <= dot(point, xVector) &&
            dot(point, xVector) <= dot(planeOrigin + xVector, xVector);
    }

    ray ray::operator*(mat4& matrix)
    {
        auto start = mathUtils::multiply(matrix, _origin);
        auto end = mathUtils::multiply(matrix, _origin + _direction);
        auto direction = glm::normalize(end - start);

        return ray(start, direction);
    }
}