#include <precompiled.h>

#include <core/time.h>

#include <rendering/defaultFramebuffer.h>
#include <rendering/framebuffer.h>

#include "cameraZoomInputController.h"

namespace phi
{
    cameraZoomInputController::cameraZoomInputController(camera* camera) :
        inputController::inputController(),
        _camera(camera),
        _direction(phi::vec3()),
        _startingCameraPosition(phi::vec3()),
        _speed(0.0f),
        _distanceTraveled(0.0f),
        _distanceLimit(0.0f),
        _bounceDistance(0.0f),
        _speedAccumulationTime(0.0),
        _inertiaTime(0.0),
        _bounceAnimation(new positionAnimation(_camera->getTransform(), easingFunctions::easeRubberBack))
    {
    }

    cameraZoomInputController::~cameraZoomInputController()
    {
        cancelBounceAnimation();
        safeDelete(_bounceAnimation);
    }

    void cameraZoomInputController::cancelBounceAnimation()
    {
        _bounceAnimation->stop();
    }
    
    void cameraZoomInputController::cancelZoom()
    {
        cancelBounceAnimation();
        _speed = 0.0f;
    }

    void cameraZoomInputController::startBounceAnimation(float speedPercent)
    {
        cancelBounceAnimation();

        auto bounceDistance = MIN_BOUNCE + ((MAX_BOUNCE - MIN_BOUNCE) * speedPercent);
        auto cameraPosition = _camera->getTransform()->getPosition();

        _bounceAnimation->start(cameraPosition, cameraPosition + (_direction * bounceDistance), 0.4);
    }

    bool cameraZoomInputController::onMouseWheel(mouseEventArgs* e)
    {
        if (e->wheelDelta > 0.0f && _bounceAnimation->isRunning())
            return false;

        cancelBounceAnimation();

        auto y = static_cast<int>(_camera->getResolution().height) - e->y;
        auto zBufferValue = defaultFramebuffer::getZBufferValue(e->x, y);

        float z = 10.0;
        if (zBufferValue != 1.0f)
            z = _camera->zBufferToDepth(zBufferValue);

        auto position = _camera->screenPointToView(e->x, e->y, z);

        auto cameraTransform = _camera->getTransform();
        auto cameraOrientation = cameraTransform->getOrientation();
        auto zNear = _camera->getNear();

        _direction = glm::normalize(cameraOrientation * vec3(-position.x, position.y, z));

        if (glm::sign(_speed) != glm::sign(e->wheelDelta))
            _speed = 0.0f;

        if (_speedAccumulationTime <= 0.0)
        {
            _speed = 0.0f;
            _speedAccumulationTime = ACCUMULATION_TIME;
        }

        _speed += (e->wheelDelta / 120.0f) * ZOOM_FACTOR;

        _inertiaTime = 0.0;
        _distanceTraveled = 0.0f;
        _distanceLimit = glm::max(z - zNear - MAX_BOUNCE - Z_NEAR_AVOIDING_GAP, 0.0f);
        _startingCameraPosition = cameraTransform->getPosition();

        return true;
    }

    bool cameraZoomInputController::update()
    {
        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _speedAccumulationTime = glm::max(_speedAccumulationTime - deltaMilliseconds, 0.0);

        _bounceAnimation->animate();

        if (_speed == 0.0f)
            return false;

        _inertiaTime += deltaMilliseconds;
        auto distancePercent = static_cast<float>(-glm::exp(_inertiaTime / -325.0f) + 1.0f);
        auto delta = _speed * distancePercent;

        if (distancePercent > 0.998f)
            _speed = 0.0f;

        if (delta > _distanceLimit)
        {
            _camera->getTransform()->setLocalPosition(_startingCameraPosition + _direction * _distanceLimit);
            _distanceTraveled = _distanceLimit;
            _speed = 0.0f;

            auto animationSpeed = glm::clamp(delta / 2.0f, 0.0f, 1.0f);
            startBounceAnimation(animationSpeed);
        }
        else
            _camera->getTransform()->setLocalPosition(_startingCameraPosition + _direction * delta);

        return true;
    }
}