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
        _zoomTime(0u),
        _zoomAccumulationTime(0u),
        _zoomBounceValue(0.0f),
        _zoomBounceAnimation(nullptr)
    {
    }

    void defaultCameraController::initPan(int mouseX, int mouseY)
    {
        if (_zoomBounceAnimation)
            return;

        _zoomSpeed = 0.0f;

        _zBufferValue = _scene->getZBufferValue(mouseX, _camera->getResolution().h - mouseY);
        phi::mat4 proj = _camera->getProjectionMatrix();

        if (_zBufferValue == 1.0f)
            _eyeZ = 20.0f;
        else
            _eyeZ = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);

        auto cameraTransform = _camera->getTransform();
        _cameraPos = cameraTransform->getPosition();
        _cameraRight = cameraTransform->getRight();
        _cameraUp = cameraTransform->getUp();
        _startPosX = mouseX;
        _startPosY = mouseY;
        _panning = true;
    }

    void defaultCameraController::initRotate(int mouseX, int mouseY)
    {
        if (_zoomBounceAnimation)
            return;

        _zoomSpeed = 0.0f;

        _zBufferValue = _scene->getZBufferValue(mouseX, _camera->getResolution().h - mouseY);

        phi::mat4 proj = _camera->getProjectionMatrix();

        _lastMousePosX = mouseX;
        _lastMousePosY = mouseY;

        float z = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);
        if (_zBufferValue == 1.0f)
            //_targetPos = phi::scenesManager::get()->getScene()->getAabb()->getCenter();
            _targetPos = glm::vec3();
        else
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

            _targetPos = camPos + camDir * z + -camRight * (float)x + camUp * (float)y;
        }

        _eyeZ = z;
        _rotating = true;
    }

    void defaultCameraController::pan(int mouseX, int mouseY)
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

        auto ys0 = (float)_startPosY - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = (float)_startPosX - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * aspect;

        auto ys1 = (float)mouseY - hh;
        auto yp1 = ys1 / hh;
        auto ym1 = -(yp1 * tg);

        auto xs1 = (float)mouseX - hw;
        auto xp1 = xs1 / hw;
        auto xm1 = xp1 * tg * aspect;

        auto eyeZ = _eyeZ;

        auto xDiff = xm1 - xm0;
        auto yDiff = ym1 - ym0;

        float x = xDiff * (eyeZ / zNear);
        float y = yDiff * (eyeZ / zNear);

        phi::vec3 pos = _cameraPos - (phi::vec3(-_cameraRight * x) + phi::vec3(_cameraUp * y));

        _camera->moveTo(pos);
    }

    void defaultCameraController::rotate(int mouseX, int mouseY)
    {
        auto zNear = _camera->getZNear();
        auto iez = 1.0f / zNear;
        auto zFar = _camera->getZFar();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = static_cast<float>(_camera->getResolution().w);
        auto h = static_cast<float>(_camera->getResolution().h);

        auto dx = _lastMousePosX - mouseX;
        auto dy = _lastMousePosY - mouseY;

        auto x = (dx / w) * 3 * phi::PI;
        auto y = (dy / h) * 3 * phi::PI;

        _camera->orbit(_targetPos, phi::vec3(0.0f, 1.0f, 0.0f), -_camera->getTransform()->getRight(), x, y);

        _lastMousePosX = mouseX;
        _lastMousePosY = mouseY;
    }

    void defaultCameraController::zoom(int mouseX, int mouseY, float delta)
    {
        if (delta > 0.0f && _zoomBounceAnimation)
            return;

        _zBufferValue = _scene->getZBufferValue(mouseX, _camera->getResolution().h - mouseY);

        auto camera = *_camera;
        phi::mat4 proj = _camera->getProjectionMatrix();

        float z;
        if (_zBufferValue == 1.0f)
            z = 10.0f;
        else
            z = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);

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

        auto transform = _camera->getTransform();
        phi::vec3 camPos = transform->getPosition();
        phi::vec3 camDir = transform->getDirection();
        phi::vec3 camRight = transform->getRight();
        phi::vec3 camUp = transform->getUp();

        _zoomDir = glm::normalize(camDir * z + -camRight * (float)x + camUp * (float)y);

        _zoomTime = 0u;
        _zoomCurrent = 0.0f;

        if (glm::sign(_zoomSpeed) != glm::sign(delta))
            _zoomSpeed = 0.0f;

        if (_zoomAccumulationTime <= 0u)
        {
            _zoomSpeed = 0.0f;
            _zoomAccumulationTime = ZOOM_ACCUMULATION_TIME;
        }

        _zoomSpeed += (delta / 120.0f) * ZOOM_FACTOR;
        _zoomLimit = glm::max(z - zNear - ZOOM_MAX_BOUNCE - 0.0001f, 0.0f); // Small gap so the camera does not go past the z-near plane

        if (_zoomBounceAnimation)
        {
            phi::floatAnimator::cancelAnimation(_zoomBounceAnimation);
            _zoomBounceAnimation = nullptr;
        }
    }

    void defaultCameraController::onMouseDown(phi::mouseEventArgs* e)
    {
        if (e->rightButtonPressed)
            initRotate(e->x, e->y);
        else if (e->middleButtonPressed)
            initPan(e->x, e->y);
    }

    void defaultCameraController::onMouseMove(phi::mouseEventArgs* e)
    {
        if (_rotating)
            rotate(e->x, e->y);
        if (_panning)
            pan(e->x, e->y);
    }

    void defaultCameraController::onMouseUp(phi::mouseEventArgs* e)
    {
        _panning = _rotating = false;
    }

    void defaultCameraController::onMouseWheel(phi::mouseEventArgs* e)
    {
        zoom(e->x, e->y, e->wheelDelta);
    }

    void defaultCameraController::updateZoom()
    {
        auto deltaSeconds = static_cast<float>(phi::time::deltaSeconds);
        auto deltaMilliseconds = static_cast<unsigned int>(phi::time::deltaSeconds * 1000.0f);
        _zoomTime += deltaMilliseconds;
        _zoomAccumulationTime = glm::max(_zoomAccumulationTime - deltaMilliseconds, 0u);

        auto delta = _zoomSpeed * glm::exp(_zoomTime / -325.0f);

        if (_zoomCurrent + delta > _zoomLimit)
        {
            _camera->getTransform()->translate(_zoomDir * (_zoomLimit - _zoomCurrent));
            _zoomCurrent = _zoomLimit;
            _zoomSpeed = 0.0f;

            if (_zoomBounceAnimation)
                phi::floatAnimator::cancelAnimation(_zoomBounceAnimation);

            auto speedPercent = glm::clamp(delta / 2.0f, 0.0f, 1.0f);
            auto bounceDistance = ZOOM_MIN_BOUNCE + (ZOOM_MAX_BOUNCE * speedPercent);
            auto cameraPosition = _camera->getTransform()->getPosition();

            _zoomBounceValue = 0.0f;
            _zoomBounceAnimation = new phi::floatAnimation
            (
                &_zoomBounceValue,
                bounceDistance,
                400,
                [this, cameraPosition](float t)
                {
                    _camera->getTransform()->setLocalPosition(cameraPosition + _zoomDir * _zoomBounceValue);
                },
                0,
                phi::easingFunctions::easeRubberBack,
                [&] { _zoomBounceAnimation = nullptr; }
            );

            phi::floatAnimator::animateFloat(_zoomBounceAnimation);
        }
        else
        {
            _zoomCurrent += delta;
            _camera->getTransform()->translate(_zoomDir * delta);
        }
    }

    void defaultCameraController::update()
    {
        updateZoom();
    }
}