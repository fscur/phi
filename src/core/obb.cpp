#include <precompiled.h>
#include "obb.h"
#include "mathUtils.h"

using namespace glm;

namespace phi
{
    bool obb::intersects(const obb& a, const obb& b)
    {
        // Source: http://www.geometrictools.com/Documentation/DynamicCollisionDetection.pdf

        auto At = mat3(
            a.axes[0].x, a.axes[1].x, a.axes[2].x,
            a.axes[0].y, a.axes[1].y, a.axes[2].y,
            a.axes[0].z, a.axes[1].z, a.axes[2].z);
        auto B = mat3(b.axes[0], b.axes[1], b.axes[2]);
        auto C = At * B;
        auto a0 = a.halfSizes.x;
        auto a1 = a.halfSizes.y;
        auto a2 = a.halfSizes.z;
        auto b0 = b.halfSizes.x;
        auto b1 = b.halfSizes.y;
        auto b2 = b.halfSizes.z;
        auto D = b.center - a.center;
        auto A0 = a.axes[0];
        auto A1 = a.axes[1];
        auto A2 = a.axes[2];
        auto B0 = b.axes[0];
        auto B1 = b.axes[1];
        auto B2 = b.axes[2];

        auto comp = [](float r0, float r1, float r) -> bool { return r > r0 + r1; };

        if (comp(a0, b0 * abs(C[0][0]) + b1 * abs(C[0][1]) + b2 * abs(C[0][2]), abs(dot(A0, D))))
            return false;

        if (comp(a1, b0 * abs(C[1][0]) + b1 * abs(C[1][1]) + b2 * abs(C[1][2]), abs(dot(A1, D))))
            return false;

        if (comp(a2, b0 * abs(C[1][0]) + b1 * abs(C[1][1]) + b2 * abs(C[1][2]), abs(dot(A2, D))))
            return false;

        if (comp(a0 * abs(C[0][0]) + a1 * abs(C[1][0]) + a2 * abs(C[2][0]), b0, abs(dot(B0, D))))
            return false;

        if (comp(a0 * abs(C[0][1]) + a1 * abs(C[1][1]) + a2 * abs(C[2][1]), b1, abs(dot(B1, D))))
            return false;

        if (comp(a0 * abs(C[0][2]) + a1 * abs(C[1][2]) + a2 * abs(C[2][2]), b2, abs(dot(B2, D))))
            return false;

        if (comp(a1 * abs(C[2][0]) + a2 * abs(C[1][0]), b1 * abs(C[0][2]) + b2 * abs(C[0][1]), abs(dot(C[1][0] * A2, D) - dot(C[2][0] * A1, D))))
            return false;

        if (comp(a1 * abs(C[2][1]) + a2 * abs(C[1][1]), b0 * abs(C[0][2]) + b2 * abs(C[0][0]), abs(dot(C[1][1] * A2, D) - dot(C[2][1] * A1, D))))
            return false;

        if (comp(a1 * abs(C[2][2]) + a2 * abs(C[1][2]), b0 * abs(C[0][1]) + b1 * abs(C[0][0]), abs(dot(C[1][2] * A2, D) - dot(C[2][2] * A1, D))))
            return false;

        if (comp(a0 * abs(C[2][0]) + a2 * abs(C[0][0]), b1 * abs(C[1][2]) + b2 * abs(C[1][1]), abs(dot(C[2][0] * A0, D) - dot(C[0][0] * A2, D))))
            return false;

        if (comp(a0 * abs(C[2][1]) + a2 * abs(C[0][1]), b0 * abs(C[1][2]) + b2 * abs(C[0][0]), abs(dot(C[2][1] * A0, D) - dot(C[0][1] * A2, D))))
            return false;

        if (comp(a0 * abs(C[2][2]) + a2 * abs(C[0][2]), b0 * abs(C[1][1]) + b1 * abs(C[1][0]), abs(dot(C[2][2] * A0, D) - dot(C[0][2] * A2, D))))
            return false;

        if (comp(a0 * abs(C[1][0]) + a1 * abs(C[0][0]), b1 * abs(C[2][2]) + b2 * abs(C[2][1]), abs(dot(C[0][0] * A1, D) - dot(C[1][0] * A0, D))))
            return false;

        if (comp(a0 * abs(C[1][1]) + a1 * abs(C[0][1]), b0 * abs(C[2][2]) + b2 * abs(C[2][0]), abs(dot(C[0][1] * A1, D) - dot(C[1][1] * A0, D))))
            return false;

        if (comp(a0 * abs(C[1][2]) + a1 * abs(C[0][2]), b0 * abs(C[2][1]) + b1 * abs(C[2][0]), abs(dot(C[0][2] * A1, D) - dot(C[1][2] * A0, D))))
            return false;

        return true;
    }

    obb obb::transformLocal(phi::transform* transform) const
    {
        auto transformPosition = transform->getLocalPosition();
        auto transformOrientation = transform->getLocalOrientation();
        auto transformSize = transform->getLocalSize();
        auto position = transformPosition + transformOrientation * (transformSize * center);
        auto axisX = transformOrientation * axes[0];
        auto axisY = transformOrientation * axes[1];
        auto axisZ = transformOrientation * axes[2];
        auto size = transformSize * halfSizes;

        return obb(position, axisX, axisY, axisZ, size);
    }

    obb obb::transform(phi::transform* transform) const
    {
        auto transformPosition = transform->getPosition();
        auto transformOrientation = transform->getOrientation();
        auto transformSize = transform->getSize();
        auto position = transformPosition + transformOrientation * (transformSize * center);
        auto a = transformOrientation * vec3(0.0f, 0.0f, 1.0f);
        auto axisX = transformOrientation * axes[0];
        auto axisY = transformOrientation * axes[1];
        auto axisZ = transformOrientation * axes[2];
        auto size = transformSize * halfSizes;

        return obb(position, axisX, axisY, axisZ, size);
    }

    mat4 obb::toModelMatrix() const
    {
        auto translationMatrix = glm::translate(center);
        auto rotationMatrix = mat4(
            vec4(axes[0], 0.0f),
            vec4(axes[1], 0.0f),
            vec4(axes[2], 0.0f),
            vec4(0.0f, 0.0f, 0.0f, 1.0f));
        auto scaleMatrix = glm::scale(halfSizes * 2.0f);

        return translationMatrix * rotationMatrix * scaleMatrix;
    }

    vec3 obb::getPositionAt(vec3 direction) const
    {
        auto extendedDirection = glm::normalize(direction * halfSizes);

        auto dotX = glm::dot(axes[0] * halfSizes.x, extendedDirection);
        auto dotY = glm::dot(axes[1] * halfSizes.y, extendedDirection);
        auto dotZ = glm::dot(axes[2] * halfSizes.z, extendedDirection);

        return center + extendedDirection * (
            glm::abs(dotX) +
            glm::abs(dotY) +
            glm::abs(dotZ));
    }

    vec3 obb::findClosestNormalTo(vec3 direction) const
    {
        auto highestDot = -1.0f;
        auto closestNormal = direction;

        auto checkClosest = [&highestDot, &closestNormal, direction](vec3 normal)
        {
            auto dot = glm::dot(normal, direction);
            if (dot > highestDot)
            {
                highestDot = dot;
                closestNormal = normal;
            }
        };

        checkClosest(axes[0]);
        checkClosest(-axes[0]);
        checkClosest(axes[1]);
        checkClosest(-axes[1]);
        checkClosest(axes[2]);
        checkClosest(-axes[2]);

        //checkClosest(glm::normalize(axes[0] + axes[1]));
        //checkClosest(glm::normalize(axes[0] + axes[2]));
        //checkClosest(glm::normalize(axes[0] + -axes[1]));
        //checkClosest(glm::normalize(axes[0] + -axes[2]));
        //checkClosest(glm::normalize(-axes[0] + axes[1]));
        //checkClosest(glm::normalize(-axes[0] + axes[2]));
        //checkClosest(glm::normalize(-axes[0] + -axes[1]));
        //checkClosest(glm::normalize(-axes[0] + -axes[2]));
        //checkClosest(glm::normalize(axes[2] + axes[1]));
        //checkClosest(glm::normalize(axes[2] + -axes[1]));
        //checkClosest(glm::normalize(-axes[2] + axes[1]));
        //checkClosest(glm::normalize(-axes[2] + -axes[1]));

        return closestNormal;
    }

    vector<vec3> obb::getCorners() const
    {
        auto getPoint = [=](vec3 pos) -> vec3
        {
            return center +
                axes[0] * halfSizes.x * pos.x +
                axes[1] * halfSizes.y * pos.y +
                axes[2] * halfSizes.z * pos.z;
        };

        auto corners = vector<vec3>
        {
            getPoint(vec3(-1.0f, -1.0f, -1.0f)),
            getPoint(vec3(-1.0f, -1.0f, 1.0f)),
            getPoint(vec3(-1.0f, 1.0f, 1.0f)),
            getPoint(vec3(-1.0f, 1.0f, -1.0f)),
            getPoint(vec3(1.0f, -1.0f, -1.0f)),
            getPoint(vec3(1.0f, -1.0f, 1.0f)),
            getPoint(vec3(1.0f, 1.0f, 1.0f)),
            getPoint(vec3(1.0f, 1.0f, -1.0f))
        };

        return corners;
    }

    vector<plane> obb::getPlanes() const
    {
        auto planes = vector<plane>
        {
            plane(getPositionAt(axes[0]), axes[0]),
            plane(getPositionAt(axes[1]), axes[1]),
            plane(getPositionAt(axes[2]), axes[2]),
            plane(getPositionAt(-axes[0]), -axes[0]),
            plane(getPositionAt(-axes[1]), -axes[1]),
            plane(getPositionAt(-axes[2]), -axes[2])
        };

        return planes;
    }

    vector<finitePlane> obb::getFinitePlanes() const
    {
        auto corners = getCorners();

        auto lbb = corners[0];
        auto lbf = corners[1];
        auto ltf = corners[2];
        auto ltb = corners[3];
        auto rbb = corners[4];
        auto rbf = corners[5];
        auto rtf = corners[6];
        auto rtb = corners[7];

        auto finitePlanes = vector<finitePlane>
        {
            finitePlane(lbb, lbf, ltb),
            finitePlane(rbf, rbb, rtf),
            finitePlane(lbf, rbf, ltf),
            finitePlane(rbb, lbb, rtb),
            finitePlane(ltf, rtf, ltb),
            finitePlane(lbb, rbb, lbf)
        };

        return finitePlanes;
    }

    void obb::getLimits(vec3& min, vec3& max) const
    {
        auto corners = obb::getCorners();
        min = corners[0];
        max = corners[0];

        for (size_t i = 1; i < 8; ++i)
        {
            auto corner = corners[i];
            if (corner.x < min.x)
                min.x = corner.x;
            if (corner.y < min.y)
                min.y = corner.y;
            if (corner.z < min.z)
                min.z = corner.z;

            if (corner.x > max.x)
                max.x = corner.x;
            if (corner.y > max.y)
                max.y = corner.y;
            if (corner.z > max.z)
                max.z = corner.z;
        }
    }

    void obb::set(const obb obb)
    {
        center = obb.center;
        axes[0] = obb.axes[0];
        axes[1] = obb.axes[1];
        axes[2] = obb.axes[2];
        halfSizes = obb.halfSizes;
    }
}