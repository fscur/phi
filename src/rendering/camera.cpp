#include <precompiled.h>
#include <core/transform.h>
#include <rendering/defaultFramebuffer.h>
#include "camera.h"
#include "frameBuffer.h"

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

    camera::camera(
        resolution resolution,
        transform* transform,
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
        _transform(transform)
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
        
        _viewMatrix = glm::lookAt(position, target, _transform->getUp());
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

    void camera::orbit(vec3 origin, float horizontalAngle, float verticalAngle)
    {
        auto rotation = createOrbitRotation(horizontalAngle, verticalAngle);
        _transform->rotate(rotation);

        auto position = getOrbitedPosition(origin, rotation);
        _transform->setLocalPosition(position);

        updateViewMatrix();
    }

    quat camera::createOrbitRotation(float horizontalAngle, float verticalAngle)
    {
        verticalAngle = limitOrbitVerticalRotation(verticalAngle);

        auto horizontalRotation = glm::angleAxis(horizontalAngle, vec3(0.0f, 1.0f, 0.0f));
        auto verticalRotation = glm::angleAxis(verticalAngle, _transform->getRight());
        auto rotation = verticalRotation * horizontalRotation;

        auto rightAxisFixRotation = createOrbitRightAxisFixRotation(rotation);

        return rightAxisFixRotation * rotation;
    }

    float camera::limitOrbitVerticalRotation(float angle)
    {
        float const LIMIT_ANGLE = PI / 18.0f;
        auto direction = _transform->getDirection();
        auto right = _transform->getRight();

        auto remainingAngleToUp = glm::orientedAngle(direction, vec3(0.0f, 1.0f, 0.0f), right);
        auto limitedAngleToUp = glm::min(remainingAngleToUp + LIMIT_ANGLE, 0.0f);
        angle = glm::max(angle, limitedAngleToUp);

        auto remainingAngleToDown = glm::orientedAngle(direction, vec3(0.0f, -1.0f, 0.0f), right);
        auto limitedAngleToDown = glm::max(remainingAngleToDown - LIMIT_ANGLE, 0.0f);
        angle = glm::min(angle, limitedAngleToDown);

        return angle;
    }

    quat camera::createOrbitRightAxisFixRotation(quat rotation)
    {
        auto rotatedDirection = glm::normalize(rotation * _transform->getDirection());
        auto rotatedRight = glm::normalize(rotation * _transform->getRight());

        auto fixedRight = glm::normalize(glm::cross(vec3(0.0f, 1.0f, 0.0f), rotatedDirection));
        auto rightFixRotation = mathUtils::rotationBetweenVectors(rotatedRight, fixedRight);

        return rightFixRotation * rotation;
    }

    vec3 camera::getOrbitedPosition(vec3 origin, quat rotation)
    {
        auto originToPosition = _transform->getLocalPosition() - origin;
        auto rotatedOriginToPosition = rotation * originToPosition;
        auto rotatedPosition = origin + rotatedOriginToPosition;

        return rotatedPosition;
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

    vec3 camera::screenPointToWorld(ivec2 mousePosition)
    {
        return screenPointToWorld(mousePosition.x, mousePosition.y);
    }

    vec3 camera::screenPointToWorld(int screenX, int screenY)
    {
        auto vp = _projectionMatrix * _viewMatrix;
        auto ivp = glm::inverse(vp);
        
        auto zBufferValue = defaultFramebuffer::getZBufferValue(screenX, static_cast<int>(_resolution.height) - screenY);

        float x = (2.0f * static_cast<float>(screenX)) / _resolution.width - 1.0f;
        float y = 1.0f - (2.0f * screenY) / _resolution.height;
        float z = 2.0f * zBufferValue - 1.0f;
        float w = 1.0f;

        auto pos = ivp * vec4(x, y, z, w);

        pos.w = 1.0f / pos.w;

        pos.x *= pos.w;
        pos.y *= pos.w;
        pos.z *= pos.w;

        return vec3(pos.x, pos.y, pos.z);
    }

    ray camera::screenPointToRay(int mouseX, int mouseY)
    {
        float x = (2.0f * static_cast<float>(mouseX)) / _resolution.width - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / _resolution.height;

        auto ip = glm::inverse(_projectionMatrix);
        auto iv = glm::inverse(_viewMatrix);

        auto rayClip = vec4(x, y, -1.0f, 1.0f);

        auto rayEye = ip * rayClip;
        rayEye = vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        auto direction = vec3(iv * rayEye);
        direction = glm::normalize(direction);

        auto origin = _transform->getLocalPosition();
        return ray(origin, direction);
    }

    vec2 camera::worldPointToScreen(vec3 worldPosition)
    {
        auto vp = _projectionMatrix * _viewMatrix;
        auto screenVec4 = vp * vec4(worldPosition, 1.0);
        auto unitScreenPosition = vec2(screenVec4.x / screenVec4.w, screenVec4.y / screenVec4.w);
        auto screenPosition = ((unitScreenPosition + 1.0f) / 2.0f) * _resolution.toVec2();
        return screenPosition;
    }

    float camera::zBufferToDepth(float zBufferValue)
    {
        return -_projectionMatrix[3].z / (zBufferValue * -2.0f + 1.0f - _projectionMatrix[2].z);
    }

    vec3 camera::castRayToPlane(int mouseX, int mouseY, plane plane)
    {
        auto ray = screenPointToRay(mouseX, mouseY);
        float t;
        ray.intersects(plane, t);
        return ray.getOrigin() + ray.getDirection() * t;
    }
}