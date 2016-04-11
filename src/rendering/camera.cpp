#include <precompiled.h>
#include "camera.h"
#include <core\transform.h>
#include <core\node.h>

namespace phi
{
    camera::camera(
        std::string name, 
        float width, 
        float height, 
        float near, 
        float far, 
        float fov) : 
        component(component::componentType::CAMERA, name),
        _width(width),
        _height(height),
        _near(near),
        _far(far),
        _fov(fov),
        _aspect(_width / _height),
        _focus(1.0f),
        _projectionMatrix(mat4(1.0f)),
        _viewMatrix(mat4(1.0f)),
        _changedProjection(false),
        _changedView(false),
        _transformChangedEventToken(eventToken())
    {
        updateViewMatrix();
        updateProjectionMatrix();
    }

    void camera::updateViewMatrix()
    {
        auto transform = phi::transform();
        if (_node != nullptr)
            transform = _node->getTransform();

        auto position = transform.getPosition();
        auto target = position + transform.getDirection() * _focus;
        _viewMatrix = glm::lookAt(position, target, vec3(0.0, 1.0, 0.0));
        _changedView = false;
    }

    inline void camera::updateProjectionMatrix()
    {
        _projectionMatrix = glm::perspective(_fov, _aspect, _near, _far);
        _changedProjection = false;
    }

    inline void camera::transformChanged()
    {
        _changedView = true;
    }

    inline void camera::onNodeChanged(node* previousValue)
    {
        if (previousValue)
            previousValue->getTransform().getChangedEvent()->unassign(_transformChangedEventToken);

        if (_node)
            _transformChangedEventToken = _node->getTransform().getChangedEvent()->assign(std::bind(&camera::transformChanged, this));
    }

    inline mat4 camera::getViewMatrix()
    {
        if (_changedView)
            updateViewMatrix();

        return _viewMatrix;
    }

    inline mat4 camera::getProjectionMatrix()
    {
        if (_changedProjection)
            updateProjectionMatrix();

        return _projectionMatrix;
    }

    inline transform* camera::getTransform()
    {
        if (_node == nullptr)
            return nullptr;

        return &_node->getTransform();
    }

    inline void camera::setWidth(float value)
    {
        _width = value;
        _aspect = _width / _height;
        _changedProjection = true;
    }

    inline void camera::setHeight(float value)
    {
        _height = value;
        _aspect = _width / _height;
        _changedProjection = true;
    }

    inline void camera::setFov(float value) 
    { 
        _fov = value; 
        _changedProjection = true; 
    }

    inline void camera::setNear(float value) 
    { 
        _near = value; 
        _changedProjection = true; 
    }

    inline void camera::setFar(float value) 
    { 
        _far = value; 
        _changedProjection = true; 
    }

    inline void camera::moveTo(vec3 position)
    {
        getTransform()->setLocalPosition(position);
    }

    void camera::zoom(vec3 offset)
    {
        auto transform = getTransform();
        auto position = transform->getPosition();
        vec3 newPosition = position + offset;

        transform->setLocalPosition(newPosition);

        _changedView = true;
    }

    void camera::orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY)
    {
        auto transform = getTransform();
        auto position = transform->getPosition();
        position = mathUtils::rotateAboutAxis(position, origin, axisX, angleX);
        position = mathUtils::rotateAboutAxis(position, origin, axisY, angleY);

        auto target = transform->getPosition() + transform->getDirection();
        target = mathUtils::rotateAboutAxis(target, origin, axisX, angleX);
        target = mathUtils::rotateAboutAxis(target, origin, axisY, angleY);

        auto dir = normalize(target - position);

        auto upDot = dot(dir, vec3(0.0f, 1.0f, 0.0f));
        if (upDot > 0.98f || upDot < -0.98f)
        {
            orbit(origin, axisX, axisY, angleX, 0.0f); // I hope this line never starts a stack overflow... well, it did!
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
}