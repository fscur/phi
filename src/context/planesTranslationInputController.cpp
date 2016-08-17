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

    vector<sweepCollision> planesTranslationInputController::findValidTouchCollisions()
    {
        vector<sweepCollision> foundTouchs;
        if (_lastTranslationTouchs)
            foundTouchs = *_lastTranslationTouchs;
        else
        {
            auto sweepTest = sweepCollisionMultiTest();
            sweepTest.colliders = _collisionNodeTranslator->getColliders();
            sweepTest.transforms = _collisionNodeTranslator->getTransforms();
            sweepTest.direction = vec3(1.0f, 0.0f, 0.0f);
            sweepTest.distance = 0.0f;
            sweepTest.inflation = DECIMAL_TRUNCATION;
            sweepTest.disregardDivergentNormals = false;

            auto sweepResult = _physicsBehaviour->getPhysicsWorld()->sweep(sweepTest);
            if (!sweepResult.collided)
                return vector<sweepCollision>();

            foundTouchs = sweepResult.collisions;
        }

        vector<sweepCollision> validTouchs;
        for (auto& collision : foundTouchs)
        {
            if (canTranslateAt(collision.normal))
                validTouchs.push_back(collision);
        }

        return validTouchs;
    }

    bool planesTranslationInputController::existsTranslationPlaneWithNormal(vec3 normal)
    {
        auto translationPlane = std::find_if(_translationPlanes.begin(), _translationPlanes.end(),
            [normal](phi::translationPlane* tp)
        {
            return mathUtils::isClose(glm::dot(normal, tp->getPlane().normal), 1.0f);
        });

        return translationPlane != _translationPlanes.end();
    }

    translationPlane* planesTranslationInputController::createTranslationPlane(plane plane, boxCollider* colidee, boxCollider* collider)
    {
        if (existsTranslationPlaneWithNormal(plane.normal))
            return nullptr;

        auto planePosition = colidee->getObb().getPositionAt(plane.normal);
        planePosition = phi::plane(planePosition, plane.normal).projectPoint(_draggingCollider->getObb().center);

        plane.origin = vec3();
        auto translationPlane = 
            translationInputController::createTranslationPlane(
                plane, 
                planePosition, 
                colidee,
                collider,
                color(30.0f / 255.0f, 140.0f / 255.0f, 210.0f / 255.0f, 1.0f));

        _layer->add(translationPlane->getPlaneGridNode());
        translationPlane->showGrid();

        return translationPlane;
    }

    void planesTranslationInputController::addPlanesIfNeeded(vector<sweepCollision> touchs)
    {
        for (auto& collision : touchs)
        {
            auto translationPlane = createTranslationPlane(plane(vec3(), collision.normal), collision.collider, collision.sourceCollider);
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

            auto translationPlaneNormal = translationPlane->getPlane().normal;
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
            removePlane(translationPlane);
            removeTranslationPlane(translationPlane);
        }
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
            auto plane = translationPlane->getPlane();
            auto normalOnDragDirection = glm::abs(glm::dot(plane.normal, dragDirection));
            if (normalOnDragDirection < minNormalOnDragDirection)
            {
                minNormalOnDragDirection = normalOnDragDirection;
                chosenPlane = translationPlane;
            }
        }

        return chosenPlane;
    }

    bool planesTranslationInputController::isTouchingCollidedObject(vec3 offset, translationPlane* translationPlane, vec3& nearestPosition, plane& touchingPlane)
    {
        auto targetObb = translationPlane->getCollidee()->getObb();
        auto planes = targetObb.getPlanes();

        for (auto& plane : planes)
        {
            if (mathUtils::isClose(glm::dot(plane.normal, translationPlane->getPlane().normal), 1.0f))
                continue;

            //auto collider = translationPlane->sourceCollider;
            auto collider = _draggingCollider;
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

        return true;
    }

    vec3 planesTranslationInputController::checkForPossibleSwitchOfPlanes(vec3 offset, translationPlane* translationPlane)
    {
        if (!translationPlane->getCollidee())
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

        auto createdTranslationPlane = createTranslationPlane(touchingPlane, translationPlane->getCollidee(), translationPlane->getCollider());
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
            auto plane = _lastChosenTranslationPlane ?
                _lastChosenTranslationPlane->getPlane() :
                _defaultTranslationPlane->getPlane();

            auto origin = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, plane);
            auto normal = translationPlane->getPlane().normal;

            translationPlane->setPlane(phi::plane(origin, normal));

            setupTranslationPlane(translationPlane);
        }

        auto position = getTranslationPosition(mousePosition, translationPlane);
        auto offset = position - _draggingRootNode->getTransform()->getLocalPosition();
        offset = checkForPossibleSwitchOfPlanes(offset, translationPlane);

        translateNode(offset);
        translatePlaneGrid(translationPlane);
        translateGhost(position, offset);

        _lastChosenTranslationPlane = translationPlane;
        _lastMousePosition = mousePosition;
    }

    bool planesTranslationInputController::onMouseDown(mouseEventArgs * e)
    {
        translationInputController::onMouseDown(e);

        if (!_dragging)
            return false;

        _lastChosenTranslationPlane = nullptr;

        auto touchs = findValidTouchCollisions();
        addPlanesIfNeeded(touchs);

        _isTouchingCollidedObject = true;
        _isSwitchingPlanes = false;

        return true;
    }

    bool planesTranslationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        auto touchs = findValidTouchCollisions();
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
                _defaultTranslationPlane->getPlane().origin = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _lastChosenTranslationPlane->getPlane());
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
            removePlane(translationPlane);

        _translationPlanes.clear();

        return translationInputController::onMouseUp(e);
    }

    bool planesTranslationInputController::update()
    {
        translationInputController::update();
        return true;
    }
}