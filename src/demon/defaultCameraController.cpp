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
        _dragObject(nullptr),
        _dragCollider(nullptr),
        _dragPlane(plane(vec3(), vec3())),
        _dragDoingInertia(false),
        _dragInertiaTime(0.0),
        _dragStartPos(phi::vec2()),
        _dragDelta(phi::vec2())
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
        _dragObject = nullptr;

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

    void defaultCameraController::moveObject(node* object, vec3 offset, vec3 planeNormal)
    {
        std::vector<transform*> transforms;
        std::vector<boxCollider*> colliders;
        object->traverse<boxCollider>([&colliders, &transforms, offset](boxCollider* b)
        {
            transforms.push_back(b->getNode()->getTransform());
            colliders.push_back(b);
        });

        auto createOffsetTransforms = [&transforms](vec3 offset) -> vector<transform*>
        {
            auto resultTransforms = vector<transform*>();
            for (auto t : transforms)
            {
                auto position = t->getPosition() + offset;
                auto rotation = t->getOrientation();
                auto size = t->getSize();
                auto offsetTransform = new transform();
                offsetTransform->setLocalPosition(position);
                offsetTransform->setLocalOrientation(rotation);
                offsetTransform->setLocalSize(size);
                resultTransforms.push_back(offsetTransform);
            }

            return resultTransforms;
        };

        auto offsetTransforms = createOffsetTransforms(offset);
        auto ignoreColliders = colliders;
        auto world = _scene->getPhysicsWorld();
        if (world->intersects(colliders, offsetTransforms, ignoreColliders))
        {
            auto sweepResults = world->intersectsSweep(colliders, offset, ignoreColliders);
            auto originalOffset = offset;
            auto dir = glm::normalize(originalOffset);
            offset = vec3();

            auto collisionsCount = sweepResults.collisions.size();
            if (collisionsCount > 0u)
            {
                auto foundPlaceToStayWithoutCollisionYeah = false;

                for (auto it = sweepResults.collisions.rbegin(); it != sweepResults.collisions.rend() && !foundPlaceToStayWithoutCollisionYeah; ++it)
                {
                    auto collision = *it;

                    auto limited = dir * glm::max(collision.distance - DECIMAL_TRUNCATION, 0.0f);
                    auto limitedTransforms = createOffsetTransforms(limited);

                    ignoreColliders.push_back(collision.collider);
                    if (!world->intersects(colliders, limitedTransforms, ignoreColliders))
                    {
                        foundPlaceToStayWithoutCollisionYeah = true;
                        auto dist = glm::dot(collision.normal, planeNormal);
                        auto normal = glm::normalize(collision.normal - dist * planeNormal);

                        auto up = glm::cross(normal, originalOffset);
                        auto dotDir = glm::cross(up, normal);
                        if (dotDir != vec3())
                            dotDir = glm::normalize(dotDir);

                        auto dotValue = glm::dot(originalOffset - limited, dotDir);

                        offset = limited + dotDir * dotValue;

                        phi::removeIfContains(ignoreColliders, collision.collider);
                        auto adjustSweepTest = world->intersectsSweep(colliders, limitedTransforms, offset - limited, ignoreColliders);
                        if (adjustSweepTest.collided)
                        {
                            auto firstCollision = adjustSweepTest.collisions[0];
                            offset = limited + dotDir * glm::max(firstCollision.distance - DECIMAL_TRUNCATION, 0.0f);
                        }
                    }

                    phi::removeIfContains(ignoreColliders, collision.collider);

                    for (auto t : limitedTransforms)
                        safeDelete(t);
                }
            }
        }

        for (auto t : offsetTransforms)
            safeDelete(t);

        object->getTransform()->translate(offset);
    }

    void defaultCameraController::dragMouseDown(int mouseX, int mouseY)
    {
        //std::vector<boxCollider*> colliders;
        //std::vector<node*> nodes;
        //_dragObject->traverse<boxCollider>([&colliders, &nodes](boxCollider* b) { colliders.push_back(b); nodes.push_back(b->getNode()); });
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
            _dragObject = node;
            while (_dragObject->getParent()->getParent() != nullptr)
                _dragObject = _dragObject->getParent();

            auto normal = normals[0];

            _dragOrigin = positions[0];
            _dragObjectStartPosition = _dragObject->getTransform()->getLocalPosition();

            phi::safeDeleteArray(normals);
            phi::safeDeleteArray(positions);

            _dragPlane = plane(_dragOrigin, normal);

            auto planeGridPosition = obb.center + -normal * (
                glm::abs(glm::dot(obb.axes[0] * obb.halfSizes.x, normal)) +
                glm::abs(glm::dot(obb.axes[1] * obb.halfSizes.y, normal)) +
                glm::abs(glm::dot(obb.axes[2] * obb.halfSizes.z, normal)) +
                DECIMAL_TRUNCATION * 10.0f);
            _planeGridPass->transform.setLocalPosition(planeGridPosition);
            _planeGridPass->transform.setDirection(normal);
            _planeGridPass->centerPosition = vec2();
            _planeGridPass->show();

            _dragDelta = vec2();
            _dragDoingInertia = false;
            _dragging = true;
            _dragCollider = collider;

            //moveObject(_dragObject, vec3(0.01f, -0.01f, -0.01f), normal);
        }
    }

    void defaultCameraController::dragMouseMove()
    {
        auto ray = _camera->screenPointToRay(static_cast<float>(_mousePosX), static_cast<float>(_mousePosY));
        float t;
        ray.intersects(_dragPlane, t);
        auto point = ray.getOrigin() + ray.getDirection() * t;

        auto diff = point - _dragOrigin;
        auto newPosition = _dragObjectStartPosition + diff;

        moveObject(_dragObject, newPosition - _dragObject->getTransform()->getLocalPosition(), _dragPlane.getNormal());

        auto planeOrigin = _planeGridPass->transform.getPosition();
        auto planeNormal = _planeGridPass->transform.getDirection();
        auto planeRight = _planeGridPass->transform.getRight();
        auto planeUp = _planeGridPass->transform.getUp();
        //auto centerBoundingBox = (_dragPlaneBottomLeft + _dragPlaneTopRight) * 0.5f + (newPosition - _dragObjectStartPosition);
        auto gridPlane = plane(planeOrigin, planeNormal);
        auto projectionGrid = gridPlane.projectPoint(_dragCollider->getObb().center);
        auto projectionGrid2D = vec2(glm::dot(planeRight, projectionGrid - planeOrigin), glm::dot(planeUp, projectionGrid - planeOrigin));
        //planePointProjection2D(planeOrigin, planeNormal, planeRight, planeUp, _dragCollider->getObb().center);

        _dragStartPos = _planeGridPass->centerPosition;
        _dragDelta = projectionGrid2D - _dragStartPos;
        _dragDoingInertia = true;
        _dragInertiaTime = 0.0f;
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
        if (!_dragDoingInertia)
            return;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _dragInertiaTime += deltaMilliseconds;
        auto percent = -glm::exp(_dragInertiaTime / -100.0f) + 1.0f;
        auto delta = _dragDelta * static_cast<float>(percent);
        _planeGridPass->centerPosition = _dragStartPos + delta;

        if (percent >= 1.0f)
            _dragDoingInertia = false;
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