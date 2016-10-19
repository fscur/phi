#include <precompiled.h>
#include "mathUtils.h"

namespace phi
{
    float mathUtils::distance(vec3 v0, vec3 v1)
    {
        double dx = v1.x - v0.x;
        double dy = v1.y - v0.y;
        double dz = v1.z - v0.z;

        double x2 = pow(dx, 2.0f);
        double y2 = pow(dy, 2.0f);
        double z2 = pow(dz, 2.0f);

        return (float)sqrt(x2 + y2 + z2);
    }

    vector<vec3> mathUtils::rotateAboutAxis(vector<vec3>* points, vec3 origin, vec3 axis, float angle)
    {
        axis = normalize(axis);

        mat4 transform = getRotationMatrixAboutAnArbitraryAxis(origin, axis, angle);

        vector<vec3> transformedPoints;

        for (auto point : *points)
        {
            vec4 p4 = vec4(point.x, point.y, point.z, 1.0f);
            vec4 transformed = transform * p4;
            vec3 p3 = vec3(transformed.x, transformed.y, transformed.z);
            transformedPoints.push_back(p3);
        }

        return transformedPoints;
    }

    vector<vec3> mathUtils::rotateAboutAxis(vector<vec3>* points, vec3 axis, float angle)
    {
        return rotateAboutAxis(points, vec3(0.0f), axis, angle);
    }

    vec3 mathUtils::rotateAboutAxis(vec3 point, vec3 origin, vec3 axis, float angle)
    {
        axis = normalize(axis);
        mat4 transform = getRotationMatrixAboutAnArbitraryAxis(origin, axis, angle);
        vec4 p4 = vec4(point.x, point.y, point.z, 1.0f);
        vec4 transformed = transform * p4;
        return vec3(transformed.x, transformed.y, transformed.z);
    }

    vec3 mathUtils::rotateAboutAxis(vec3 point, vec3 axis, float angle)
    {
        return rotateAboutAxis(point, vec3(0.0f), axis, angle);
    }

    mat4 mathUtils::getRotationMatrixAboutAnArbitraryAxis(vec3 origin, vec3 axis, float angle)
    {
        double y2 = pow(axis.y, 2.0);
        double z2 = pow(axis.z, 2.0);
        double l = sqrt(y2 + z2);

        double a = axis.x;
        double b = l != 0.0 ? axis.y / l : 1.0;
        double c = l != 0.0 ? axis.z / l : 0.0;

        double cossine = cos(angle);
        double sine = sin(angle);

        //translate by -origin to make axis pass through the origin
        mat4 t0 = mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -origin.x, -origin.y, -origin.z, 1);

        //rotate about X axis to align rotation axis with XY plane
        mat4 t1 = mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, b, -c, 0.0f,
            0.0f, c, b, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        //rotate about Z axis to align rotation axis with X Axis
        mat4 t2 = mat4(
            a, -l, 0.0f, 0.0f,
            l, a, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        //rotate about X with the desired rotation angle
        mat4 t3 = mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cossine, sine, 0.0f,
            0.0f, -sine, cossine, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        //undo t2 rotation
        mat4 t4 = mat4(
            a, l, 0.0f, 0.0f,
            -l, a, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        //undo t1 rotation
        mat4 t5 = mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, b, c, 0.0f,
            0.0f, -c, b, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        //undo t0 translation
        mat4 t6 = mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            origin.x, origin.y, origin.z, 1.0f);

        mat4 transform;

        if (l == 0)
            transform = t6 * t3 * t0;
        else
            transform = t6 * t5 * t4 * t3 * t2 * t1 * t0;

        return transform;
    }

    quat mathUtils::rotationBetweenVectors(vec3 start, vec3 dest)
    {
        start = normalize(start);
        dest = normalize(dest);

        float cosTheta = dot(start, dest);
        vec3 rotationAxis;

        if (cosTheta < -1 + 0.000001f)
        {
            // special case when vectors in opposite directions:
            // there is no "ideal" rotation axis
            // So guess one; any will do as long as it's perpendicular to start
            rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
            if (length(rotationAxis) < 0.01f) // bad luck, they were parallel, try again!
                rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

            rotationAxis = glm::normalize(rotationAxis);
            return angleAxis(glm::pi<float>(), rotationAxis);
        }

        rotationAxis = cross(start, dest);

        float s = sqrt((1.0f + cosTheta) * 2.0f);
        float invs = 1.0f / s;

        return quat(
            s * 0.5f,
            rotationAxis.x * invs,
            rotationAxis.y * invs,
            rotationAxis.z * invs
        );
    }

    inline vec3 mathUtils::multiply(const mat4 & m, const vec3 & v)
    {
        auto a = vec4(v, 1.0f);
        return multiply(m, a);
    }

    vec3 mathUtils::multiply(const mat4& m, const vec4& v)
    {
        auto b = m * v;
        return vec3(b.x, b.y, b.z);
    }

    inline bool mathUtils::isClose(float a, float b, float error)
    {
        return abs(a - b) < error;
    }

    inline float mathUtils::truncateDecimals(float value)
    {
        return round(value * DECIMAL_TRUNCATION_INV) / DECIMAL_TRUNCATION_INV;
    }

    inline float mathUtils::truncateDecimals(float value, int decimals)
    {
        float exp = pow(10.0f, (float)decimals);
        return round(value * exp) / exp;
    }

    inline float mathUtils::isParallel(vec3 a, vec3 b)
    {
        return mathUtils::isClose(glm::dot(a, b), 1.0f);
    }

    inline vec3 mathUtils::getCentroid(const vector<vec3>& points)
    {
        auto x = 0.0f;
        auto y = 0.0f;
        auto z = 0.0f;

        for (auto& point : points)
        {
            x += point.x;
            y += point.y;
            z += point.z;
        }

        auto pointsCount = points.size();
        return vec3(x / pointsCount, y / pointsCount, z / pointsCount);
    }

    inline vec3 mathUtils::getClosestAxisTo(const vec3 & direction)
    {
        vector<vec3> axisNormals =
        {
            vec3(1.0f, 0.0f, 0.0f),
            vec3(0.0f, 1.0f, 0.0f),
            vec3(0.0f, 0.0f, 1.0f),
            vec3(-1.0f, 0.0f, 0.0f),
            vec3(0.0f, -1.0f, 0.0f),
            vec3(0.0f, 0.0f, -1.0f)
        };

        return getClosestAxisTo(direction, axisNormals);
    }

    inline vec3 mathUtils::getClosestAxisTo(const vec3 & direction, const vector<vec3>& axes)
    {
        auto maxDot = -20.0f;
        auto maxNormal = vec3();
        auto axesCount = axes.size();
        for (uint_fast8_t i = 0; i < axesCount; ++i)
        {
            auto dot = glm::dot(direction, axes[i]);
            if (dot > maxDot)
            {
                maxDot = dot;
                maxNormal = axes[i];
            }
        }

        assert(maxNormal != vec3());

        return glm::normalize(maxNormal);
    }

    inline float mathUtils::normalizeAngle(float angle)
    {
        return glm::mod(angle, PI * 2.0f * glm::sign(angle));
    }

    inline float mathUtils::counterClockwiseAngle(float angle)
    {
        if (angle >= 0.0f)
            return angle;

        return angle + PI * 2.0f;
    };
}