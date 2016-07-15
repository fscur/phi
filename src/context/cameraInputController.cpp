#include <precompiled.h>
#include "cameraInputController.h"

#include <core\input.h>
#include <core\time.h>
#include <core\mesh.h>
#include <core\ray.h>
#include <core\multiCommand.h>

#include <rendering\framebuffer.h>

#include <application\application.h>

namespace phi
{
    cameraInputController::cameraInputController(camera* camera) :
        _camera(camera),
        _isCtrlPressed(false),
        _mousePosX(0),
        _mousePosY(0),
        _lastMousePosX(0),
        _lastMousePosY(0),
        _rotating(false),
        _rotationDoingInertia(false),
        _rotationDelta(phi::vec2()),
        _rotationTargetPos(phi::vec3()),
        _rotationLastMouseMoveTime(0.0),
        _rotationInertiaTime(0.0),
        _rotationInertiaLastPercent(0.0f),
        _panning(false),
        _panDoingInertia(false),
        _panEyeZ(0.0f),
        _panCameraPos(phi::vec3()),
        _panCameraRight(phi::vec3()),
        _panCameraUp(phi::vec3()),
        _panDelta(phi::vec3()),
        _panTargetCameraPos(phi::vec3()),
        _panLastMouseMoveTime(0.0),
        _panInertiaTime(0.0)
    {
    }

    void cameraInputController::onKeyDown(keyboardEventArgs * e)
    {
        _isCtrlPressed = e->key == PHIK_CTRL;
    }

    void cameraInputController::onKeyUp(keyboardEventArgs * e)
    {
        if (e->key == PHIK_CTRL)
            _isCtrlPressed = false;
    }

    void cameraInputController::onMouseDown(phi::mouseEventArgs* e)
    {
        if (e->middleButtonPressed && !_rotating)
            panMouseDown(e->x, e->y);

        if (e->rightButtonPressed && !_panning)
            rotationMouseDown(e->x, e->y);
    }

    void cameraInputController::onMouseMove(phi::mouseEventArgs* e)
    {
        _lastMousePosX = _mousePosX;
        _lastMousePosY = _mousePosY;
        _mousePosX = e->x;
        _mousePosY = e->y;

        if (_panning)
            panMouseMove();
        if (_rotating)
            rotationMouseMove();
    }

    void cameraInputController::onMouseUp(phi::mouseEventArgs* e)
    {
        if (e->middleButtonPressed)
            panMouseUp();

        if (e->rightButtonPressed)
            rotationMouseUp();
    }

    void cameraInputController::onMouseWheel(phi::mouseEventArgs* e)
    {
    }

    void cameraInputController::update()
    {
        rotationUpdate();
        panUpdate();

        _lastMousePosX = _mousePosX;
        _lastMousePosY = _mousePosY;
    }

    void cameraInputController::panMouseDown(int mouseX, int mouseY)
    {
        rotationCancel();
        panCancel();

        auto y = static_cast<int>(_camera->getResolution().height) - mouseY;
        auto zBufferValue = framebuffer::defaultFramebuffer->getZBufferValue(mouseX, y);

        if (zBufferValue == 1.0f)
            _panEyeZ = 20.0f;
        else
            _panEyeZ = _camera->getWorldZRelativeToCamera(zBufferValue);

        auto cameraTransform = _camera->getTransform();
        _panCameraPos = _panTargetCameraPos = cameraTransform->getPosition();
        _panCameraRight = cameraTransform->getRight();
        _panCameraUp = cameraTransform->getUp();

        _panDelta = vec3();
        _panning = true;
    }

    void cameraInputController::panMouseMove()
    {
        auto resolution = _camera->getResolution();
        auto zNear = _camera->getNear();
        auto aspect = resolution.getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = resolution.width;
        auto h = resolution.height;

        auto hh = h * 0.5f;
        auto hw = w * 0.5f;

        auto ys0 = (float)_lastMousePosY - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = (float)_lastMousePosX - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * aspect;

        auto ys1 = (float)_mousePosY - hh;
        auto yp1 = ys1 / hh;
        auto ym1 = -(yp1 * tg);

        auto xs1 = (float)_mousePosX - hw;
        auto xp1 = xs1 / hw;
        auto xm1 = xp1 * tg * aspect;

        auto xDiff = xm1 - xm0;
        auto yDiff = ym1 - ym0;

        float x = xDiff * (_panEyeZ / zNear);
        float y = yDiff * (_panEyeZ / zNear);

        auto delta = phi::vec2(x, y);
        auto deltaWorld = phi::vec3(_panCameraRight * delta.x) + phi::vec3(-_panCameraUp * delta.y);

        _panCameraPos = _camera->getTransform()->getPosition();
        _panTargetCameraPos += deltaWorld;
        _panDelta = _panTargetCameraPos - _panCameraPos;
        _panInertiaTime = 0.0;
        _panLastMouseMoveTime = phi::time::totalSeconds * 1000.0;
        _panDoingInertia = true;
    }

    void cameraInputController::panMouseUp()
    {
        _panning = false;
        auto nowMilliseconds = phi::time::totalSeconds * 1000.0;
        auto deltaTime = static_cast<float>(glm::max(10.0, nowMilliseconds - _panLastMouseMoveTime));
        _panCameraPos = _camera->getTransform()->getPosition();
        _panDelta = _panTargetCameraPos - _panCameraPos;
        _panDelta += _panDelta * (1.0f - glm::min(1.0f, glm::max(0.0f, deltaTime / 100.0f)));
    }

    void cameraInputController::panUpdate()
    {
        if (!_panDoingInertia)
            return;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _panInertiaTime += deltaMilliseconds;
        auto desFactor = _panning ? 100.0f : 325.0f;
        auto percent = -glm::exp(_panInertiaTime / -desFactor) + 1.0f;
        auto delta = static_cast<float>(glm::length(_panDelta) * percent);

        if (delta == 0.0f)
            return;

        auto dir = glm::normalize(_panDelta);
        _camera->getTransform()->setLocalPosition(_panCameraPos + dir * delta);

        if (percent >= 1.0f)
            _panDoingInertia = false;
    }

    void cameraInputController::panCancel()
    {
        _panDelta = phi::vec3();
    }

    void cameraInputController::rotationMouseDown(int mouseX, int mouseY)
    {
        rotationCancel();
        panCancel();

        auto y = static_cast<int>(_camera->getResolution().height) - mouseY;
        auto zBufferValue = framebuffer::defaultFramebuffer->getZBufferValue(mouseX, y);

        if (zBufferValue == 1.0f)
            _rotationTargetPos = glm::vec3();
        else
        {
            auto worldZ = _camera->getWorldZRelativeToCamera(zBufferValue);
            auto worldPosition = _camera->getWorldPositionRelativeToCamera(mouseX, mouseY, worldZ);

            auto transform = _camera->getTransform();
            auto camPos = transform->getPosition();
            auto camDir = transform->getDirection();
            auto camRight = transform->getRight();
            auto camUp = transform->getUp();

            _rotationTargetPos =
                camPos +
                (-camRight * worldPosition.x) +
                (camUp * worldPosition.y) +
                (camDir * worldPosition.z);
        }

        _rotationLastMouseMoveTime = 0.0;
        _rotating = true;
    }

    void cameraInputController::rotationMouseMove()
    {
        auto resolution = _camera->getResolution();
        auto w = resolution.width;
        auto h = resolution.height;

        auto dx = _mousePosX - _lastMousePosX;
        auto dy = _mousePosY - _lastMousePosY;

        auto x = (dx / w) * 3.0f * phi::PI;
        auto y = (dy / h) * 3.0f * phi::PI;

        auto delta = phi::vec2(x, y);

        auto lastRemainingRotation = (1.0f - _rotationInertiaLastPercent) * glm::length(_rotationDelta);
        _rotationDelta = glm::normalize(delta) * (lastRemainingRotation + glm::length(delta));

        _rotationLastMouseMoveTime = phi::time::totalSeconds * 1000.0;
        _rotationInertiaTime = 0.0;
        _rotationDoingInertia = true;
        _rotationInertiaLastPercent = 0.0f;
    }

    void cameraInputController::rotationMouseUp()
    {
        _rotating = false;

        if (glm::length(_rotationDelta) == 0.0f)
            return;

        auto nowMilliseconds = phi::time::totalSeconds * 1000.0;
        auto deltaTime = static_cast<float>(glm::max(10.0, nowMilliseconds - _rotationLastMouseMoveTime));
        auto lastRemainingRotation = (1.0f - _rotationInertiaLastPercent) * glm::length(_rotationDelta);
        _rotationDelta = glm::normalize(_rotationDelta) * (lastRemainingRotation + lastRemainingRotation * (1.0f - glm::min(1.0f, glm::max(0.0f, deltaTime / 100.0f))));
    }

    void cameraInputController::rotationUpdate()
    {
        if (!_rotationDoingInertia)
            return;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0f;
        _rotationInertiaTime += deltaMilliseconds;
        auto desFactor = _rotating ? 100.0f : 325.0f;
        auto percent = static_cast<float>(-glm::exp(_rotationInertiaTime / -desFactor) + 1.0);
        auto delta = _rotationDelta * (percent - _rotationInertiaLastPercent);
        _rotationInertiaLastPercent = percent;

        _camera->orbit(_rotationTargetPos, phi::vec3(0.0f, 1.0f, 0.0f), -_camera->getTransform()->getRight(), -delta.x, -delta.y);
        if (percent >= 1.0f)
            _rotationDoingInertia = false;
    }

    void cameraInputController::rotationCancel()
    {
        _rotationDelta = phi::vec2();
    }
}