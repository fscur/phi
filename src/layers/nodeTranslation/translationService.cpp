#include <precompiled.h>
#include "translationService.h"
#include <core/boxCollider.h>
#include <animation/animator.h>
#include <core/plane.h>
#include <physics/sweepCollision.h>
#include <physics/intersectionCollisionMultiTest.h>
#include <physics/sweepCollisionMultiTest.h>
#include <core/ghostMesh.h>
#include <core/mesh.h>
#include <core/string.h>

namespace phi
{
    translationService::translationService(
        const vector<node*>* targetNodes,
        layer* layer,
        physicsWorld* physicsWorld) :
        _targetNodes(targetNodes),
        _layer(layer),
        _camera(layer->getCamera()),
        _translationPlanes(vector<translationPlane*>()),
        _isTranslating(false),
        _canChangePlanes(false),
        _planesToRemove(vector<translationPlane*>()),
        _nodeTranslator(new collisionNodeTranslator(physicsWorld)),
        _ghostTranslator(new ghostNodeTranslator(layer))
    {
    }

    translationService::~translationService()
    {
    }

    vec3 translationService::getClosestAxisTo(vec3 direction)
    {
        vec3 axisNormals[] =
        {
            vec3(1.0f, 0.0f, 0.0f),
            vec3(0.0f, 1.0f, 0.0f),
            vec3(0.0f, 0.0f, 1.0f),
            vec3(-1.0f, 0.0f, 0.0f),
            vec3(0.0f, -1.0f, 0.0f),
            vec3(0.0f, 0.0f, -1.0f)
        };

        auto maxDot = -20.0f;
        auto maxNormal = vec3();
        for (uint_fast8_t i = 0; i < 6; ++i)
        {
            auto dot = glm::dot(direction, axisNormals[i]);
            if (dot > maxDot)
            {
                maxDot = dot;
                maxNormal = axisNormals[i];
            }
        }

        assert(maxNormal != vec3());

        return glm::normalize(maxNormal);
    }

    plane translationService::createPlaneFromAxis(vec3 axis)
    {
        auto minDot = std::numeric_limits<float>().max();
        auto planeOrigin = vec3();

        for (auto& targetNode : *_targetNodes)
        {
            auto collider = targetNode->getComponent<boxCollider>();
            assert(collider != nullptr);

            auto obb = collider->getObb();
            auto obbPosition = obb.getPositionAt(-axis);

            auto dot = glm::dot(obbPosition, axis);
            if (dot < minDot)
            {
                minDot = dot;
                planeOrigin = obbPosition;
            }
        }

        return plane(planeOrigin, axis);
    }

    translationPlane* translationService::createAxisAlignedTranslationPlane(ivec2 mousePosition)
    {
        auto viewDirection = -_camera->screenPointToRay(mousePosition.x, mousePosition.y).getDirection();
        auto worldAxis = getClosestAxisTo(viewDirection);
        auto axisPlane = createPlaneFromAxis(worldAxis);

        auto worldPosition = _camera->screenPointToWorld(mousePosition.x, mousePosition.y);
        auto origin = axisPlane.projectPoint(worldPosition);

        return createTranslationPlane(plane(origin, axisPlane.normal));
    }

    translationPlane* translationService::createTranslationPlane(plane plane)
    {
        auto planeGridNode = new node("plane");
        auto planeTransform = planeGridNode->getTransform();

        planeTransform->setLocalPosition(plane.origin);
        planeTransform->setDirection(plane.normal);

        auto animator = new phi::animator();

        auto planeGrid = new phi::planeGrid();
        //planeGrid->setColor(color::fromRGBA(0.7f, 0.8f, 0.9f, 1.0f));
        planeGrid->setColor(color::fromRGBA(0.0f, 0.8f, 0.2f, 1.0f));
        planeGrid->setLineThickness(8.5f);
        planeGridNode->addComponent(planeGrid);
        planeGridNode->addComponent(animator);

        auto draggingAnimation = new phi::transformAnimation(planeTransform, easingFunctions::easeOutCubic);
        animator->addAnimation(draggingAnimation);

        auto fadeUpdadeFunction = [=](float value)
        {
            planeGrid->setOpacity(value);
        };

        auto fadeInAnimation = new phi::floatAnimation();
        fadeInAnimation->setUpdateFunction(fadeUpdadeFunction);
        animator->addAnimation(fadeInAnimation);

        auto fadeOutAnimation = new phi::floatAnimation();
        fadeOutAnimation->setUpdateFunction(fadeUpdadeFunction);
        animator->addAnimation(fadeOutAnimation);

        auto clippingPlanesFadeOutAnimation = new phi::floatAnimation();
        animator->addAnimation(clippingPlanesFadeOutAnimation);

        auto translationPlane = new phi::translationPlane(plane);
        translationPlane->setCollidee(nullptr);
        translationPlane->setCollider(nullptr);
        translationPlane->setPlaneGridNode(planeGridNode);
        translationPlane->setDraggingAnimation(draggingAnimation);
        translationPlane->setFadeInAnimation(fadeInAnimation);
        translationPlane->setFadeOutAnimation(fadeOutAnimation);
        translationPlane->setClippingPlanesFadeAnimation(clippingPlanesFadeOutAnimation);
        translationPlane->setExtinctionFactor(getExtinctionFactor(plane.normal));

        return translationPlane;
    }

    void translationService::addTranslationPlane(translationPlane* translationPlane)
    {
        _layer->add(translationPlane->getPlaneGridNode());
        _translationPlanes.push_back(translationPlane);
        translationPlane->showGrid();
    }

    void translationService::enqueuePlaneForRemoval(translationPlane* planeToRemove)
    {
        auto fadeOutAnimationEnded = [=]
        {
            _planesToRemove.push_back(planeToRemove);
        };

        planeToRemove->fadeGridOpacityOut(fadeOutAnimationEnded);
    }

    void translationService::removePlanesIf(std::function<bool(translationPlane*)> predicate)
    {
        auto removeFunction = [&](translationPlane* translationPlane)
        {
            bool shouldRemove = predicate(translationPlane);

            if (shouldRemove)
            {
                _offsetPlanes.erase(translationPlane);
                safeDelete(_offsetPlanes[translationPlane]);
                enqueuePlaneForRemoval(translationPlane);
            }

            return shouldRemove;
        };

        _translationPlanes.erase(std::remove_if(_translationPlanes.begin(), _translationPlanes.end(), removeFunction), _translationPlanes.end());
    }

    void translationService::removeInvalidPlanes()
    {
        removePlanesIf(
            [&](translationPlane* translationPlane) 
        { 
            return !isPlaneVisible(translationPlane->getPlane(), translationPlane->getExtinctionfactor()); 
        });

        /*vector<translationPlane*> planesToRemove;

        for (auto& translationPlane : _translationPlanes)
        {
            if (!isPlaneVisible(translationPlane->getPlane(), translationPlane->getExtinctionfactor()))
                planesToRemove.push_back(translationPlane);
        }

        for (auto& planeToRemove : planesToRemove)
        {
            removeIfContains(_translationPlanes, planeToRemove);
            _offsetPlanes.erase(planeToRemove);
            safeDelete(_offsetPlanes[planeToRemove]);
            enqueuePlaneForRemoval(planeToRemove);
        }*/
    }

    void translationService::deleteRemovedPlanes()
    {
        for (auto& translationPlane : _planesToRemove)
        {
            translationPlane->hideGrid();
            auto planeGridNode = translationPlane->getPlaneGridNode();
            planeGridNode->getParent()->removeChild(planeGridNode);
            safeDelete(translationPlane);
        }

        _planesToRemove.clear();
    }

    void translationService::updateTranslationPlanesVisibility()
    {
        for (auto& translationPlane : _translationPlanes)
        {
            auto visibility = getPlaneVisibility(translationPlane->getPlane());
            translationPlane->updatePlaneGridVisibility(visibility);
        }
    }

    bool translationService::isPlaneValidForAddition(plane plane)
    {
        if (!isPlaneVisible(plane, getExtinctionFactor(plane.normal)))
            return false;

        for (auto& translationPlane : _translationPlanes)
            if (plane.isParallel(translationPlane->getPlane()))
                return false;

        return true;
    }

    bool translationService::isPlaneVisible(plane plane, float planeExtinctionFactor)
    {
        auto planeVisibility = getPlaneVisibility(plane);
        return planeVisibility > planeExtinctionFactor;
    }

    float translationService::getExtinctionFactor(vec3 normal)
    {
        return mathUtils::isParallel(vec3(0.0, 1.0, 0.0), normal) ? 0.2f : 0.5f;
    }

    float translationService::getPlaneVisibility(plane plane)
    {
        auto cameraTransform = _camera->getTransform();
        auto cameraPosition = cameraTransform->getPosition();
        auto toPlaneDir = -glm::normalize(plane.origin - cameraPosition);
        return glm::dot(plane.normal, toPlaneDir);
    }

    void translationService::addValidPlanesFromTouchCollisions()
    {
        vector<plane> validPlanesFromTouchs;

        for (auto& touch : *_lastTouchingCollisions)
        {
            auto touchPlaneNormal = getClosestAxisTo(touch.normal);
            auto touchPlaneOrigin = touch.collider->getObb().getPositionAt(-touchPlaneNormal);
            auto touchPlane = plane(touchPlaneOrigin, touchPlaneNormal);

            touchPlane.origin = touchPlane.projectPoint(_currentOffsetPlane->origin);

            bool isTouchPlaneParallelToAnyOtherTouchPlane = false;
            for (auto& validPlane : validPlanesFromTouchs)
            {
                if (touchPlane.isParallel(validPlane))
                {
                    isTouchPlaneParallelToAnyOtherTouchPlane = true;
                    break;
                }
            }

            if (!isTouchPlaneParallelToAnyOtherTouchPlane && isPlaneValidForAddition(touchPlane))
            {
                validPlanesFromTouchs.push_back(touchPlane);
                auto translationPlane = createTranslationPlane(touchPlane);
                _offsetPlanes[translationPlane] = new plane(_offsetPlaneOrigin, touchPlane.normal);
                addTranslationPlane(translationPlane);
            }
        }
    }

    void translationService::changePlanes(translationPlane* translationPlane, plane* offsetPlane)
    {
        _currentOffsetPlane = offsetPlane;
        _currentTranslationPlane = translationPlane;
        _nodeTranslator->setPlane(_currentOffsetPlane);
    }

    void translationService::changePlanesIfNeeded(vec3& endPosition)
    {
        bool changedPlane = false;

        for (auto& translationPlane : _translationPlanes)
        {
            if (_currentTranslationPlane == translationPlane)
                continue;

            auto offsetPlane = _offsetPlanes[translationPlane];
            endPosition = offsetPlane->projectPoint(endPosition);

            changePlanes(translationPlane, offsetPlane);
            changedPlane = true;
            break;
        }

        if (changedPlane)
        {
            removePlanesIf(
                [&](translationPlane* translationPlane)
                {
                    return translationPlane != _currentTranslationPlane;
                });

            /*vector<translationPlane*> planesToRemove;

            for (auto& translationPlane : _translationPlanes)
            {
            if (translationPlane == _currentTranslationPlane)
            continue;

            planesToRemove.push_back(translationPlane);
            }

            for (auto& planeToRemove : planesToRemove)
            {
            removeIfContains(_translationPlanes, planeToRemove);
            _offsetPlanes.erase(planeToRemove);
            enqueuePlaneForRemoval(planeToRemove);
            }*/
        }
    }

    void translationService::translateTargetNodes(vec3 endPosition)
    {
        auto offset = endPosition - _offsetPlaneOrigin;

        if (offset == vec3())
            return;

        _ghostTranslator->translate(offset);

        offset = _nodeTranslator->translate(offset);

        _offsetPlaneOrigin += offset;
        _currentOffsetPlane->origin = _offsetPlaneOrigin;
    }

    void translationService::translatePlaneGrid(vec3 endPosition)
    {
        //auto planeGridPosition = _currentTranslationPlane->getPlane().projectPoint(endPosition);
        //_currentTranslationPlane->animatePlaneGridPosition(planeGridPosition);

        for (auto& translationPlane : _translationPlanes)
        {
            auto planeGridPosition = translationPlane->getPlane().projectPoint(endPosition);
            translationPlane->animatePlaneGridPosition(planeGridPosition);
        }
    }

    void translationService::startTranslation(ivec2 mousePosition)
    {
        for (auto& node : *_targetNodes)
            _originalPositions[node] = node->getTransform()->getLocalPosition();

        _nodeTranslator->addRange(*_targetNodes);
        _lastTouchingCollisions = _nodeTranslator->getLastTranslationTouchingCollisions();

        _ghostTranslator->addRange(*_targetNodes);

        _isTranslating = true;
        _lastMousePosition = mousePosition;
        _offsetPlaneOrigin = _camera->screenPointToWorld(mousePosition.x, mousePosition.y);
    }

    void translationService::translate(ivec2 mousePosition)
    {
        if (!_isTranslating)
            return;

        auto endPosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, *_currentOffsetPlane);

        if (_canChangePlanes)
        {
            addValidPlanesFromTouchCollisions();
            changePlanesIfNeeded(endPosition);
        }

        translateTargetNodes(endPosition);
        translatePlaneGrid(endPosition);

        if (_nodeTranslator->getLastTranslationTouchingCollisions()->size() > 0)
            _ghostTranslator->enable();
        else
            _ghostTranslator->disable();

        _lastMousePosition = mousePosition;
    }

    void translationService::endTranslation()
    {
        _isTranslating = false;

        for (auto& translationPlane : _translationPlanes)
            enqueuePlaneForRemoval(translationPlane);

        _translationPlanes.clear();
        _offsetPlanes.clear();

        _ghostTranslator->clear();
        _nodeTranslator->clear();
        _nodeTranslator->enableCollisions();
    }

    void translationService::cancelTranslation()
    {
        for (auto& node : *_targetNodes)
            node->getTransform()->setLocalPosition(_originalPositions[node]);
    }

    void translationService::update()
    {
        deleteRemovedPlanes();

        if (!_isTranslating)
            return;

        updateTranslationPlanesVisibility();
        removeInvalidPlanes();

        if (_translationPlanes.size() == 0)
        {
            auto translationPlane = createAxisAlignedTranslationPlane(_lastMousePosition);
            addTranslationPlane(translationPlane);
            auto offsetPlane = new plane(_offsetPlaneOrigin, translationPlane->getPlane().normal);
            _offsetPlanes[translationPlane] = offsetPlane;
            changePlanes(translationPlane, offsetPlane);
        }
    }

    void translationService::disableCollisions()
    {
        _nodeTranslator->disableCollisions();
    }

    void translationService::enableCollisions()
    {
        _nodeTranslator->enableCollisions();
    }

    void translationService::disablePlaneChanges()
    {
        _canChangePlanes = true;
    }

    void translationService::enablePlaneChanges()
    {
        _canChangePlanes = false;
    }

    /*

    vector<sweepCollision> translationService::findTouchingCollisions()
    {
    auto sweepTest = sweepCollisionMultiTest();
    sweepTest.colliders = _nodeTranslator->getColliders();
    sweepTest.transforms = _nodeTranslator->getTransforms();
    sweepTest.direction = vec3(1.0f, 0.0f, 0.0f);
    sweepTest.distance = 0.0f;
    sweepTest.inflation = DECIMAL_TRUNCATION;
    sweepTest.disregardDivergentNormals = false;

    auto sweepResult = _physicsWorld->sweep(sweepTest);
    if (!sweepResult.collided)
    return vector<sweepCollision>();

    return sweepResult.collisions;
    }

    vector<sweepCollision> translationService::getValidTouchCollisions(vector<sweepCollision>& touchs)
    {
    vector<sweepCollision> validTouchs;
    //for (auto& collision : touchs)
    //{
    //if (canTranslateAt(collision.normal))
    //validTouchs.push_back(collision);
    //}

    return validTouchs;
    }

    translationPlane* translationService::createTranslationPlane(
        plane plane,
        boxCollider* colidee,
        boxCollider* collider,
        clippingDistance::clippingDistance clippingDistance)
    {
        if (existsTranslationPlaneWithNormal(plane.normal))
            return nullptr;

        auto planePosition = colidee->getObb().getPositionAt(plane.normal);
        auto castPosition = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _currentTranslationPlane->getMousePlane());
        planePosition = phi::plane(planePosition, plane.normal).projectPoint(castPosition);

        auto createdTranslationPlane = createTranslationPlane(planePosition, vec3(0.0, 1.0, 0.0));

        //auto createdTranslationPlane =
        //translationInputController::createTranslationPlane(
        //plane,
        //planePosition,
        //colidee,
        //collider,
        //color(30.0f / 255.0f, 140.0f / 255.0f, 210.0f / 255.0f, 1.0f));

        //if (_translationPlanes.size() > 0)
        createClippingPlanes(createdTranslationPlane, clippingDistance);

        _layer->add(createdTranslationPlane->getPlaneGridNode());
        createdTranslationPlane->showGrid();

        return createdTranslationPlane;
    }

    void translationService::createClippingPlanes(
        translationPlane* clippingTranslationPlane,
        clippingDistance::clippingDistance clippingDistance)
    {
        auto createdTranslationPlaneGrid = clippingTranslationPlane->getPlaneGridComponent();
        auto clippingPlane = new phi::clippingPlane(clippingTranslationPlane->getGridPlane());
        clippingTranslationPlane->setClippingPlane(clippingPlane);

        for (auto& clippedTranslationPlane : _translationPlanes)
        {
            auto clippedPlane = clippedTranslationPlane->getClippingPlane();
            createdTranslationPlaneGrid->addClippingPlane(clippedPlane);
            createdTranslationPlaneGrid->setClippingPlaneDistance(clippedPlane, clippingDistance);

            auto planeGridComponent = clippedTranslationPlane->getPlaneGridComponent();
            planeGridComponent->addClippingPlane(clippingPlane);
            planeGridComponent->setClippingPlaneDistance(clippingPlane, clippingDistance);
        }
    }

    void translationService::removeClippingPlanes(translationPlane* planeToRemove)
    {
        for (auto& translationPlane : _translationPlanes)
        {
            auto planeGrid = translationPlane->getPlaneGridNode()->getComponent<phi::planeGrid>();
            auto clippingPlane = planeToRemove->getClippingPlane();

            planeGrid->removeClippingPlane(clippingPlane);
        }
    }

    void translationService::addPlanesIfNeeded(vector<sweepCollision> touchs)
    {
        for (auto& collision : touchs)
        {
            auto castPosition =
                _camera->castRayToPlane(
                    _lastMousePosition.x,
                    _lastMousePosition.y,
                    _lastChosenTranslationPlane->getMousePlane());

            auto translationPlane =
                createTranslationPlane(
                    plane(castPosition, collision.normal),
                    collision.collidee,
                    collision.collider,
                    clippingDistance::positive);

            if (translationPlane)
                _translationPlanes.push_back(translationPlane);
        }
    }



    bool translationService::existsTranslationPlaneWithNormal(vec3 normal)
    {
        auto translationPlane = std::find_if(_translationPlanes.begin(), _translationPlanes.end(),
            [normal](phi::translationPlane* tp)
        {
            return mathUtils::isClose(glm::dot(normal, tp->getMousePlane().normal), 1.0f);
        });

        return translationPlane != _translationPlanes.end();
    }

    bool translationService::isDraggingObjectIntersectingAnyObject()
    {
        auto intersectionTest = intersectionCollisionMultiTest();
        intersectionTest.colliders = _nodeTranslator->getColliders();
        intersectionTest.transforms = _nodeTranslator->getTransforms();

        return _physicsWorld->intersects(intersectionTest);
    }
    */
}