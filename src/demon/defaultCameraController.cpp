#include <precompiled.h>
#include "defaultCameraController.h"

#include <core\time.h>

#include <rendering\camera.h>

namespace demon
{
    defaultCameraController::defaultCameraController(phi::scene* scene) :
        _scene(scene),
        cameraController(scene->camera),
        _rotating(false),
        _panning(false),
        _zoomSpeed(0.0f),
        _zoomInertiaTime(0u),
        _zoomSpeedAccumulationTime(0u),
        _zoomBounceAnimation(nullptr),
        _rotationDelta(phi::vec2()),
        _rotationInertiaTime(0u),
        _rotationDoingInertia(false),
        _rotationLastMouseMoveTime(0u),
        _panInertiaTime(0u),
        _panDelta(phi::vec3()),
        _panDoingInertia(false)
    {
    }

    void defaultCameraController::onMouseDown(phi::mouseEventArgs* e)
    {
        if (e->rightButtonPressed && !_panning)
            rotationMouseDown(e->x, e->y);
        else if (e->middleButtonPressed && !_rotating)
            panMouseDown(e->x, e->y);
    }

    void defaultCameraController::onMouseMove(phi::mouseEventArgs* e)
    {
        _lastMousePosX = _mousePosX;
        _lastMousePosY = _mousePosY;
        _mousePosX = e->x;
        _mousePosY = e->y;

        if (_rotating)
            rotationMouseMove();
        if (_panning)
            panMouseMove();
    }

    void defaultCameraController::onMouseUp(phi::mouseEventArgs* e)
    {
        if (e->middleButtonPressed)
            panMouseUp();

        if (e->rightButtonPressed)
            rotationMouseUp();
    }

    void defaultCameraController::onMouseWheel(phi::mouseEventArgs* e)
    {
        if (!_panning && !_rotating)
            zoomMouseWheel(e->x, e->y, e->wheelDelta);
    }

    void defaultCameraController::update()
    {
        zoomUpdate();
        rotationUpdate();
        panUpdate();

        _lastMousePosX = _mousePosX;
        _lastMousePosY = _mousePosY;
    }

    void defaultCameraController::zoomMouseWheel(int mouseX, int mouseY, float delta)
    {
        if (delta > 0.0f && _zoomBounceAnimation)
            return;

        if (_zoomBounceAnimation)
        {
            phi::floatAnimator::cancelAnimation(_zoomBounceAnimation);
            _zoomBounceAnimation = nullptr;
        }

        rotationCancel();
        panCancel();

        auto zBufferValue = _scene->getZBufferValue(mouseX, _camera->getResolution().h - mouseY);

        auto camera = *_camera;
        phi::mat4 proj = _camera->getProjectionMatrix();

        float z;
        if (zBufferValue == 1.0f)
            z = 10.0f;
        else
            z = -proj[3].z / (zBufferValue * -2.0f + 1.0f - proj[2].z);

        auto zNear = _camera->getZNear();
        auto iez = 1.0f / zNear;
        auto zFar = _camera->getZFar();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = static_cast<float>(_camera->getResolution().w);
        auto h = static_cast<float>(_camera->getResolution().h);

        auto hh = h * 0.5f;
        auto hw = w * 0.5f;

        auto ys0 = mouseY - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = mouseX - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * aspect;

        auto x = (xm0 / zNear) * z;
        auto y = (ym0 / zNear) * z;

        auto cameraTransform = _camera->getTransform();
        phi::vec3 camDir = cameraTransform->getDirection();
        phi::vec3 camRight = cameraTransform->getRight();
        phi::vec3 camUp = cameraTransform->getUp();

        _zoomDir = glm::normalize(camDir * z + -camRight * static_cast<float>(x) + camUp * static_cast<float>(y));

        _zoomInertiaTime = 0u;
        _zoomDistanceTraveled = 0.0f;

        if (glm::sign(_zoomSpeed) != glm::sign(delta))
            _zoomSpeed = 0.0f;

        if (_zoomSpeedAccumulationTime <= 0u)
        {
            _zoomSpeed = 0.0f;
            _zoomSpeedAccumulationTime = ZOOM_ACCUMULATION_TIME;
        }

        _zoomSpeed += (delta / 120.0f) * ZOOM_FACTOR;
        _zoomDistanceLimit = glm::max(z - zNear - ZOOM_MAX_BOUNCE - 0.0001f, 0.0f); // Small gap so the camera does not go past the z-near plane
        _zoomCameraPos = cameraTransform->getPosition();
    }

    void defaultCameraController::zoomUpdate()
    {
        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _zoomSpeedAccumulationTime = glm::max(_zoomSpeedAccumulationTime - deltaMilliseconds, 0.0);

        if (_zoomSpeed == 0.0f)
            return;

        _zoomInertiaTime += deltaMilliseconds;
        auto percent = static_cast<float>(-glm::exp(_zoomInertiaTime / -325.0f) + 1.0f);
        auto delta = _zoomSpeed * percent;

        if (percent > 0.998f)
            _zoomSpeed = 0.0f;

        if (delta > _zoomDistanceLimit)
        {
            _camera->getTransform()->setLocalPosition(_zoomCameraPos + _zoomDir * _zoomDistanceLimit);
            _zoomDistanceTraveled = _zoomDistanceLimit;
            _zoomSpeed = 0.0f;

            zoomCancel();

            auto speedPercent = glm::clamp(delta / 2.0f, 0.0f, 1.0f);
            auto bounceDistance = ZOOM_MIN_BOUNCE + (ZOOM_MAX_BOUNCE * speedPercent);
            auto cameraPosition = _camera->getTransform()->getPosition();

            _zoomBounceAnimation = new phi::floatAnimation
            (
                0.0f,
                bounceDistance,
                400,
                [this, cameraPosition](float t)
            {
                _camera->getTransform()->setLocalPosition(cameraPosition + _zoomDir * t);
            },
                0,
                phi::easingFunctions::easeRubberBack,
                [&] { _zoomBounceAnimation = nullptr; }
            );

            phi::floatAnimator::animateFloat(_zoomBounceAnimation);
        }
        else
            _camera->getTransform()->setLocalPosition(_zoomCameraPos + _zoomDir * delta);
    }

    void defaultCameraController::zoomCancel()
    {
        if (_zoomBounceAnimation)
        {
            phi::floatAnimator::cancelAnimation(_zoomBounceAnimation);
            _zoomBounceAnimation = nullptr;
        }

        _zoomSpeed = 0.0f;
    }

    void defaultCameraController::panMouseDown(int mouseX, int mouseY)
    {
        zoomCancel();
        rotationCancel();
        panCancel();

        auto zBufferValue = _scene->getZBufferValue(mouseX, _camera->getResolution().h - mouseY);
        phi::mat4 proj = _camera->getProjectionMatrix();

        if (zBufferValue == 1.0f)
            _panEyeZ = 20.0f;
        else
            _panEyeZ = -proj[3].z / (zBufferValue * -2.0f + 1.0f - proj[2].z);

        auto cameraTransform = _camera->getTransform();
        _panCameraPos = _panTargetCameraPos = cameraTransform->getPosition();
        _panCameraRight = cameraTransform->getRight();
        _panCameraUp = cameraTransform->getUp();
        _panDelta = glm::vec3();
        _panning = true;
    }

    void defaultCameraController::panMouseMove()
    {
        auto zNear = _camera->getZNear();
        auto iez = 1.0f / zNear;
        auto zFar = _camera->getZFar();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = static_cast<float>(_camera->getResolution().w);
        auto h = static_cast<float>(_camera->getResolution().h);

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
        _panInertiaTime = 0u;
        _panLastMouseMoveTime = phi::time::totalSeconds * 1000.0;
        _panDoingInertia = true;
    }

    void defaultCameraController::panMouseUp()
    {
        _panning = false;
        auto nowMilliseconds = phi::time::totalSeconds * 1000.0;
        auto deltaTime = static_cast<float>(glm::max(10.0, nowMilliseconds - _panLastMouseMoveTime));
        _panCameraPos = _camera->getTransform()->getPosition();
        _panDelta = _panTargetCameraPos - _panCameraPos;
        _panDelta += _panDelta * (1.0f - glm::min(1.0f, glm::max(0.0f, deltaTime / 100.0f)));
    }

    void defaultCameraController::panUpdate()
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

    void defaultCameraController::panCancel()
    {
        _panDelta = phi::vec3();
    }

    void defaultCameraController::rotationMouseDown(int mouseX, int mouseY)
    {
        zoomCancel();
        rotationCancel();
        panCancel();

        auto zBufferValue = _scene->getZBufferValue(mouseX, _camera->getResolution().h - mouseY);
        phi::mat4 proj = _camera->getProjectionMatrix();

        if (zBufferValue == 1.0f)
            _rotationTargetPos = glm::vec3(); //_targetPos = phi::scenesManager::get()->getScene()->getAabb()->getCenter();
        else
        {
            auto z = -proj[3].z / (zBufferValue * -2.0f + 1.0f - proj[2].z);
            auto zNear = _camera->getZNear();
            auto iez = 1.0f / zNear;
            auto zFar = _camera->getZFar();
            auto aspect = _camera->getAspect();
            auto fov = _camera->getFov();

            auto tg = tan(fov * 0.5f) * zNear;

            auto w = static_cast<float>(_camera->getResolution().w);
            auto h = static_cast<float>(_camera->getResolution().h);

            auto hh = h * 0.5f;
            auto hw = w * 0.5f;

            auto ys0 = mouseY - hh;
            auto yp0 = ys0 / hh;
            auto ym0 = -(yp0 * tg);

            auto xs0 = mouseX - hw;
            auto xp0 = xs0 / hw;
            auto xm0 = xp0 * tg * aspect;

            auto x = (xm0 / zNear) * (z);
            auto y = (ym0 / zNear) * (z);

            auto transform = _camera->getTransform();
            auto camPos = transform->getPosition();
            auto camDir = transform->getDirection();
            auto camRight = transform->getRight();
            auto camUp = transform->getUp();

            _rotationTargetPos = camPos + camDir * z + -camRight * (float)x + camUp * (float)y;
        }

        _rotationLastMouseMoveTime = 0u;
        _rotating = true;
    }

    void defaultCameraController::rotationMouseMove()
    {
        auto zNear = _camera->getZNear();
        auto iez = 1.0f / zNear;
        auto zFar = _camera->getZFar();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = static_cast<float>(_camera->getResolution().w);
        auto h = static_cast<float>(_camera->getResolution().h);

        auto dx = _mousePosX - _lastMousePosX;
        auto dy = _mousePosY - _lastMousePosY;

        auto x = (dx / w) * 3 * phi::PI;
        auto y = (dy / h) * 3 * phi::PI;

        auto delta = phi::vec2(x, y);

        auto lastRemainingRotation = (1.0f - _rotationInertiaLastPercent) * glm::length(_rotationDelta);
        _rotationDelta = glm::normalize(delta) * (lastRemainingRotation + glm::length(delta));

        _rotationLastMouseMoveTime = phi::time::totalSeconds * 1000.0;
        _rotationInertiaTime = 0u;
        _rotationDoingInertia = true;
        _rotationInertiaLastPercent = 0.0f;
    }

    void defaultCameraController::rotationMouseUp()
    {
        _rotating = false;

        if (glm::length(_rotationDelta) == 0.0f)
            return;

        auto nowMilliseconds = phi::time::totalSeconds * 1000.0;
        auto deltaTime = static_cast<float>(glm::max(10.0, nowMilliseconds - _rotationLastMouseMoveTime));
        auto lastRemainingRotation = (1.0f - _rotationInertiaLastPercent) * glm::length(_rotationDelta);
        _rotationDelta = glm::normalize(_rotationDelta) * (lastRemainingRotation + lastRemainingRotation * (1.0f - glm::min(1.0f, glm::max(0.0f, deltaTime / 100.0f))));
    }

    void defaultCameraController::rotationUpdate()
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

    void defaultCameraController::rotationCancel()
    {
        _rotationDelta = phi::vec2();
    }
}