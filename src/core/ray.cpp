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

    bool ray::intersects(aabb& aabb, vector<rayIntersection>& intersections)
    {
        if (intersects(aabb))
        {
            intersections = vector<rayIntersection>();

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

            auto addIntersection = [&](vec3 normal, float t)
            {
                rayIntersection intersection;
                intersection.normal = normal;
                intersection.t = t;
                intersection.position = _origin + _direction * normal;
                intersections.push_back(intersection);
            };

            float t;
            if (intersects(orientedPlane(lbb, lbf, ltb), t))
                addIntersection(vec3(-1.0f, 0.0f, 0.0f), t);
            if (intersects(orientedPlane(rbf, rbb, rtf), t))
                addIntersection(vec3(1.0f, 0.0f, 0.0f), t);
            if (intersects(orientedPlane(lbf, rbf, ltf), t))
                addIntersection(vec3(0.0f, 0.0f, 1.0f), t);
            if (intersects(orientedPlane(rbb, lbb, rtb), t))
                addIntersection(vec3(0.0f, 0.0f, -1.0f), t);
            if (intersects(orientedPlane(ltf, rtf, ltb), t))
                addIntersection(vec3(0.0f, 1.0f, 0.0f), t);
            if (intersects(orientedPlane(lbb, rbb, lbf), t))
                addIntersection(vec3(0.0f, -1.0f, 0.0f), t);

            return true;
        }

        std::sort(intersections.begin(), intersections.end(), [](const rayIntersection& a, const rayIntersection& b) -> bool
        {
            return a.t < b.t;
        });

        return false;
    }

    bool ray::intersects(obb& obb, vector<rayIntersection>& intersections)
    {
        auto obbSides = obb.getSidesPlanes();

        for (auto& side : obbSides)
        {
            float t;
            if (intersects(side, t))
            {
                rayIntersection intersection;
                intersection.normal = side.getNormal();
                intersection.t = t;
                intersection.position = _origin + _direction * t;
                intersections.push_back(intersection);
            }
        }

        std::sort(intersections.begin(), intersections.end(), [](const rayIntersection& a, const rayIntersection& b) -> bool
        {
            return a.t < b.t;
        });

        return intersections.size() > 0;
    }

    bool ray::intersects(plane& plane, float& t)
    {
        auto planeNormal = plane.normal;
        auto planeOrigin = plane.origin;
        auto planeNormalOnOrigin = dot(planeNormal, _origin);
        auto planeNormalOnDirection = dot(planeNormal, _direction);

        auto planeNormalOnBl = dot(planeNormal, planeOrigin);
        t = (planeNormalOnBl - planeNormalOnOrigin) / planeNormalOnDirection;

        return !mathUtils::isClose(planeNormalOnDirection, 0.0f);
    }

    bool ray::intersects(orientedPlane& plane, float& t)
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