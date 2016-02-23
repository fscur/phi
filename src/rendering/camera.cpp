#include "camera.h"

#include <glm/gtx/vector_angle.hpp>

namespace phi
{
    camera::camera(float nearDistance, float farDistance, vec2 resolution, float fov) : 
        object3D("camera", object3D::objectType::CAMERA),
        _frustum(new frustum(vec3(), getDirection(), getUp(), nearDistance, farDistance, resolution, fov)),
        _focus(1.0f),
        _viewMatrix(lookAt(getPosition(), getPosition() + getDirection() * _focus, getUp()))
    {
    }

    void camera::setTarget(vec3 value)
    {
        //if (getChanged())
            update();

        auto diff = value - _position;
        setDirection(glm::normalize(diff));
        _focus = glm::length(diff);
        setChanged();
    }

    void camera::update()
    {
        auto changed = getChanged();

        object3D::update();

        //if (changed)
        //{
            auto target = _position + _direction * _focus;
            _viewMatrix = lookAt(_position, target, vec3(0.0, 1.0, 0.0));
            _frustum->update();
        //}
    }

    void camera::moveTo(vec3 position)
    {
        setLocalPosition(position);
    }

    void camera::zoomIn(vec3 targetPos)
    {
        auto zFar = _frustum->getZFar();
        auto zNear = _frustum->getZNear();
        float dist = mathUtils::distance(targetPos, getPosition()) - zNear;
        vec3 direction = normalize(targetPos - getPosition());

        float factor = 0.5f;

        if (dist < 1.0)
            factor = 1 - (1 / (pow(2.0f, dist)));

        dist *= factor;

        vec3 offset = direction * dist;

        vec3 position = getPosition() + offset;

        _focus = glm::max(_focus - dist, _frustum->getZNear());
        setLocalPosition(position);
        setChanged();

        if (dist - zNear < 0.2f)
        {
            _frustum->setZFar(100.01f);
            _frustum->setZNear(0.01f);
        }
    }

    void camera::zoomOut(vec3 targetPos)
    {
        auto zFar = _frustum->getZFar();
        auto zNear = _frustum->getZNear();
        float dist = mathUtils::distance(targetPos, getPosition());
        vec3 direction = normalize(targetPos - getPosition());

        dist *= -0.5f / (1 - 0.5f);

        vec3 offset = direction * dist;

        vec3 position = getPosition() + offset;

        _focus -= dist;
        setLocalPosition(position);

        if (dist - zNear > 0.2f)
        {
            _frustum->setZFar(1000.0f);
            _frustum->setZNear(0.1f);
        }

        setChanged();
    }

    void camera::orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY)
    {
        vec3 position = getPosition();
        position = mathUtils::rotateAboutAxis(position, origin, axisX, angleX);
        position = mathUtils::rotateAboutAxis(position, origin, axisY, angleY);

        vec3 target = getPosition() + getDirection() * _focus;
        target = mathUtils::rotateAboutAxis(target, origin, axisX, angleX);
        target = mathUtils::rotateAboutAxis(target, origin, axisY, angleY);

        auto dir = normalize(target - position);

        auto upDot = dot(dir, vec3(0.0f, 1.0f, 0.0f));
        if (upDot > 0.98f || upDot < -0.98f)
        {
            orbit(origin, axisX, axisY, angleX, 0.0f); // I hope this line never starts a stack overflow
            return;
        }

        auto right = normalize(cross(dir, vec3(0.0f, -1.0f, 0.0f)));

        auto q1 = mathUtils::rotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), dir);
        auto angle = orientedAngle(q1 * vec3(1.0f, 0.0f, 0.0f), right, dir);
        auto q2 = angleAxis(angle, dir);

        setLocalPosition(position);
        setOrientation(q2 * q1);

        setChanged();
    }
}