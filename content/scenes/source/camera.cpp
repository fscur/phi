#include "camera.h"
#include "globals.h"
#include <glm/gtx/vector_angle.hpp>

namespace phi
{
    camera::camera(
        float zNear,
        float zFar,
        float aspect,
        float fov)
        : object3D()
    {
        _frustum = new frustum(glm::vec3(), getDirection(), getUp(), zNear, zFar, aspect, fov);
        _focus = 1.0f;
        _viewMatrix = glm::lookAt(getPosition(), getPosition() + getDirection() * _focus, getUp());
    }

    void camera::setTarget(glm::vec3 value)
    {
        if (getChanged())
            update();

        auto diff = value - getPosition();
        setDirection(glm::normalize(diff));
        _focus = glm::length(diff);
        setChanged(true);
    }

    void camera::update()
    {
        auto changed = getChanged();

        object3D::update();

        if (changed)
            _viewMatrix = glm::lookAt(getPosition(), getPosition() + getDirection() * _focus, getUp());
    }

    void camera::moveTo(glm::vec3 position)
    {
        setLocalPosition(position);
    }

    void camera::zoomIn(glm::vec3 targetPos)
    {
        float dist = mathUtils::distance(targetPos, getPosition());
        glm::vec3 direction = glm::normalize(targetPos - getPosition());

        float factor = 0.3f;

        if (dist < 1.0)
            factor = 1 - (1/(pow(2.0f, dist)));

        dist *= factor;

        if (dist < _frustum->getZNear() + 0.25f)
            return;

        glm::vec3 offset = direction * dist;

        glm::vec3 position = getPosition() + offset;

        _focus = glm::max(_focus - dist, _frustum->getZNear());
        setLocalPosition(position);

        setChanged(true);
    }

    void camera::zoomOut(glm::vec3 targetPos)
    {
        float dist = mathUtils::distance(targetPos, getPosition());
        glm::vec3 direction = glm::normalize(targetPos - getPosition());

        dist *= -0.3f / (1 -0.3f);

        glm::vec3 offset = direction * dist;

        glm::vec3 position = getPosition() + offset;

        _focus -= dist;
        setLocalPosition(position);

        setChanged(true);
    }

    void camera::orbit(glm::vec3 origin, glm::vec3 axisX, glm::vec3 axisY, float angleX, float angleY)
    {
        glm::vec3 position = getPosition();
        position = mathUtils::rotateAboutAxis(position, origin, axisX, angleX);
        position = mathUtils::rotateAboutAxis(position, origin, axisY, angleY);

        glm::vec3 target = getPosition() + getDirection() * _focus;
        target = mathUtils::rotateAboutAxis(target, origin, axisX, angleX);
        target = mathUtils::rotateAboutAxis(target, origin, axisY, angleY);

        auto dir = glm::normalize(target - position);

        auto upDot = glm::dot(dir, glm::vec3(0.0f, 1.0f, 0.0f));
        if (upDot > 0.98f || upDot < -0.98f)
        {
            orbit(origin, axisX, axisY, angleX, 0.0f); // I hope this line never starts a stack overflow
            return;
        }

        auto right = glm::normalize(glm::cross(dir, glm::vec3(0.0f, -1.0f, 0.0f)));

        auto q1 = mathUtils::rotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), dir);
        auto angle = glm::orientedAngle(q1 * glm::vec3(1.0f, 0.0f, 0.0f), right, dir);
        auto q2 = glm::angleAxis(angle, dir);

        setLocalPosition(position);
        setOrientation(q2 * q1);

        setChanged(true);
    }
}