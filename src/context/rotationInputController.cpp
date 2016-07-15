#include <precompiled.h>

#include <core\time.h>

#include <rendering\framebuffer.h>

#include "rotationInputController.h"

namespace phi
{
    rotationInputController::rotationInputController(camera* camera) :
        inputController::inputController(),
        _camera(camera),
        _rotating(false),
        _doingInertia(false),
        _delta(phi::vec2()),
        _targetPos(phi::vec3()),
        _lastMouseMoveTime(0.0),
        _inertiaTime(0.0),
        _inertiaLastPercent(0.0f),
        _lastMousePosX(0),
        _lastMousePosY(0)
    {
    }

    void rotationInputController::cancelRotation()
    {
        _delta = phi::vec2();
    }

    void rotationInputController::onMouseDown(mouseEventArgs* e)
    {
        if (!e->rightButtonPressed)
            return;

        cancelRotation();

        auto y = static_cast<int>(_camera->getResolution().height) - e->y;
        auto zBufferValue = framebuffer::defaultFramebuffer->getZBufferValue(e->x, y);

        if (zBufferValue == 1.0f)
            _targetPos = glm::vec3();
        else
        {
            auto worldZ = _camera->getWorldZRelativeToCamera(zBufferValue);
            auto worldPosition = _camera->getWorldPositionRelativeToCamera(e->x, e->y, worldZ);

            auto transform = _camera->getTransform();
            auto camPos = transform->getPosition();
            auto camDir = transform->getDirection();
            auto camRight = transform->getRight();
            auto camUp = transform->getUp();

            _targetPos =
                camPos +
                (-camRight * worldPosition.x) +
                (camUp * worldPosition.y) +
                (camDir * worldPosition.z);
        }

        _lastMouseMoveTime = 0.0;
        _rotating = true;
        _lastMousePosX = e->x;
        _lastMousePosY = e->y;
    }

    void rotationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_rotating)
            return;

        auto resolution = _camera->getResolution();
        auto w = resolution.width;
        auto h = resolution.height;

        auto dx = e->x - _lastMousePosX;
        auto dy = e->y - _lastMousePosY;

        auto x = (dx / w) * 3.0f * phi::PI;
        auto y = (dy / h) * 3.0f * phi::PI;

        auto delta = phi::vec2(x, y);

        auto lastRemainingRotation = (1.0f - _inertiaLastPercent) * glm::length(_delta);
        _delta = glm::normalize(delta) * (lastRemainingRotation + glm::length(delta));

        _lastMouseMoveTime = phi::time::totalSeconds * 1000.0;
        _inertiaTime = 0.0;
        _doingInertia = true;
        _inertiaLastPercent = 0.0f;

        _lastMousePosX = e->x;
        _lastMousePosY = e->y;
    }

    void rotationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->rightButtonPressed)
            return;

        _rotating = false;

        if (glm::length(_delta) == 0.0f)
            return;

        auto nowMilliseconds = phi::time::totalSeconds * 1000.0;
        auto deltaTime = static_cast<float>(glm::max(10.0, nowMilliseconds - _lastMouseMoveTime));
        auto lastRemainingRotation = (1.0f - _inertiaLastPercent) * glm::length(_delta);
        _delta = glm::normalize(_delta) * (lastRemainingRotation + lastRemainingRotation * (1.0f - glm::min(1.0f, glm::max(0.0f, deltaTime / 100.0f))));
    }

    void rotationInputController::update()
    {
        if (!_doingInertia)
            return;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0f;
        _inertiaTime += deltaMilliseconds;
        auto desFactor = _rotating ? 100.0f : 325.0f;
        auto percent = static_cast<float>(-glm::exp(_inertiaTime / -desFactor) + 1.0);
        auto delta = _delta * (percent - _inertiaLastPercent);
        _inertiaLastPercent = percent;

        _camera->orbit(_targetPos, phi::vec3(0.0f, 1.0f, 0.0f), -_camera->getTransform()->getRight(), -delta.x, -delta.y);
        if (percent >= 1.0f)
            _doingInertia = false;
    }
}