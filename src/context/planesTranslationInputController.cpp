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

        auto howMuchOfTheSurfaceTheCameraIsSeeing = glm::dot(normal, toPlaneDir);
        auto isCameraSeeingSurface = howMuchOfTheSurfaceTheCameraIsSeeing < -0.2f;
        
        return isCameraSeeingSurface;
    }

    bool planesTranslationInputController::isDraggingObjectIntersectingAnyObject()
    {
        auto intersectionTest = intersectionCollisionMultiTest();
        intersectionTest.colliders = _collisionNodeTranslator->getColliders();
        intersectionTest.transforms = _collisionNodeTranslator->getTransforms();

        return _physicsBehaviour->getPhysicsWorld()->intersects(intersectionTest);
    }

    vector<sweepCollision> planesTranslationInputController::findTouchingCollisions()
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

        return sweepResult.collisions;
    }

    vector<sweepCollision> planesTranslationInputController::getValidTouchCollisions(vector<sweepCollision>& touchs)
    {
        vector<sweepCollision> validTouchs;
        for (auto& collision : touchs)
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
            return mathUtils::isClose(glm::dot(normal, tp->getMousePlane().normal), 1.0f);
        });

        return translationPlane != _translationPlanes.end();
    }

    void planesTranslationInputController::createClippingPlanes(translationPlane* clippingTranslationPlane, clippingDistance::clippingDistance clippingDistance)
    {
        auto createdTranslationPlaneGrid = clippingTranslationPlane->getPlaneGridComponent();
        auto clippingPlane = new phi::clippingPlane(clippingTranslationPlane->getGridPlane(), clippingDistance);
        clippingTranslationPlane->setClippingPlane(clippingPlane);

        for (auto& clippedTranslationPlane : _translationPlanes)
        {
            auto clippedPlane = clippedTranslationPlane->getClippingPlane();
            clippedPlane->distance = clippingDistance;
            createdTranslationPlaneGrid->addClippingPlane(clippedPlane);

            auto planeGridComponent = clippedTranslationPlane->getPlaneGridComponent();
            planeGridComponent->addClippingPlane(clippingPlane);
        }
    }

    translationPlane* planesTranslationInputController::createTranslationPlane(plane plane, boxCollider* colidee, boxCollider* collider, clippingDistance::clippingDistance clippingDistance)
    {
        if (existsTranslationPlaneWithNormal(plane.normal))
            return nullptr;

        auto planePosition = colidee->getObb().getPositionAt(plane.normal);
        auto castPosition = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _defaultTranslationPlane->getMousePlane());
        planePosition = phi::plane(planePosition, plane.normal).projectPoint(castPosition);

        auto createdTranslationPlane =
            translationInputController::createTranslationPlane(
                plane,
                planePosition,
                colidee,
                collider,
                color(30.0f / 255.0f, 140.0f / 255.0f, 210.0f / 255.0f, 1.0f));

        //if (_translationPlanes.size() > 0)
        createClippingPlanes(createdTranslationPlane, clippingDistance);

        _layer->add(createdTranslationPlane->getPlaneGridNode());
        createdTranslationPlane->showGrid();

        return createdTranslationPlane;
    }

    void planesTranslationInputController::addPlanesIfNeeded(vector<sweepCollision> touchs)
    {
        for (auto& collision : touchs)
        {
            auto castPosition = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _lastChosenTranslationPlane->getMousePlane());
            auto translationPlane =
                createTranslationPlane(
                    plane(castPosition, collision.normal),
                    collision.collidee,
                    collision.collider,
                    clippingDistance::positive);

            if (translationPlane)
                addTranslationPlane(translationPlane);
        }
    }

    void planesTranslationInputController::removeClippingPlanes(translationPlane* planeToRemove)
    {
        for (auto& translationPlane : _translationPlanes)
        {
            auto planeGrid = translationPlane->getPlaneGridNode()->getComponent<phi::planeGrid>();
            auto clippingPlane = planeToRemove->getClippingPlane();

            planeGrid->removeClippingPlane(clippingPlane);
        }

        enqueuePlaneForRemoval(planeToRemove);
    }

    void planesTranslationInputController::enqueuePlaneForRemoval(translationPlane* planeToRemove)
    {
        auto fadeOutAnimationEnded = [=]
        {
            _planesToRemove.push_back(planeToRemove);
        };

        planeToRemove->fadeGridOpacityOut(fadeOutAnimationEnded);
    }

    void planesTranslationInputController::removeDetachedPlanes(vector<sweepCollision> touchs)
    {
        vector<translationPlane*> translationPlanesToRemove;

        for (auto& translationPlane : _translationPlanes)
        {
            if (translationPlane == _lastChosenTranslationPlane)
                continue;

            auto translationPlaneNormal = translationPlane->getMousePlane().normal;
            auto touchsSearch = std::find_if(touchs.begin(), touchs.end(),
                [translationPlaneNormal](sweepCollision& c)
            {
                return mathUtils::isClose(glm::dot(translationPlaneNormal, c.normal), 1.0f);
            });

            if (touchsSearch == touchs.end())
                translationPlanesToRemove.push_back(translationPlane);
        }

        for (auto planeToRemove : translationPlanesToRemove)
        {
            removeTranslationPlane(planeToRemove);
            removeClippingPlanes(planeToRemove);
            //enqueuePlaneForRemoval(planeToRemove);
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
        auto collidedOnLastTranslation = _lastTranslationTouchs && _lastTranslationTouchs->size() > 0.0f;
        auto hasMoreThanOneChoiceOfPlanes = _translationPlanes.size() > 1;

        auto minNormalOnDragDirection = 1.0f;
        translationPlane* chosenPlane = nullptr;

        for (auto& translationPlane : _translationPlanes)
        {
            bool isLastTranslationPlane = translationPlane == _lastChosenTranslationPlane;

            if (isLastTranslationPlane &&
                hasMoreThanOneChoiceOfPlanes &&
                collidedOnLastTranslation)
                continue;

            auto plane = translationPlane->getMousePlane();
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

        auto draggingColliderObb = _draggingCollider->getObb();
        auto obbCorners = draggingColliderObb.getCorners();
        auto cornersCount = obbCorners.size();

        for (auto& plane : planes)
        {
            if (mathUtils::isClose(glm::dot(plane.normal, translationPlane->getMousePlane().normal), 1.0f))
                continue;

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

    vec3 planesTranslationInputController::checkForPossibleSwitchOfPlanes(vec3 offset, translationPlane* translationPlane, ivec2 mousePosition)
    {
        if (_disableCollision)
            return offset;

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
        _switchPlanesMousePosition = mousePosition;

        auto castPosition = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, translationPlane->getMousePlane());
        auto createdTranslationPlane = 
            createTranslationPlane(
                plane(castPosition, touchingPlane.normal), 
                translationPlane->getCollidee(), 
                translationPlane->getCollider(), 
                clippingDistance::negative);

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
        if (_isSwitchingPlanes)
        {
            auto difference = vec2(mousePosition) - vec2(_switchPlanesMousePosition);
            if (length(difference) < 50.0f)
                return;

            _isSwitchingPlanes = false;
        }

        if (translationPlane != _lastChosenTranslationPlane)
            setupTranslationPlane(translationPlane);

        auto position = getTranslationPosition(mousePosition, translationPlane);

        auto currentPosition = _draggingRootNode->getTransform()->getLocalPosition();
        auto offset = position - currentPosition;

        offset = checkForPossibleSwitchOfPlanes(offset, translationPlane, mousePosition);

        translateNode(offset);
        translatePlaneGrid(translationPlane, mousePosition);

        if (_lastTranslationTouchs->size() > 0)
            showGhost();
        else
            hideGhost();

        translateGhost(currentPosition + offset, offset);

        _lastChosenTranslationPlane = translationPlane;
    }

    void planesTranslationInputController::changeToDefaultTranslationPlane()
    {
        for (auto& translationPlane : _translationPlanes)
            enqueuePlaneForRemoval(translationPlane);

        _translationPlanes.clear();

        _defaultTranslationPlane->showGrid();

        if (_lastChosenTranslationPlane != _defaultTranslationPlane)
        {
            auto castPosition = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _lastChosenTranslationPlane->getMousePlane());
            auto defaultPlane = _defaultTranslationPlane->getMousePlane();
            auto gridPlane = phi::plane(_draggingCollider->getObb().getPositionAt(-defaultPlane.normal), defaultPlane.normal);
            auto gridPosition = gridPlane.projectPoint(castPosition);
            _defaultTranslationPlane->getPlaneGridNode()->getTransform()->setLocalPosition(gridPosition);

            _defaultTranslationPlane->setMousePlane(plane(castPosition, defaultPlane.normal));
            setupTranslationPlane(_defaultTranslationPlane);
            _lastChosenTranslationPlane = _defaultTranslationPlane;
        }
    }

    bool planesTranslationInputController::onMouseDown(mouseEventArgs * e)
    {
        auto baseResult = translationInputController::onMouseDown(e);

        if (!_dragging)
            return false;

        _isTouchingCollidedObject = true;
        _isSwitchingPlanes = false;
        _lastChosenTranslationPlane = _defaultTranslationPlane;

        if (isDraggingObjectIntersectingAnyObject())
            return baseResult;

        auto touchs = getValidTouchCollisions(findTouchingCollisions());
        addPlanesIfNeeded(touchs);

        return true;
    }

    bool planesTranslationInputController::executeMouseMove(ivec2 mousePosition)
    {
        if (!_dragging)
            return false;

        if (!_disableCollision && isDraggingObjectIntersectingAnyObject())
        {
            changeToDefaultTranslationPlane();
            return false;
        }

        auto dragDirection = mouseOffsetToWorld(mousePosition);
        auto chosenTranslationPlane = findBestPlaneToDrag(dragDirection);

        if (chosenTranslationPlane)
            translateOn(chosenTranslationPlane, mousePosition);
        else
            changeToDefaultTranslationPlane();

        auto touchs = getValidTouchCollisions(*_lastTranslationTouchs);
        
        if (!_isSwitchingPlanes)
            removeDetachedPlanes(touchs);

        if (!_disableCollision)
            addPlanesIfNeeded(touchs);

        if (chosenTranslationPlane)
            return true;
        else
            return false;
    }

    bool planesTranslationInputController::onMouseMove(mouseEventArgs* e)
    {
        auto mousePosition = ivec2(e->x, e->y);
        auto result = executeMouseMove(mousePosition);
        if (!result)
            translationInputController::onMouseMove(e);

        _lastMousePosition = mousePosition;
        return true;
    }

    bool planesTranslationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        for (auto& translationPlane : _translationPlanes)
            enqueuePlaneForRemoval(translationPlane);

        _translationPlanes.clear();

        return translationInputController::onMouseUp(e);
    }

    bool planesTranslationInputController::update()
    {
        for (auto& plane : _planesToRemove)
        {
            deletePlane(plane);
            plane = nullptr;
        }

        _planesToRemove.clear();

        return true;
    }
}