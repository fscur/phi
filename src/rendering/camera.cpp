#include <precompiled.h>
#include "camera.h"
#include <core\transform.h>

namespace phi
{
    camera::camera(
        resolution resolution,
        float near,
        float far,
        float fov) :
        _resolution(resolution),
        _near(near),
        _far(far),
        _fov(fov),
        _halfFovTangent(glm::tan(fov * 0.5f)),
        _projectionMatrix(mat4(1.0f)),
        _viewMatrix(mat4(1.0f)),
        _transform(new transform())
    {
        updateViewMatrix();
        updateProjectionMatrix();
        _transformChangedEventToken = _transform->getChangedEvent()->assign(std::bind(&camera::transformChanged, this, std::placeholders::_1));
    }

    inline camera::~camera()
    {
        _transform->getChangedEvent()->unassign(_transformChangedEventToken);
        safeDelete(_transform);
    }

    void camera::updateViewMatrix()
    {
        auto position = _transform->getPosition();
        auto target = position + _transform->getDirection();

        _viewMatrix = glm::lookAt(position, target, vec3(0.0, 1.0, 0.0));
    }

    inline void camera::updateProjectionMatrix()
    {
        _projectionMatrix = glm::perspective(_fov, _resolution.getAspect(), _near, _far);
    }

    inline void camera::transformChanged(transform* sender)
    {
        updateViewMatrix();
    }

    inline mat4 camera::getViewMatrix()
    {
        return _viewMatrix;
    }

    inline mat4 camera::getProjectionMatrix()
    {
        return _projectionMatrix;
    }

    inline mat4 camera::getInverseProjectionMatrix()
    {
        return glm::inverse(_projectionMatrix);
    }

    inline mat4 camera::getViewProjectionMatrix()
    {
        return _viewMatrix * _projectionMatrix;
    }

    void camera::setResolution(const resolution & resolution)
    {
        _resolution = resolution;
        updateProjectionMatrix();
    }

    inline void camera::setFov(float value)
    {
        _fov = value;
        _halfFovTangent = glm::tan(_fov * 0.5f);
        updateProjectionMatrix();
    }

    inline void camera::setNear(float value)
    {
        _near = value;
        updateProjectionMatrix();
    }

    inline void camera::setFar(float value)
    {
        _far = value;
        updateProjectionMatrix();
    }

    inline void camera::moveTo(vec3 position)
    {
        _transform->setLocalPosition(position);
    }

    void camera::zoom(vec3 offset)
    {
        auto position = _transform->getPosition();
        vec3 newPosition = position + offset;

        _transform->setLocalPosition(newPosition);

        updateViewMatrix();
    }

    void camera::orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY)
    {
        auto position = _transform->getPosition();
        position = mathUtils::rotateAboutAxis(position, origin, axisX, angleX);
        position = mathUtils::rotateAboutAxis(position, origin, axisY, angleY);

        auto target = _transform->getPosition() + _transform->getDirection();
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

        _transform->setLocalPosition(position);
        _transform->setLocalOrientation(q2 * q1);

        updateViewMatrix();
    }

    vec3 camera::screenPointToView(int mouseX, int mouseY, float depth)
    {
        auto tg = _halfFovTangent * _near;

        auto hw = _resolution.width * 0.5f;
        auto hh = _resolution.height * 0.5f;

        auto ys0 = mouseY - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = mouseX - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * _resolution.getAspect();

        auto x = (xm0 / _near) * depth;
        auto y = (ym0 / _near) * depth;

        return vec3(x, y, depth);
    }

    float camera::zBufferToDepth(float zBufferValue)
    {
        return -_projectionMatrix[3].z / (zBufferValue * -2.0f + 1.0f - _projectionMatrix[2].z);
    }

    ray camera::screenPointToRay(float mouseX, float mouseY)
    {
        float x = (2.0f * mouseX) / _resolution.width - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / _resolution.height;

        auto ip = inverse(_projectionMatrix);
        auto iv = inverse(_viewMatrix);

        auto rayClip = vec4(x, y, -1.0f, 1.0f);

        auto rayEye = ip * rayClip;
        rayEye = vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        auto rayWorld = vec3(iv * rayEye);
        rayWorld = glm::normalize(rayWorld);

        auto origin = _transform->getLocalPosition();
        return ray(origin, rayWorld);
    }
}