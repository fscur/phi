#pragma once
#include <phi.h>
#include "coreApi.h"

namespace phi
{
    class CORE_API mathUtils
    {
    public:

        static float distance(vec3 v0, vec3 v1);
        static vector<vec3> rotateAboutAxis(vector<vec3>* points, vec3 origin, vec3 axis, float angle);
        static vector<vec3> rotateAboutAxis(vector<vec3>* points, vec3 axis, float angle);
        static vec3 rotateAboutAxis(vec3 point, vec3 origin, vec3 axis, float angle);
        static vec3 rotateAboutAxis(vec3 point, vec3 axis, float angle);
        static mat4 getRotationMatrixAboutAnArbitraryAxis(vec3 origin, vec3 axis, float angle);
        static quat rotationBetweenVectors(vec3 start, vec3 dest);

        static vec3 multiply(const mat4& m, const vec3& v);

        static bool isClose(float a, float b, float error = DECIMAL_TRUNCATION)
        {
            return abs(a - b) < error;
        }

        static float truncateDecimals(float value)
        {
            return round(value * DECIMAL_TRUNCATION_INV) / DECIMAL_TRUNCATION_INV;
        }

        static float truncateDecimals(float value, int decimals)
        {
            float exp = pow(10.0f, (float)decimals);
            return round(value * exp) / exp;
        }

        static float isParallel(vec3 a, vec3 b)
        {
            return mathUtils::isClose(glm::dot(a, b), 1.0f);
        }
    };
}