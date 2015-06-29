#include "camera.h"
#include "globals.h"

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
    }

    void camera::setTarget(glm::vec3 value)
    {
        auto diff = value - getPosition();
        setDirection(glm::normalize(diff));
        _focus = glm::length(diff);
        _changed = true;
    }

    void camera::update()
    {
        auto changed = _changed;

        object3D::update();

        if (changed)
            _viewMatrix = glm::lookAt(getPosition(), getPosition() + getDirection() * _focus, getUp());
    }

    void camera::moveTo(glm::vec3 position)
    {
        setPosition(position);
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
        setPosition(position);

        _changed = true;
    }

    void camera::zoomOut(glm::vec3 targetPos)
    {
        float dist = mathUtils::distance(targetPos, getPosition());
        glm::vec3 direction = glm::normalize(targetPos - getPosition());

        dist *= -0.3f / (1 -0.3f);

        glm::vec3 offset = direction * dist;

        glm::vec3 position = getPosition() + offset;

        _focus -= dist;
        setPosition(position);
        LOG(std::to_string(position.x));

        _changed = true;
    }

    void camera::orbit(glm::vec3 origin, glm::vec3 axis, float angle)
    {  
        glm::vec3 position = mathUtils::rotateAboutAxis(getPosition(), origin, axis, angle);
        glm::vec3 target = mathUtils::rotateAboutAxis(getPosition() + getDirection() * _focus, origin, axis, angle);

        setPosition(position);
        setDirection(glm::normalize(target - getPosition()));

        _changed = true;
    }

    void camera::orbit2(glm::vec3 origin, glm::vec3 axisX, glm::vec3 axisY, float angleX, float angleY)
    {  
        glm::vec3 position = mathUtils::rotateAboutAxis(getPosition(), origin, axisX, angleX);
        position = mathUtils::rotateAboutAxis(position, origin, axisY, angleY);

        glm::vec3 target = mathUtils::rotateAboutAxis(getPosition() + getDirection() * _focus, origin, axisX, angleX);
        target = mathUtils::rotateAboutAxis(target, origin, axisY, angleY);

        setPosition(position);
        setDirection(glm::normalize(target - getPosition()));

        _changed = true;
    }
}