#include <precompiled.h>
#include "defaultCameraController.h"

#include <core\input.h>
#include <core\time.h>
#include <core\mesh.h>
#include <core\ray.h>
#include <core\multiCommand.h>

#include <rendering\camera.h>
#include <loader\importer.h>

#include <scenes\selectSceneObjectCommand.h>

#include <apps\application.h>

using namespace phi;

namespace demon
{
    using namespace phi;

    defaultCameraController::defaultCameraController(scene* scene, commandsManager* commandsManager) :
        cameraController(scene->getCamera()),
        _scene(scene),
        _commandsManager(commandsManager),
        _isCtrlPressed(false),
        _mousePosX(0),
        _mousePosY(0),
        _lastMousePosX(0),
        _lastMousePosY(0),
        _zoomDir(phi::vec3()),
        _zoomCameraPos(phi::vec3()),
        _zoomSpeed(0.0f),
        _zoomDistanceTraveled(0.0f),
        _zoomDistanceLimit(0.0f),
        _zoomSpeedAccumulationTime(0.0),
        _zoomInertiaTime(0.0),
        _zoomBounceAnimation(nullptr),
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
        _panInertiaTime(0.0),
        _dragging(false),
        _planeDrag(_scene->getPhysicsWorld()),
        _dragCollider(nullptr),
        _dragPlaneGridDoingInertia(false),
        _dragPlaneGridInertiaTime(0.0),
        _dragPlaneGridInitialPosition(phi::vec2()),
        _dragPlaneGridDelta(phi::vec2())
    {
        auto texturePath = phi::application::resourcesPath + "\\images\\grid.png";
        _gridImage = phi::importer::importImage(texturePath);
        _planeGridPass = _scene->getRenderer()->getPlaneGridRenderPass();
        _planeGridPass->setImage(_gridImage);
    }

    defaultCameraController::~defaultCameraController()
    {
        zoomCancel();
        phi::safeDelete(_gridImage);
    }

    void defaultCameraController::onKeyDown(keyboardEventArgs * e)
    {
        _isCtrlPressed = e->key == PHIK_CTRL;
    }

    void defaultCameraController::onKeyUp(keyboardEventArgs * e)
    {
        if (e->key == PHIK_CTRL)
            _isCtrlPressed = false;
    }

    void defaultCameraController::onMouseDown(phi::mouseEventArgs* e)
    {
        if (e->leftButtonPressed)
        {
            auto selectedMesh = _scene->pick(e->x, e->y);

            auto selectedObjects = _scene->getSelectedObjects();
            auto shouldUnselect = selectedObjects.size() > 0u;

            if (selectedMesh)
            {
                _commandsManager->executeCommand(
                    new selectSceneObjectCommand(selectedMesh->getNode(), selectedObjects));
            }
            else if (shouldUnselect)
            {
                _commandsManager->executeCommand(
                    new unselectSceneObjectCommand(selectedObjects));
            }

            dragMouseDown(e->x, e->y);
        }

        if (e->middleButtonPressed && !_rotating)
            panMouseDown(e->x, e->y);

        if (e->rightButtonPressed && !_panning)
            rotationMouseDown(e->x, e->y);
    }

    void defaultCameraController::onMouseMove(phi::mouseEventArgs* e)
    {
        _lastMousePosX = _mousePosX;
        _lastMousePosY = _mousePosY;
        _mousePosX = e->x;
        _mousePosY = e->y;

        if (_dragging)
            dragMouseMove();
        if (_panning)
            panMouseMove();
        if (_rotating)
            rotationMouseMove();
    }

    void defaultCameraController::onMouseUp(phi::mouseEventArgs* e)
    {
        if (e->leftButtonPressed)
            dragMouseUp();

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
        dragUpdate();
        zoomUpdate();
        rotationUpdate();
        panUpdate();

        _lastMousePosX = _mousePosX;
        _lastMousePosY = _mousePosY;
    }

    void defaultCameraController::dragMouseDown(int mouseX, int mouseY)
    {
        auto pickMesh = _scene->pick(mouseX, mouseY);
        if (!pickMesh)
            return;

        auto node = pickMesh->getNode();
        auto collider = node->getComponent<boxCollider>();
        auto obb = collider->getObb();

        vec3* positions;
        vec3* normals;
        size_t count;
        auto ray = _camera->screenPointToRay(static_cast<float>(mouseX), static_cast<float>(mouseY));
        if (ray.intersects(obb, positions, normals, count))
        {
            _dragCollider = collider;
            auto dragObject = node;
            while (dragObject->getParent()->getParent() != nullptr)
                dragObject = dragObject->getParent();

            auto normal = normals[0];
            auto rayCastOnPlanePosition = positions[0];

            phi::safeDeleteArray(normals);
            phi::safeDeleteArray(positions);

            auto dragPlane = plane(rayCastOnPlanePosition, normal);

            auto planePosition = obb.getPositionAt(-normal);
            planePosition = glm::normalize(planePosition) * (glm::length(planePosition) + DECIMAL_TRUNCATION);

            _planeGridPass->setPositionAndOrientation(planePosition, normal);
            _planeGridPass->projectAndSetFocusPosition(planePosition);
            _planeGridPass->show();

            _dragPlaneGridDelta = vec2();
            _dragPlaneGridDoingInertia = false;
            _dragging = true;

            _planeDrag.startDrag(dragObject, dragPlane);
        }
    }

    void defaultCameraController::dragMouseMove()
    {
        auto ray = _camera->screenPointToRay(static_cast<float>(_mousePosX), static_cast<float>(_mousePosY));
        _planeDrag.updateDrag(ray);

        _dragPlaneGridInitialPosition = _planeGridPass->getFocusPosition();
        _dragPlaneGridDelta = _planeGridPass->projectPoint(_dragCollider->getObb().center) - _dragPlaneGridInitialPosition;
        _dragPlaneGridDoingInertia = true;
        _dragPlaneGridInertiaTime = 0.0f;
    }

    void defaultCameraController::dragMouseUp()
    {
        if (_dragging)
        {
            _planeGridPass->hide();
            _dragging = false;
        }
    }

    void defaultCameraController::dragUpdate()
    {
        if (!_dragPlaneGridDoingInertia)
            return;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _dragPlaneGridInertiaTime += deltaMilliseconds;
        auto percent = -glm::exp(_dragPlaneGridInertiaTime / -100.0f) + 1.0f;
        auto delta = _dragPlaneGridDelta * static_cast<float>(percent);
        _planeGridPass->setFocusPosition(_dragPlaneGridInitialPosition + delta);

        _dragPlaneGridDoingInertia = percent < 1.0f;
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

        auto zBufferValue = _scene->getZBufferValue(mouseX, static_cast<int>(_camera->getHeight()) - mouseY);

        float z = 10.0;
        if (zBufferValue != 1.0f)
            z = _camera->getWorldZRelativeToCamera(mouseX, mouseY, zBufferValue);

        auto position = _camera->getWorldPositionRelativeToCamera(mouseX, mouseY, z);

        auto cameraTransform = _camera->getTransform();
        auto camDir = cameraTransform->getDirection();
        auto camRight = cameraTransform->getRight();
        auto camUp = cameraTransform->getUp();
        auto zNear = _camera->getNear();

        _zoomDir = glm::normalize(
            camDir * z +
            -camRight * position.x +
            camUp * position.y);

        _zoomInertiaTime = 0.0;
        _zoomDistanceTraveled = 0.0f;

        if (glm::sign(_zoomSpeed) != glm::sign(delta))
            _zoomSpeed = 0.0f;

        if (_zoomSpeedAccumulationTime <= 0.0)
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

        auto zBufferValue = _scene->getZBufferValue(mouseX, static_cast<int>(_camera->getHeight()) - mouseY);

        if (zBufferValue == 1.0f)
            _panEyeZ = 20.0f;
        else
            _panEyeZ = _camera->getWorldZRelativeToCamera(mouseX, mouseY, zBufferValue);

        auto cameraTransform = _camera->getTransform();
        _panCameraPos = _panTargetCameraPos = cameraTransform->getPosition();
        _panCameraRight = cameraTransform->getRight();
        _panCameraUp = cameraTransform->getUp();
        _panDelta = glm::vec3();
        _panning = true;
    }

    void defaultCameraController::panMouseMove()
    {
        auto zNear = _camera->getNear();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = _camera->getWidth();
        auto h = _camera->getHeight();

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

    void defaultCameraController::panMouseUp()
    {
        _panning = false;
        auto nowMilliseconds = phi::time::totalSeconds * 1000.0;
        auto deltaTime = static_cast<float>(glm::max(10.0, nowMilliseconds - _panLastMouseMoveTime));
        _panCameraPos = _camera->getTransform()->getPosition();
        _panDelta = _panTargetCameraPos - _panCameraPos;
        _panDelta += _panDelta * (1.0f - glm::clamp(deltaTime / 100.0f, 0.0f, 1.0f));
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

        auto zBufferValue = _scene->getZBufferValue(mouseX, static_cast<int>(_camera->getHeight()) - mouseY);

        if (zBufferValue == 1.0f)
            _rotationTargetPos = glm::vec3();
        else
        {
            auto worldZ = _camera->getWorldZRelativeToCamera(mouseX, mouseY, zBufferValue);
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

    void defaultCameraController::rotationMouseMove()
    {
        auto w = _camera->getWidth();
        auto h = _camera->getHeight();

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