#include <precompiled.h>
#include "camera.h"
#include <core\transform.h>
#include <core\node.h>

namespace phi
{
    camera::camera(std::string name, float nearDistance, float farDistance, sizeui resolution, float fov) : 
        component(component::componentType::CAMERA, name),
        _near(nearDistance),
        _far(farDistance),
        _resolution(resolution),
        _fov(fov),
        _focus(1.0f),
        _aspect(static_cast<float>(resolution.w) / static_cast<float>(resolution.h)),
        _changedView(false),
        _changedProjection(false)
    {
        updateViewMatrix();
        updateProjectionMatrix();
    }

    mat4 camera::getViewMatrix()
    {
        if (_changedView)
            updateViewMatrix();

        return _viewMatrix;
    }

    mat4 camera::getProjectionMatrix()
    {
        if (_changedProjection)
            updateProjectionMatrix();

        return _projectionMatrix;
    }

    transform* camera::getTransform()
    {
        if (_node == nullptr)
            return nullptr;

        return &(_node->getTransform());
    }

    void camera::setResolution(sizeui value)
    {
        _resolution = value;
        _aspect = static_cast<float>(_resolution.w) / static_cast<float>(_resolution.h);
        _changedProjection = true;
    }

    void camera::updateViewMatrix()
    {
        auto transform = getTransform();
        if (transform == nullptr)
            transform = new phi::transform();

        auto position = transform->getPosition();
        auto target = position + transform->getDirection() * _focus;
        _viewMatrix = glm::lookAt(position, target, vec3(0.0, 1.0, 0.0));
        _changedView = false;
    }

    void camera::updateProjectionMatrix()
    {
        _projectionMatrix = glm::perspective(_fov, _aspect, _near, _far);
        _changedProjection = false;
    }

    void camera::moveTo(vec3 position)
    {
        getTransform()->setLocalPosition(position);
    }

    void camera::zoomIn(vec3 targetPos)
    {
        auto transform = getTransform();
        auto position = transform->getPosition();
        auto dist = mathUtils::distance(targetPos, position) - _near;
        vec3 direction = normalize(targetPos - position);

        auto factor = 0.5f;

        if (dist < 1.0)
            factor = 1 - (1 / (pow(2.0f, dist)));

        dist *= factor;

        auto offset = direction * dist;

        auto newPosition = position + offset;

        _focus = glm::max(_focus - dist, _near);
        transform->setLocalPosition(newPosition);

        if (dist - _near < 0.2f)
        {
            _far = 100.01f;
            _near = 0.01f;
            _changedProjection = true;
        }

        _changedView = true;
    }

    void camera::zoomOut(vec3 targetPos)
    {
        auto transform = getTransform();
        auto position = transform->getPosition();
        auto dist = mathUtils::distance(targetPos, position);
        vec3 direction = normalize(targetPos - position);

        dist *= -0.5f / (1 - 0.5f);

        vec3 offset = direction * dist;

        vec3 newPosition = position + offset;

        _focus -= dist;
        transform->setLocalPosition(newPosition);

        if (dist - _near > 0.2f)
        {
            _far = 1000.0f;
            _near = 0.1f;
            _changedProjection = true;
        }

        _changedView = true;
    }

    void camera::orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY)
    {
        auto transform = getTransform();
        auto position = transform->getPosition();
        position = mathUtils::rotateAboutAxis(position, origin, axisX, angleX);
        position = mathUtils::rotateAboutAxis(position, origin, axisY, angleY);

        auto target = transform->getPosition() + transform->getDirection() * _focus;
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

        transform->setLocalPosition(position);
        transform->setLocalOrientation(q2 * q1);

        _changedView = true;
    }

    void camera::update()
    {
        // TODO: when the new event system is done, change this to update only when the transform changes
        updateViewMatrix();
    }
}