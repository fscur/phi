#pragma once
#include <phi.h>
#include "core.h"

namespace phi
{
    class mathUtils
    {
    public:

        static CORE_API float round(float value, int decimals)
        {
            float exp = pow(10.0f, (float)decimals);
            value *= exp;

            int istack = (int)floor(value);
            float out = value - istack;

            if (out < 0.5)
            {
                value = floor(value);
                value /= exp;
                return value;
            }

            if (out > 0.4) 
            {
                value = ceil(value);
                value /= exp;
                return value;
            }

            return value;
        }

        static CORE_API float distance(vec3 v0, vec3 v1);
        static CORE_API vector<vec3> rotateAboutAxis(vector<vec3>* points, vec3 origin, vec3 axis, float angle);
        static CORE_API vector<vec3> rotateAboutAxis(vector<vec3>* points, vec3 axis, float angle);
        static CORE_API vec3 rotateAboutAxis(vec3 point, vec3 origin, vec3 axis, float angle);
        static CORE_API vec3 rotateAboutAxis(vec3 point, vec3 axis, float angle);
        static CORE_API mat4 getRotationMatrixAboutAnArbitraryAxis(vec3 origin, vec3 axis, float angle);
        static CORE_API quat rotationBetweenVectors(vec3 start, vec3 dest);

        static CORE_API vec3 multiply(const mat4 mat, const vec3 vec);

        static CORE_API bool isClose(float a, float b, float error = 1e-3)
        {
            return abs(a - b) < error;
        }
    };
}
