#include "phi/core/mathUtils.h"

namespace phi
{
    float mathUtils::distance(glm::vec3 v0, glm::vec3 v1)
    {
        double dx = v1.x - v0.x;
        double dy = v1.y - v0.y;
        double dz = v1.z - v0.z;

        double x2 = pow(dx, 2); 
        double y2 = pow(dy, 2);
        double z2 = pow(dz, 2);

        return (float)glm::sqrt(x2 + y2 + z2);
    }

    std::list<glm::vec3> mathUtils::rotateAboutAxis(std::list<glm::vec3>* points, glm::vec3 origin, glm::vec3 axis, float angle)
    {
        axis = glm::normalize(axis);

        glm::mat4 transform = getRotationMatrixAboutAnArbitraryAxis(origin, axis, angle);

        std::list<glm::vec3> transformedPoints;

        std::list<glm::vec3>::iterator i;  

        for (i=points->begin(); i != points->end(); ++i)
        {
            glm::vec3 point = *i;
            glm::vec4 p4 = glm::vec4(point.x, point.y, point.z, 1.0f);
            glm::vec4 transformed = transform * p4;
            glm::vec3 p3 = glm::vec3(transformed.x, transformed.y, transformed.z);
            transformedPoints.push_back(p3);
        }

        return transformedPoints;
    }

    std::list<glm::vec3> mathUtils::rotateAboutAxis(std::list<glm::vec3>* points, glm::vec3 axis, float angle)
    {
        return rotateAboutAxis(points, glm::vec3(0.0f), axis, angle);
    }

    glm::vec3 mathUtils::rotateAboutAxis(glm::vec3 point, glm::vec3 origin, glm::vec3 axis, float angle)
    {
        axis = glm::normalize(axis);
        glm::mat4 transform = getRotationMatrixAboutAnArbitraryAxis(origin, axis, angle);
        glm::vec4 p4 = glm::vec4(point.x, point.y, point.z, 1.0f);
        glm::vec4 transformed = transform * p4;
        return glm::vec3(transformed.x, transformed.y, transformed.z);
    }

    glm::vec3 mathUtils::rotateAboutAxis(glm::vec3 point, glm::vec3 axis, float angle)
    {
        return rotateAboutAxis(point, glm::vec3(0.0f), axis, angle);
    }

    glm::mat4 mathUtils::getRotationMatrixAboutAnArbitraryAxis(glm::vec3 origin, glm::vec3 axis, float angle)
    {
        double y2 = pow(axis.y, 2);
        double z2 = pow(axis.z, 2);
        double l = glm::sqrt(y2 + z2);

        double a = axis.x;
        double b = l != 0 ? axis.y / l : 1;
        double c = l != 0 ? axis.z / l : 0;

        double cossine = cos(angle);
        double sine = sin(angle);

        //translate by -origin to make axis pass through the origin
        glm::mat4 t0 = glm::mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -origin.x, -origin.y, -origin.z, 1);

        //rotate about X axis to align rotation axis with XY plane
        glm::mat4 t1 = glm::mat4(
            1, 0, 0, 0,
            0, b, -c, 0,
            0, c, b, 0,
            0, 0, 0, 1);

        //rotate about Z axis to align rotation axis with X Axis
        glm::mat4 t2 = glm::mat4(
            a, -l, 0, 0,
            l, a, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);

        //rotate about X with the desired rotation angle
        glm::mat4 t3 = glm::mat4(
            1, 0, 0, 0,
            0, cossine, sine, 0,
            0, -sine, cossine, 0,
            0, 0, 0, 1);

        //undo t2 rotation
        glm::mat4 t4 = glm::mat4(
            a, l, 0, 0,
            -l, a, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);

        //undo t1 rotation
        glm::mat4 t5 = glm::mat4(
            1, 0, 0, 0,
            0, b, c, 0,
            0, -c, b, 0,
            0, 0, 0, 1);

        //undo t0 translation
        glm::mat4 t6 = glm::mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            origin.x, origin.y, origin.z, 1);

        glm::mat4 transform;

        if (l == 0)
            transform = t6 * t3 * t0;
        else
            transform = t6 * t5 * t4 * t3 * t2 * t1 * t0;

        return transform;
    }

    glm::quat mathUtils::rotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
    {
        start = glm::normalize(start);
        dest = glm::normalize(dest);

        float cosTheta = glm::dot(start, dest);
        glm::vec3 rotationAxis;

        if (cosTheta < -1 + 0.000001f){
            // special case when vectors in opposite directions:
            // there is no "ideal" rotation axis
            // So guess one; any will do as long as it's perpendicular to start
            rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
            if (glm::length(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
                rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

            rotationAxis = glm::normalize(rotationAxis);
            return glm::angleAxis(glm::pi<float>(), rotationAxis);
        }

        rotationAxis = glm::cross(start, dest);

        float s = sqrt( (1+cosTheta)*2 );
        float invs = 1 / s;

        return glm::quat(
            s * 0.5f, 
            rotationAxis.x * invs,
            rotationAxis.y * invs,
            rotationAxis.z * invs
            );
    }

    glm::vec3 mathUtils::multiply(const glm::mat4 mat, const glm::vec3 vec)
    {
        auto vec4 = glm::vec4(vec.x, vec.y, vec.z, 1.0f);
        vec4 = mat * vec4;
        return glm::vec3(vec4.x, vec4.y, vec4.z);
    };
}