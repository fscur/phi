#pragma once

#include <phi.h>

namespace phi
{
    class finitePlane
    {
    private:
        vec3 _origin;
        vec3 _rightPoint;
        vec3 _upPoint;
        vec3 _normal;
        vec3 _xAxis;
        vec3 _yAxis;

    public:
        finitePlane() :
            finitePlane(vec3(), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f))
        {
        }

        finitePlane(vec3 origin, vec3 rightPoint, vec3 upPoint) :
            _origin(origin),
            _rightPoint(rightPoint),
            _upPoint(upPoint),
            _normal(glm::normalize(glm::cross(rightPoint - origin, upPoint - origin))),
            _xAxis(glm::normalize(rightPoint - origin)),
            _yAxis(glm::normalize(upPoint - origin))
        {
            // TODO: Do we need to test if the 3 points do not form a right triangle (which causes the plane to have an incorrect coordinate system)?
            // For now, it causes undefined behaviour
        }

        vec3 getOrigin() { return _origin; }
        vec3 getRightPoint() { return _rightPoint; }
        vec3 getUpPoint() { return _upPoint; }
        vec3 getNormal() { return _normal; }
        vec3 getXAxis() { return _xAxis; }
        vec3 getYAxis() { return _yAxis; }

        vec2 projectPoint(vec3 point)
        {
            auto x = dot(_xAxis, point - _origin);
            auto y = dot(_yAxis, point - _origin);

            return vec2(x, y);
        }
    };
}