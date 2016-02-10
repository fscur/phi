#include "mathUtils.h"

namespace phi
{
    float mathUtils::distance(vec3 v0, vec3 v1)
    {
        double dx = v1.x - v0.x;
        double dy = v1.y - v0.y;
        double dz = v1.z - v0.z;

        double x2 = pow(dx, 2); 
        double y2 = pow(dy, 2);
        double z2 = pow(dz, 2);

        return (float)sqrt(x2 + y2 + z2);
    }

    std::vector<vec3> mathUtils::rotateAboutAxis(std::vector<vec3>* points, vec3 origin, vec3 axis, float angle)
    {
        axis = normalize(axis);

        mat4 transform = getRotationMatrixAboutAnArbitraryAxis(origin, axis, angle);

        std::vector<vec3> transformedPoints;

        for (auto point : *points)
        {
            vec4 p4 = vec4(point.x, point.y, point.z, 1.0f);
            vec4 transformed = transform * p4;
            vec3 p3 = vec3(transformed.x, transformed.y, transformed.z);
            transformedPoints.push_back(p3);
        }

        return transformedPoints;
    }

    std::vector<vec3> mathUtils::rotateAboutAxis(std::vector<vec3>* points, vec3 axis, float angle)
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
        double y2 = pow(axis.y, 2);
        double z2 = pow(axis.z, 2);
        double l = sqrt(y2 + z2);

        double a = axis.x;
        double b = l != 0 ? axis.y / l : 1;
        double c = l != 0 ? axis.z / l : 0;

        double cossine = cos(angle);
        double sine = sin(angle);

        //translate by -origin to make axis pass through the origin
        mat4 t0 = mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -origin.x, -origin.y, -origin.z, 1);

        //rotate about X axis to align rotation axis with XY plane
        mat4 t1 = mat4(
            1, 0, 0, 0,
            0, b, -c, 0,
            0, c, b, 0,
            0, 0, 0, 1);

        //rotate about Z axis to align rotation axis with X Axis
        mat4 t2 = mat4(
            a, -l, 0, 0,
            l, a, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);

        //rotate about X with the desired rotation angle
        mat4 t3 = mat4(
            1, 0, 0, 0,
            0, cossine, sine, 0,
            0, -sine, cossine, 0,
            0, 0, 0, 1);

        //undo t2 rotation
        mat4 t4 = mat4(
            a, l, 0, 0,
            -l, a, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);

        //undo t1 rotation
        mat4 t5 = mat4(
            1, 0, 0, 0,
            0, b, c, 0,
            0, -c, b, 0,
            0, 0, 0, 1);

        //undo t0 translation
        mat4 t6 = mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            origin.x, origin.y, origin.z, 1);

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

        if (cosTheta < -1 + 0.000001f){
            // special case when vectors in opposite directions:
            // there is no "ideal" rotation axis
            // So guess one; any will do as long as it's perpendicular to start
            rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
            if (length(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
                rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

            rotationAxis = glm::normalize(rotationAxis);
            return angleAxis(glm::pi<float>(), rotationAxis);
        }

        rotationAxis = cross(start, dest);

        float s = sqrt( (1+cosTheta)*2 );
        float invs = 1 / s;

        return quat(
            s * 0.5f, 
            rotationAxis.x * invs,
            rotationAxis.y * invs,
            rotationAxis.z * invs
            );
    }

    vec3 mathUtils::multiply(const mat4 m, const vec3 v)
    {
        auto a = vec4(v.x, v.y, v.z, 1.0f);
        auto b = m * a;
        return vec3(b.x, b.y, b.z);
    };
}