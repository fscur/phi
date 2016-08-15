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
        phi::removeIfContains(_translationPlanes, translationPlane);
        if (_translationPlanes.size() == 0)
            _defaultTranslationPlane->showGrid();
    }

    bool planesTranslationInputController::canTranslateAt(sweepCollision collision)
    {
        auto planePosition = _draggingCollider->getObb().getPositionAt(-collision.normal);
        auto cameraPosition = _camera->getTransform()->getPosition();

        auto toPlaneDir = glm::normalize(planePosition - cameraPosition);

        auto discardDot = glm::abs(glm::dot(collision.normal, toPlaneDir));
        return discardDot > 0.2f;
    }

    vector<sweepCollision> planesTranslationInputController::findTouchCollisions()
    {
        vector<sweepCollision> touchs;

        intersectionCollisionMultiTest touchTest;
        touchTest.colliders = _collisionNodeTranslator->getColliders();
        touchTest.transforms = _collisionNodeTranslator->getTransforms();
        auto touchTestResult = _physicsBehaviour->getPhysicsWorld()->touchs(touchTest);

        if (!touchTestResult.collided)
            return touchs;

        for (auto& collision : touchTestResult.collisions)
        {
            if (canTranslateAt(collision))
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

    void planesTranslationInputController::addPlanesIfNeeded(vector<sweepCollision> touchs)
    {
        for (auto& collision : touchs)
        {
            auto collisionNormal = collision.normal;
            if (existsTranslationPlaneWithNormal(collisionNormal))
                continue;

            auto planePosition = collision.collider->getObb().getPositionAt(collision.normal);
            planePosition = plane(planePosition, collision.normal).projectPoint(_draggingCollider->getObb().center);

            auto plane = phi::plane(vec3(), collision.normal);
            auto translationPlane = createTranslationPlane(plane, planePosition, collision.collider, color(30.0f / 255.0f, 140.0f / 255.0f, 210.0f / 255.0f, 1.0f));
            addTranslationPlane(translationPlane);
            _layer->add(translationPlane->planeGridNode);
            translationPlane->showGrid();
        }
    }

    void planesTranslationInputController::removeDetachedPlanes(vector<sweepCollision> touchs)
    {
        auto translationPlanesCount = _translationPlanes.size();
        for (size_t i = 0; i < translationPlanesCount; ++i)
        {
            auto translationPlane = _translationPlanes[i];
            if (translationPlane == _lastChosenTranslationPlane)
                continue;

            auto translationPlaneNormal = translationPlane->plane.normal;
            auto touchsSearch = std::find_if(touchs.begin(), touchs.end(),
                [translationPlaneNormal](sweepCollision& c)
            {
                return mathUtils::isClose(glm::dot(translationPlaneNormal, c.normal), 1.0f);
            });

            if (touchsSearch == touchs.end())
            {
                deletePlane(translationPlane);
                removeTranslationPlane(translationPlane);
                i--;
                translationPlanesCount--;
            }
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
            auto normalOnDragDirection = glm::dot(translationPlane->plane.normal, dragDirection);
            if (normalOnDragDirection < minNormalOnDragDirection)
            {
                minNormalOnDragDirection = normalOnDragDirection;
                chosenPlane = translationPlane;
            }
        }

        return chosenPlane;
    }

    void planesTranslationInputController::translateOn(translationPlane* translationPlane, ivec2 mousePosition)
    {
        if (translationPlane != _lastChosenTranslationPlane)
        {
            if (_lastChosenTranslationPlane)
                translationPlane->plane.origin = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _lastChosenTranslationPlane->plane);
            else
                translationPlane->plane.origin = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _defaultTranslationPlane->plane);

            setupTranslationPlane(translationPlane);
        }

        auto position = getTranslationPosition(mousePosition, _defaultTranslationPlane);
        auto offset = position - _draggingRootNode->getTransform()->getLocalPosition();

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
        removeDetachedPlanes(touchs);

        auto mousePosition = ivec2(e->x, e->y);
        auto dragDirection = mouseOffsetToWorld(mousePosition);
        auto chosenTranslationPlane = findBestPlaneToDrag(dragDirection);

        if (!chosenTranslationPlane && _lastChosenTranslationPlane)
            chosenTranslationPlane = _lastChosenTranslationPlane;

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