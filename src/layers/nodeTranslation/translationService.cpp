#include <precompiled.h>
#include "translationService.h"
#include <core/boxCollider.h>
#include <animation/animator.h>
#include <core/plane.h>
#include <physics/sweepCollision.h>
#include <physics/intersectionCollisionMultiTest.h>
#include <physics/intersectionCollisionPairTest.h>
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
        _physicsWorld(physicsWorld),
        _translationPlanes(vector<translationPlane*>()),
        _isTranslating(false),
        _canChangePlanes(true),
        _lastVisiblePlane(nullptr),
        _planesToRemove(vector<translationPlane*>()),
        _nodeTranslator(new collisionNodeTranslator(physicsWorld)),
        _ghostTranslator(new ghostNodeTranslator(layer))
    {
    }

    translationService::~translationService()
    {
    }

    plane translationService::getClosestPlaneToDirection(vector<plane>& planes, vec3 direction)
    {
        auto maxDot = -20.0f;
        auto closestPlane = plane();
        for (auto& plane : planes)
        {
            auto dot = glm::dot(direction, plane.normal);
            if (dot > maxDot)
            {
                maxDot = dot;
                closestPlane = plane;
            }
        }

        return closestPlane;
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
            auto obb = targetNode->getObb();
            auto obbPosition = obb->getPositionAt(-axis);

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
        planeGrid->setColor(color::fromRGBA(0.3f, 0.5f, 0.8f, 1.0f));
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

    void translationService::addAxisAlignedTranslationPlane()
    {
        auto translationPlane = createAxisAlignedTranslationPlane(_lastMousePosition);
        addTranslationPlane(translationPlane);
        auto offsetPlane = new plane(_offsetPlaneOrigin, translationPlane->getPlane().normal);
        _offsetPlanes[translationPlane] = offsetPlane;
        changePlanes(translationPlane, offsetPlane);
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
        return mathUtils::isParallel(vec3(0.0, 1.0, 0.0), normal) ? 0.3f : 0.4f;
    }

    float translationService::getPlaneVisibility(plane plane)
    {
        auto cameraTransform = _camera->getTransform();
        auto cameraPosition = cameraTransform->getPosition();
        auto toPlaneDir = -glm::normalize(plane.origin - cameraPosition);
        return glm::dot(plane.normal, toPlaneDir);
    }

    void translationService::addPlaneIfNeeded()
    {
        if (_translationPlanes.size() == 0)
        {
            translationPlane* translationPlane = nullptr;

            //if (_lastVisiblePlane && isPlaneVisible(*_lastVisiblePlane, getExtinctionFactor(_lastVisiblePlane->normal)))
            //    translationPlane = createTranslationPlane(*_lastVisiblePlane);
            //else
            translationPlane = createAxisAlignedTranslationPlane(_lastMousePosition);

            addTranslationPlane(translationPlane);
            auto offsetPlane = new plane(_offsetPlaneOrigin, translationPlane->getPlane().normal);
            _offsetPlanes[translationPlane] = offsetPlane;
            changePlanes(translationPlane, offsetPlane);
        }
    }

    bool translationService::isNormalValidForCollision(const sweepCollision& touch, const vec3& normal)
    {
        bool isNormalValid = false;
        for (auto& collideePlane : touch.collidee->getObb().getPlanes())
        {
            if (mathUtils::isClose(glm::dot(collideePlane.normal, touch.normal), 1.0f))
            {
                isNormalValid = true;
                break;
            }
        }

        for (auto& colliderPlane : touch.collider->getObb().getPlanes())
        {
            if (mathUtils::isClose(glm::dot(colliderPlane.normal, -touch.normal), 1.0f))
            {
                isNormalValid = true;
                break;
            }
        }

        return isNormalValid;
    }

    void translationService::addValidPlanesFromTouchCollisions()
    {
        vector<plane> validPlanesFromTouchs;

        for (auto& touch : *_lastTouchingCollisions)
        {
            auto touchPlaneNormal = touch.normal;
            auto touchPlanePosition = touch.position;
            
            if (!isNormalValidForCollision(touch, touchPlaneNormal))
            {
                //debug("pulou normal");
                continue;
            }

            //auto touchPlane2 = getClosestPlaneToPosition(touch.collidee->getObb().getPlanes(), touchPlanePosition);

        /*    auto f = glm::dot(touchPlane2.normal, -touch.normal);
            if (mathUtils::isClose(f, 0.0f))
            {
                touchPlaneNormal = touch.normal;
            }
            else
            {
                touchPlaneNormal = touchPlane2.normal;
            }*/

            auto touchPlaneOrigin = touch.collidee->getObb().getPositionAt(touchPlaneNormal);

            //auto touchPlaneOrigin = touch.collider->getObb().getPositionAt(-touchPlaneNormal);
            auto touchPlane = phi::plane(touchPlaneOrigin, touchPlaneNormal);

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
        _lastVisiblePlane = new plane(_currentTranslationPlane->getPlane());
    }

    bool translationService::changePlanesIfNeeded()
    {
        bool changedPlane = false;

        for (auto& translationPlane : _translationPlanes)
        {
            if (_currentTranslationPlane == translationPlane)
                continue;

            auto offsetPlane = _offsetPlanes[translationPlane];

            changePlanes(translationPlane, offsetPlane);
            changedPlane = true;
            break;
        }

        if (changedPlane)
        {
            _ghostTranslator->disable();

            removePlanesIf(
                [&](translationPlane* translationPlane)
                {
                    return translationPlane != _currentTranslationPlane;
                });
        }

        return changedPlane;
    }

    bool translationService::changeToAttachedPlane(ivec2 mousePosition)
    {
        bool changedPlane = false;
        bool isLeavingPlane = false;
        bool deleteCollisions = false;
        float maxDistance = std::numeric_limits<float>().lowest();
        float leavingDistance = -1.0f;
        plane leavingPlane;

        for (auto& touch : _currentCollisions)
        {

            auto node = touch.collider->getNode();
            while (node->getParent()->getParent() != nullptr) node = node->getParent();
            auto colliderObb = node->getObb();

            auto collideePlanes = touch.collidee->getObb().getPlanes();

            for (auto& collideePlane : collideePlanes)
            {
                if (collideePlane.isParallel(*_currentOffsetPlane))
                    continue;

                auto distanceFromPlane = glm::dot(collideePlane.toVec4(), vec4(colliderObb->center, 1.0f));
                if (distanceFromPlane > maxDistance)
                {
                    maxDistance = distanceFromPlane;
                    leavingPlane = collideePlane;
                    vec4 leavingPos = vec4(colliderObb->getPositionAt(-leavingPlane.normal), 1.0);
                    leavingDistance = glm::dot(leavingPlane.toVec4(), leavingPos);

                    if (leavingDistance > DECIMAL_TRUNCATION)
                    {
                        auto extinctionFactor = getExtinctionFactor(collideePlane.normal);
                        if (!isPlaneVisible(collideePlane, extinctionFactor))
                        {
                            deleteCollisions = true;
                            break;
                        }
                        
                        isLeavingPlane = true;
                    }
                }
            }
        }

        if (deleteCollisions)
            _currentCollisions.clear();

        if (isLeavingPlane)
        {
            auto endPosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, *_currentOffsetPlane);
            auto halfLastPlaneOffset = glm::length(endPosition - _offsetPlaneOrigin) * 0.5f;
            auto offsetPlaneOrigin = _offsetPlaneOrigin - leavingPlane.normal * (leavingDistance + halfLastPlaneOffset);
            _nodeTranslator->translate(-leavingPlane.normal * (leavingDistance - DECIMAL_TRUNCATION));
            //auto offsetPlaneOrigin = _offsetPlaneOrigin - leavingPlane.normal * 0.02f;
            auto offsetPlane = new plane(_offsetPlaneOrigin, leavingPlane.normal);
            auto translationPlane = createTranslationPlane(leavingPlane);
            _offsetPlanes[translationPlane] = offsetPlane;
            addTranslationPlane(translationPlane);

            changePlanes(translationPlane, offsetPlane);
            changedPlane = true;
        }

        if (changedPlane)
        {
            removePlanesIf(
                [&](translationPlane* translationPlane)
            {
                return translationPlane != _currentTranslationPlane;
            });
        }

        return changedPlane;
    }

    plane translationService::getClosestPlaneToPosition(vector<plane>& planes, vec3 position)
    {
        float minDistance = std::numeric_limits<float>().max();
        plane closestPlane;

        for (auto& plane : planes)
        {
            auto distanceToPlane = glm::abs(glm::dot(plane.toVec4(), vec4(position, 1.0f)));
            if (distanceToPlane < minDistance)
            {
                closestPlane = plane;
                minDistance = distanceToPlane;
            }
        }

        return closestPlane;
    }

    void translationService::translateTargetNodes(vec3 endPosition)
    {
        auto offset = endPosition - _offsetPlaneOrigin;

        if (offset == vec3())
            return;

        //debug("offset: " + to_string(offset));


        auto resolvedOffset = _nodeTranslator->translate(offset);

        //debug("offset diff: " + std::to_string(glm::length(resolvedOffset - offset)));

        _offsetPlaneOrigin += resolvedOffset;
        _currentOffsetPlane->origin = _offsetPlaneOrigin;

        if (_nodeTranslator->getLastTranslationTouchingCollisions()->size() > 0)
        {
            if (glm::length(resolvedOffset - offset) > 0.1f)
            {
                _ghostTranslator->enable();
                _ghostTranslator->translate(offset);
            }

            _currentCollisions.clear();
            //debug("cleared collisions");

            float minDistance = std::numeric_limits<float>().max();
            sweepCollision closestTouch;
            for (auto& touch : *_lastTouchingCollisions)
            {
                if (touch.distance < minDistance)
                {
                    minDistance = touch.distance;
                    closestTouch = touch;
                }
            }
            //debug("added collision");
            _currentCollisions.push_back(closestTouch);
        }
        else
            _ghostTranslator->disable();
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

    vector<sweepCollision> translationService::findTouchingCollisions(vec3 direction, float distance)
    {
        auto sweepTest = sweepCollisionMultiTest();
        sweepTest.colliders = &_targetNodesColliders;
        sweepTest.transforms = _nodeTranslator->getTransforms();
        sweepTest.direction = direction;
        sweepTest.distance = distance;

        if (distance == 0.0f)
            sweepTest.inflation = 2.5f * DECIMAL_TRUNCATION;

        sweepTest.disregardDivergentNormals = false;

        auto sweepResult = _physicsWorld->sweep(sweepTest);
        if (!sweepResult.collided)
            return vector<sweepCollision>();

        return sweepResult.collisions;
    }

    vector<sweepCollision> translationService::getValidTouchCollisions(vector<sweepCollision>& touchs)
    {
        vector<sweepCollision> validTouchs;
        vector<plane> validPlanes;

        for (auto& touch : touchs)
        {
            auto touchPlane = plane(_offsetPlaneOrigin, touch.normal);
            auto extinctionFactor = getExtinctionFactor(touchPlane.normal);

            bool isTouchPlaneParallelToAnyOtherTouchPlane = false;
            for (auto& validPlane : validPlanes)
            {
                if (touchPlane.isParallel(validPlane))
                {
                    isTouchPlaneParallelToAnyOtherTouchPlane = true;
                    break;
                }
            }

            if (!isTouchPlaneParallelToAnyOtherTouchPlane && isPlaneVisible(touchPlane, extinctionFactor))
            {
                validPlanes.push_back(touchPlane);
                validTouchs.push_back(touch);
            }
        }

        return validTouchs;
    }

    void translationService::startTranslation(ivec2 mousePosition)
    {
        _nodeTranslator->addRange(*_targetNodes);

        _targetNodesColliders.clear();
        for (auto& node : *_targetNodes)
            _targetNodesColliders.push_back(node->getComponent<boxCollider>());

        _lastTouchingCollisions = _nodeTranslator->getLastTranslationTouchingCollisions();
        _ghostTranslator->addRange(*_targetNodes);

        _isTranslating = true;
        _lastMousePosition = mousePosition;
        _offsetPlaneOrigin = _camera->screenPointToWorld(mousePosition.x, mousePosition.y);

        auto touchCollisions = findTouchingCollisions(vec3(0.0f, 1.0f, 0.0f), 0.0f);
        _currentCollisions = getValidTouchCollisions(touchCollisions);

        addPlaneIfNeeded();
    }

    void translationService::translate(ivec2 mousePosition)
    {
        if (!_isTranslating)
            return;

        if (_canChangePlanes)
        {
            if (!changeToAttachedPlane(mousePosition))
            {
                addValidPlanesFromTouchCollisions();
                changePlanesIfNeeded();
            }
        }

        auto endPosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, *_currentOffsetPlane);

        translateTargetNodes(endPosition);
        translatePlaneGrid(endPosition);

        auto touchCollisions = findTouchingCollisions(-_currentOffsetPlane->normal, 2.5f * DECIMAL_TRUNCATION);
        if (touchCollisions.size() > 0)
            _currentCollisions = getValidTouchCollisions(touchCollisions);

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

    void translationService::update()
    {
        deleteRemovedPlanes();

        if (!_isTranslating)
            return;
        
        updateTranslationPlanesVisibility();
        removeInvalidPlanes();
        addPlaneIfNeeded();
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
        _canChangePlanes = false;
    }

    void translationService::enablePlaneChanges()
    {
        _canChangePlanes = true;
    }
}