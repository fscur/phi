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
        _projectionMatrix(mat4(1.0f)),
        _viewMatrix(mat4(1.0f)),
        _changedProjection(false),
        _changedView(false),
        _transformChangedEventToken(eventToken())
    {
        updateViewMatrix();
        updateProjectionMatrix();
    }

    inline camera::~camera()
    {
        if (_node)
            _node->getTransform()->getChangedEvent()->unassign(_transformChangedEventToken);
    }

    void camera::updateViewMatrix()
    {
        phi::vec3 position;
        phi::vec3 target = glm::vec3(0.0f, 0.0, 1.0f);
        if (_node != nullptr)
        {
            auto transform = _node->getTransform();
            position = transform->getPosition();
            target = position + transform->getDirection();
        }

        _viewMatrix = glm::lookAt(position, target, vec3(0.0, 1.0, 0.0));
        _changedView = false;
    }

    inline void camera::updateProjectionMatrix()
    {
        _projectionMatrix = glm::perspective(_fov, _aspect, _near, _far);
        _changedProjection = false;
    }

    inline void camera::transformChanged(transform* sender)
    {
        _changedView = true;
    }

    inline void camera::onNodeChanged(node* previousValue)
    {
        if (previousValue)
            previousValue->getTransform()->getChangedEvent()->unassign(_transformChangedEventToken);

        if (_node)
            _transformChangedEventToken = _node->getTransform()->getChangedEvent()->assign(std::bind(&camera::transformChanged, this, std::placeholders::_1));
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

        return _node->getTransform();
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
    
    vec3 camera::getWorldPositionRelativeToCamera(int mouseX, int mouseY, float z)
    {
        auto tg = tan(_fov * 0.5f) * _near;

        auto hw = _width * 0.5f;
        auto hh = _height * 0.5f;

        auto ys0 = mouseY - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = mouseX - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * _aspect;

        auto x = (xm0 / _near) * z;
        auto y = (ym0 / _near) * z;

        return vec3(x, y, z);
    }

    float camera::getWorldZRelativeToCamera(int mouseX, int mouseY, float zBufferValue)
    {
        return -_projectionMatrix[3].z / (zBufferValue * -2.0f + 1.0f - _projectionMatrix[2].z);
    }

    ray camera::screenPointToRay(float mouseX, float mouseY)
    {
        float x = (2.0f * mouseX) / _width - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / _height;

        auto ip = inverse(_projectionMatrix);
        auto iv = inverse(_viewMatrix);

        auto rayClip = vec4(x, y, -1.0f, 1.0f);

        auto rayEye = ip * rayClip;
        rayEye = vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        auto rayWorld = vec3(iv * rayEye);
        rayWorld = glm::normalize(rayWorld);

        auto origin = getTransform()->getLocalPosition();
        return ray(origin, rayWorld);
    }
}