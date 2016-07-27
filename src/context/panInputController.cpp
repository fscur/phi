#include <precompiled.h>

#include <core\time.h>

#include <rendering\frameBuffer.h>

#include "panInputController.h"

namespace phi
{
    panInputController::panInputController(camera* camera) :
        inputController::inputController(),
        _camera(camera),
        _panning(false),
        _doingInertia(false),
        _eyeZ(0.0f),
        _cameraPos(phi::vec3()),
        _cameraRight(phi::vec3()),
        _cameraUp(phi::vec3()),
        _delta(phi::vec3()),
        _targetCameraPos(phi::vec3()),
        _lastMouseMoveTime(0.0),
        _inertiaTime(0.0),
        _lastMousePosX(0),
        _lastMousePosY(0)
    {
    }

    void panInputController::cancelPan()
    {
        _delta = phi::vec3();
        _doingInertia = false;
    }

    bool panInputController::onMouseDown(mouseEventArgs* e)
    {
        if (!e->middleButtonPressed)
            return false;

        cancelPan();

        auto y = static_cast<int>(_camera->getResolution().height) - e->y;
        auto zBufferValue = framebuffer::defaultFramebuffer->getZBufferValue(e->x, y);

        if (zBufferValue == 1.0f)
            _eyeZ = 20.0f;
        else
            _eyeZ = _camera->zBufferToDepth(zBufferValue);

        auto cameraTransform = _camera->getTransform();
        _cameraPos = _targetCameraPos = cameraTransform->getPosition();
        _cameraRight = cameraTransform->getRight();
        _cameraUp = cameraTransform->getUp();

        _delta = vec3();
        _panning = true;
        _lastMousePosX = e->x;
        _lastMousePosY = e->y;

        return true;
    }

    bool panInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_panning)
            return false;

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

        auto ys1 = (float)e->y - hh;
        auto yp1 = ys1 / hh;
        auto ym1 = -(yp1 * tg);

        auto xs1 = (float)e->x - hw;
        auto xp1 = xs1 / hw;
        auto xm1 = xp1 * tg * aspect;

        auto xDiff = xm1 - xm0;
        auto yDiff = ym1 - ym0;

        float x = xDiff * (_eyeZ / zNear);
        float y = yDiff * (_eyeZ / zNear);

        auto delta = phi::vec2(x, y);
        auto deltaWorld = phi::vec3(_cameraRight * delta.x) + phi::vec3(-_cameraUp * delta.y);

        _cameraPos = _camera->getTransform()->getPosition();
        _targetCameraPos += deltaWorld;
        _delta = _targetCameraPos - _cameraPos;
        _inertiaTime = 0.0;
        _lastMouseMoveTime = phi::time::totalSeconds * 1000.0;
        _doingInertia = true;
        _lastMousePosX = e->x;
        _lastMousePosY = e->y;

        return true;
    }

    bool panInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->middleButtonPressed)
            return false;

        _panning = false;
        auto nowMilliseconds = phi::time::totalSeconds * 1000.0;
        auto deltaTime = static_cast<float>(glm::max(10.0, nowMilliseconds - _lastMouseMoveTime));
        _cameraPos = _camera->getTransform()->getPosition();
        _delta = _targetCameraPos - _cameraPos;
        _delta += _delta * (1.0f - glm::min(1.0f, glm::max(0.0f, deltaTime / 100.0f)));

        return true;
    }

    bool panInputController::update()
    {
        if (!_doingInertia)
            return false;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _inertiaTime += deltaMilliseconds;
        auto desFactor = _panning ? 100.0f : 325.0f;
        auto percent = -glm::exp(_inertiaTime / -desFactor) + 1.0f;
        auto delta = static_cast<float>(glm::length(_delta) * percent);

        if (delta == 0.0f)
            return false;

        auto dir = glm::normalize(_delta);
        _camera->getTransform()->setLocalPosition(_cameraPos + dir * delta);

        if (percent >= 1.0f)
            _doingInertia = false;

        return true;
    }
}