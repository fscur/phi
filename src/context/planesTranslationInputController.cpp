#include <precompiled.h>

#include "planesTranslationInputController.h"

namespace phi
{
    planesTranslationInputController::planesTranslationInputController(camera* camera, layer* planesLayer, physicsLayerBehaviour* physicsBehaviour) :
        translationInputController(camera, planesLayer),
        _physicsBehaviour(physicsBehaviour),
        _lastChosenTranslationPlane(nullptr)
    {
    }

    void planesTranslationInputController::addTranslationPlane(translationPlane* translationPlane)
    {
        _translationPlanes.push_back(translationPlane);
        if (_translationPlanes.size() >= 1)
            _defaultTranslationPlane->hideGrid();
    }

    void planesTranslationInputController::removeTranslationPlane(translationPlane* translationPlane)
    {
        removeIfContains(_translationPlanes, translationPlane);
        if (_translationPlanes.size() == 0)
            _defaultTranslationPlane->showGrid();
    }

    bool planesTranslationInputController::canTranslateAt(vec3 normal)
    {
        auto planePosition = _draggingCollider->getObb().getPositionAt(-normal);
        auto cameraTransform = _camera->getTransform();
        auto cameraPosition = cameraTransform->getPosition();
        auto cameraDirection = _camera->getTransform()->getDirection();

        auto toPlaneDir = glm::normalize(planePosition - cameraPosition);

        auto discardDot = glm::abs(glm::dot(normal, toPlaneDir));
        auto isAgainstCamera = glm::dot(cameraDirection, normal) < 0.0f;
        return discardDot > 0.2f && isAgainstCamera;
    }

    vector<sweepCollision> planesTranslationInputController::findTouchCollisions()
    {
        vector<sweepCollision> touchs;

        //intersectionCollisionMultiTest touchTest;
        //touchTest.colliders = _collisionNodeTranslator->getColliders();
        //touchTest.transforms = _collisionNodeTranslator->getTransforms();
        //auto touchTestResult = _physicsBehaviour->getPhysicsWorld()->touchs(touchTest);

        //if (!touchTestResult.collided)
        //    return touchs;

        if (!_lastTranslationTouchs)
            return touchs;

        for (auto& collision : (*_lastTranslationTouchs))
        {
            if (canTranslateAt(collision.normal))
                touchs.push_back(collision);
        }

        return touchs;
    }

    bool planesTranslationInputController::existsTranslationPlaneWithNormal(vec3 normal)
    {
        auto translationPlane = std::find_if(_translationPlanes.begin(), _translationPlanes.end(),
            [normal](phi::translationPlane* tp)
        {
            return mathUtils::isClose(glm::dot(normal, tp->plane.normal), 1.0f);
        });

        return translationPlane != _translationPlanes.end();
    }

    translationPlane* planesTranslationInputController::createTranslationPlane(plane plane, boxCollider* collider)
    {
        if (existsTranslationPlaneWithNormal(plane.normal))
            return nullptr;

        auto planePosition = collider->getObb().getPositionAt(plane.normal);
        planePosition = phi::plane(planePosition, plane.normal).projectPoint(_draggingCollider->getObb().center);

        plane.origin = vec3();
        auto translationPlane = translationInputController::createTranslationPlane(plane, planePosition, collider, color(30.0f / 255.0f, 140.0f / 255.0f, 210.0f / 255.0f, 1.0f));
        _layer->add(translationPlane->planeGridNode);
        translationPlane->showGrid();

        return translationPlane;
    }

    void planesTranslationInputController::addPlanesIfNeeded(vector<sweepCollision> touchs)
    {
        for (auto& collision : touchs)
        {
            auto translationPlane = createTranslationPlane(plane(vec3(), collision.normal), collision.collider);
            if (translationPlane)
                addTranslationPlane(translationPlane);
        }
    }

    void planesTranslationInputController::removeDetachedPlanes(vector<sweepCollision> touchs)
    {
        vector<translationPlane*> translationPlanesToRemove;

        for (auto& translationPlane : _translationPlanes)
        {
            if (translationPlane == _lastChosenTranslationPlane)
                continue;

            auto translationPlaneNormal = translationPlane->plane.normal;
            auto touchsSearch = std::find_if(touchs.begin(), touchs.end(),
                [translationPlaneNormal](sweepCollision& c)
            {
                return mathUtils::isClose(glm::dot(translationPlaneNormal, c.normal), 1.0f);
            });

            if (touchsSearch == touchs.end())
                translationPlanesToRemove.push_back(translationPlane);
        }

        for (auto translationPlane : translationPlanesToRemove)
        {
            deletePlane(translationPlane);
            removeTranslationPlane(translationPlane);
        }
    }

    bool planesTranslationInputController::onMouseDown(mouseEventArgs * e)
    {
        translationInputController::onMouseDown(e);

        if (!_dragging)
            return false;

        _lastChosenTranslationPlane = nullptr;

        auto touchs = findTouchCollisions();
        addPlanesIfNeeded(touchs);

        _isTouchingCollidedObject = true;
        _isSwitchingPlanes = false;

        return true;
    }

    vec3 planesTranslationInputController::mouseOffsetToWorld(ivec2 mousePosition)
    {
        auto mouseDir = glm::normalize(vec2(_lastMousePosition - mousePosition));
        auto cameraTransform = _camera->getTransform();
        return glm::normalize(cameraTransform->getRight() * mouseDir.x + cameraTransform->getUp() * mouseDir.y);
    }

    translationPlane* planesTranslationInputController::findBestPlaneToDrag(vec3 dragDirection)
    {
        auto minNormalOnDragDirection = 1.0f;
        translationPlane* chosenPlane = nullptr;

        for (auto& translationPlane : _translationPlanes)
        {
            auto plane = translationPlane->plane;
            auto normalOnDragDirection = glm::abs(glm::dot(plane.normal, dragDirection));
            if (normalOnDragDirection < minNormalOnDragDirection)
            {
                minNormalOnDragDirection = normalOnDragDirection;
                chosenPlane = translationPlane;
            }
        }

        return chosenPlane;
    }

    bool isPointAbovePlane(vec3 point, plane plane)
    {
        auto dist = dot(point, plane.normal) - dot(plane.origin, plane.normal);
        return dist >= 0.0f;
    }

    bool planesTranslationInputController::isTouchingCollidedObject(vec3 offset, translationPlane* translationPlane, vec3& nearestPosition, plane& touchingPlane)
    {
        auto targetObb = translationPlane->collider->getObb();
        auto planes = targetObb.getPlanes();

        for (auto& plane : planes)
        {
            if (mathUtils::isClose(glm::dot(plane.normal, translationPlane->plane.normal), 1.0f))
                continue;

            for (auto& collider : (*_collisionNodeTranslator->getColliders()))
            {
                auto colliderObb = collider->getObb();

                float minDistanceToPlane = std::numeric_limits<float>().max();
                vec3 minPoint;
                auto isPointAbovePlaneAndUpdateMinDistance = [&minDistanceToPlane, &minPoint, &plane, &offset](vec3 point)
                {
                    auto distanceToPlane = plane.distanceFrom(point + offset);
                    auto isAbovePlane = distanceToPlane >= 0.0f;

                    if (distanceToPlane < minDistanceToPlane)
                    {
                        minDistanceToPlane = distanceToPlane;
                        minPoint = point;
                    }

                    return isAbovePlane;
                };

                auto obbCorners = colliderObb.getCorners();
                auto cornersCount = obbCorners.size();

                auto allAbovePlane = true;
                for (auto j = 0; j < cornersCount; ++j)
                {
                    if (!isPointAbovePlaneAndUpdateMinDistance(obbCorners[j]))
                    {
                        allAbovePlane = false;
                        break;
                    }
                }

                if (!allAbovePlane)
                    continue;

                nearestPosition = minPoint;
                touchingPlane = plane;
                return false;
            }
        }

        return true;
    }

    vec3 planesTranslationInputController::checkForPossibleSwitchOfPlanes(vec3 offset, translationPlane* translationPlane)
    {
        if (!translationPlane->collider)
            return offset;

        vec3 nearestPosition;
        plane touchingPlane;
        auto previousIsTouchingCollidedObject = _isTouchingCollidedObject;
        _isTouchingCollidedObject = isTouchingCollidedObject(offset, translationPlane, nearestPosition, touchingPlane);

        if (_isTouchingCollidedObject || !previousIsTouchingCollidedObject)
            return offset;

        if (!canTranslateAt(touchingPlane.normal))
            return offset;

        _isSwitchingPlanes = true;

        auto createdTranslationPlane = createTranslationPlane(touchingPlane, translationPlane->collider);
        if (createdTranslationPlane)
            addTranslationPlane(createdTranslationPlane);

        auto lineDirection = glm::normalize(offset);
        auto lineOrigin = nearestPosition;

        float t;
        auto intersected = touchingPlane.intersectsLine(lineOrigin, lineDirection, t);
        assert(intersected);

        auto stopOffset = lineDirection * t + touchingPlane.normal * DECIMAL_TRUNCATION;

        return stopOffset;
    }

    void planesTranslationInputController::translateOn(translationPlane* translationPlane, ivec2 mousePosition)
    {
        // TODO:
        // Only stops when at low mouse speed
        // Change color and opacity of planes based on dot with camera

        if (_isSwitchingPlanes)
        {
            auto difference = vec2(mousePosition) - vec2(_lastMousePosition);
            if (length(difference) < 25.0f)
                return;
            else
                _isSwitchingPlanes = false;
        }

        if (translationPlane != _lastChosenTranslationPlane)
        {
            if (_lastChosenTranslationPlane)
                translationPlane->plane.origin = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _lastChosenTranslationPlane->plane);
            else
                translationPlane->plane.origin = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _defaultTranslationPlane->plane);

            setupTranslationPlane(translationPlane);
        }

        auto position = getTranslationPosition(mousePosition, translationPlane);
        auto offset = position - _draggingRootNode->getTransform()->getLocalPosition();
        offset = checkForPossibleSwitchOfPlanes(offset, translationPlane);

        translateNode(offset);
        translatePlaneGrid(translationPlane);

        _lastChosenTranslationPlane = translationPlane;
        _lastMousePosition = mousePosition;
    }

    bool planesTranslationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        auto touchs = findTouchCollisions();
        addPlanesIfNeeded(touchs);
        if (!_isSwitchingPlanes)
            removeDetachedPlanes(touchs);

        auto mousePosition = ivec2(e->x, e->y);
        auto dragDirection = mouseOffsetToWorld(mousePosition);
        auto chosenTranslationPlane = findBestPlaneToDrag(dragDirection);

        if (chosenTranslationPlane)
        {
            translateOn(chosenTranslationPlane, mousePosition);
            return true;
        }
        else
        {
            if (_lastChosenTranslationPlane)
            {
                _defaultTranslationPlane->plane.origin = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _lastChosenTranslationPlane->plane);
                setupTranslationPlane(_defaultTranslationPlane);
                _lastChosenTranslationPlane = nullptr;
            }

            return translationInputController::onMouseMove(e);
        }
    }

    bool planesTranslationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        for (auto& translationPlane : _translationPlanes)
            deletePlane(translationPlane);

        _translationPlanes.clear();

        return translationInputController::onMouseUp(e);
    }
}