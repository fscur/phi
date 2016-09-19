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
        static vec3 multiply(const mat4& m, const vec4& v);

        static bool isClose(float a, float b, float error = DECIMAL_TRUNCATION);
        static float truncateDecimals(float value);
        static float truncateDecimals(float value, int decimals);
        static float isParallel(vec3 a, vec3 b);
        static vec3 getCentroid(vector<vec3> points);
        static vec3 getClosestAxisTo(const vec3& direction);
    };
}