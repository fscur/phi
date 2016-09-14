#include <precompiled.h>
#include <core/boxCollider.h>
#include <core/ghostMesh.h>
#include <core/mesh.h>
#include <core/plane.h>
#include <core/string.h>
#include <animation/animator.h>
#include <physics/intersectionCollisionMultiTest.h>
#include <physics/intersectionCollisionPairTest.h>
#include <physics/sweepCollision.h>
#include <physics/sweepCollisionMultiTest.h>

#include "translationService.h"

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
        _currentTranslationPlane(nullptr),
        _isTranslating(false),
        _canChangePlanes(true),
        _nodeTranslator(new collisionNodeTranslator(physicsWorld)),
        _ghostTranslator(new ghostNodeTranslator(layer))
    {
    }

    translationService::~translationService()
    {
        safeDelete(_nodeTranslator);
        safeDelete(_ghostTranslator);
    }

    void translationService::startTranslation(ivec2 mousePosition)
    {
        resetTargetNodesColliders();

        _nodeTranslator->addRange(*_targetNodes);
        _ghostTranslator->addRange(*_targetNodes);
        _currentTranslationPlane = nullptr;
        _isTranslating = true;
        _lastMousePosition = mousePosition;
        _offsetPlaneOrigin = _camera->screenPointToWorld(mousePosition.x, mousePosition.y);

        checkCollisionsBeforeTranslation();

        bool changedPlanes = tryChangingPlanes();

        if (!changedPlanes)
            createPlanes();
    }

    void translationService::resetTargetNodesColliders()
    {
        _targetNodesColliders.clear();

        for (auto& node : *_targetNodes)
            _targetNodesColliders.push_back(node->getComponent<boxCollider>());
    }

    void translationService::checkCollisionsBeforeTranslation()
    {
        auto touchCollisions = findTouchingCollisions(vec3(0.0f, 1.0f, 0.0f), 0.0f);
        _currentCollisions = getValidTouchCollisions(touchCollisions);
        _lastCollisions = &_currentCollisions;
    }

    void translationService::createPlanes()
    {
        auto translationPlane = createAxisAlignedTranslationPlane(_lastMousePosition);
        auto offsetPlane = plane(_offsetPlaneOrigin, translationPlane->getPlane().normal);

        changePlanes(translationPlane, offsetPlane);
        showTranslationPlane();
    }

    translationPlane* translationService::createAxisAlignedTranslationPlane(ivec2 mousePosition)
    {
        auto viewDirection = -_camera->screenPointToRay(mousePosition.x, mousePosition.y).getDirection();
        auto worldAxis = mathUtils::getClosestAxisTo(viewDirection);
        auto axisPlane = createPlaneFromAxis(worldAxis);

        auto worldPosition = _camera->screenPointToWorld(mousePosition.x, mousePosition.y);
        auto origin = axisPlane.projectPoint(worldPosition);

        return createTranslationPlane(plane(origin, axisPlane.normal));
    }

    plane translationService::createPlaneFromAxis(const vec3& axis)
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

    translationPlane* translationService::createTranslationPlane(const plane& plane)
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

    void translationService::showTranslationPlane()
    {
        _layer->add(_currentTranslationPlane->getPlaneGridNode());
        _currentTranslationPlane->showGrid();
    }

    void translationService::changePlanes(translationPlane* translationPlane, const plane& offsetPlane)
    {
        if (_currentTranslationPlane)
            enqueuePlaneForDeletion(_currentTranslationPlane);

        _currentTranslationPlane = translationPlane;
        _offsetPlane = offsetPlane;
        _nodeTranslator->setPlane(_offsetPlane);
    }

    void translationService::translate(ivec2 mousePosition)
    {
        if (!_isTranslating)
            return;

        tryChangingPlanes();

        auto endPosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, _offsetPlane);

        translateTargetNodes(endPosition);
        translatePlaneGrid(endPosition);
        checkCollisionsAferTranslation();

        _lastMousePosition = mousePosition;
    }

    bool translationService::tryChangingPlanes()
    {
        if (_canChangePlanes)
        {
            if (tryChangeToAttachedPlane())
                return true;

            if (tryChangeToPlanesFromCollisions())
                return true;
        }

        return false;
    }

    bool translationService::tryChangeToAttachedPlane()
    {
        bool shouldLeavePlane = false;
        bool deleteCollisions = false;
        float maxDistance = std::numeric_limits<float>().lowest();
        float leavingDistance = -1.0f;
        plane leavingPlane;

        for (auto& touch : _currentCollisions)
        {
            auto node = touch.collider->getNode();
            while (node->getParent()->getParent() != nullptr) 
                node = node->getParent();

            auto colliderObb = node->getObb();
            auto collideePlanes = touch.collidee->getObb().getPlanes();

            for (auto& collideePlane : collideePlanes)
            {
                if (collideePlane.isParallel(_offsetPlane))
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
                        if (!isPlaneVisible(collideePlane))
                        {
                            deleteCollisions = true;
                            break;
                        }

                        shouldLeavePlane = true;
                    }
                }
            }
        }

        if (deleteCollisions)
            _currentCollisions.clear();

        if (shouldLeavePlane)
        {
            _nodeTranslator->translate(-leavingPlane.normal * (leavingDistance - DECIMAL_TRUNCATION));

            auto offsetPlane = plane(_offsetPlaneOrigin, leavingPlane.normal);
            auto translationPlane = createTranslationPlane(leavingPlane);

            changePlanes(translationPlane, offsetPlane);
            showTranslationPlane();

            return true;
        }

        return false;
    }

    bool translationService::tryChangeToPlanesFromCollisions()
    {
        for (auto& touch : *_lastCollisions)
        {
            auto touchPlaneNormal = touch.normal;
            auto touchPlanePosition = touch.position;

            if (!isNormalValidForCollision(touch, touchPlaneNormal))
                continue;

            auto touchPlaneOrigin = touch.collidee->getObb().getPositionAt(touchPlaneNormal);
            auto touchPlane = phi::plane(touchPlaneOrigin, touchPlaneNormal);

            touchPlane.origin = touchPlane.projectPoint(_offsetPlane.origin);

            if (canChangeTo(touchPlane))
            {
                auto translationPlane = createTranslationPlane(touchPlane);
                auto offsetPlane = plane(_offsetPlaneOrigin, touchPlane.normal);
                changePlanes(translationPlane, offsetPlane);
                showTranslationPlane();
                return true;
            }
        }

        return false;
    }

    void translationService::enqueuePlaneForDeletion(translationPlane* planeToRemove)
    {
        auto fadeOutAnimationEnded = [=]
        {
            _planesToDelete.push_back(planeToRemove);
        };

        planeToRemove->fadeGridOpacityOut(fadeOutAnimationEnded);
    }

    bool translationService::canChangeTo(const plane& plane)
    {
        if (!isPlaneVisible(plane))
            return false;

        if (_currentTranslationPlane && plane.isParallel(_currentTranslationPlane->getPlane()))
            return false;

        return true;
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

    void translationService::translateTargetNodes(const vec3& targetPosition)
    {
        auto offset = targetPosition - _offsetPlaneOrigin;

        if (offset == vec3())
            return;

        auto resolvedOffset = _nodeTranslator->translate(offset);

        _offsetPlaneOrigin += resolvedOffset;
        _offsetPlane.origin = _offsetPlaneOrigin;

        _lastCollisions = _nodeTranslator->getLastTranslationTouchingCollisions();

        auto hadAnyCollisions = _lastCollisions->size() > 0;
        auto shouldShowGhost = hadAnyCollisions && glm::length(resolvedOffset - offset) > 0.1f;

        if (shouldShowGhost)
        {
            _ghostTranslator->enable();
            _ghostTranslator->translate(offset);
        }
        else
        {
            _ghostTranslator->disable();
        }

        if (hadAnyCollisions)
            resetCurrentCollisions();
    }

    void translationService::resetCurrentCollisions()
    {
        _currentCollisions.clear();

        float minDistance = std::numeric_limits<float>().max();
        sweepCollision closestTouch;
        for (auto& touch : *_lastCollisions)
        {
            if (touch.distance < minDistance)
            {
                minDistance = touch.distance;
                closestTouch = touch;
            }
        }

        _currentCollisions.push_back(closestTouch);
    }

    void translationService::translatePlaneGrid(const vec3& targetPosition)
    {
        auto planeGridPosition = _currentTranslationPlane->getPlane().projectPoint(targetPosition);
        _currentTranslationPlane->animatePlaneGridPosition(planeGridPosition);
    }

    void translationService::checkCollisionsAferTranslation()
    {
        auto touchCollisions = findTouchingCollisions(-_offsetPlane.normal, 2.5f * DECIMAL_TRUNCATION);
        if (touchCollisions.size() > 0)
            _currentCollisions = getValidTouchCollisions(touchCollisions);
    }

    vector<sweepCollision> translationService::findTouchingCollisions(const vec3& direction, float distance)
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
            bool isTouchPlaneParallelToAnyOtherTouchPlane = false;

            for (auto& validPlane : validPlanes)
            {
                if (touchPlane.isParallel(validPlane))
                {
                    isTouchPlaneParallelToAnyOtherTouchPlane = true;
                    break;
                }
            }

            if (!isTouchPlaneParallelToAnyOtherTouchPlane && isPlaneVisible(touchPlane))
            {
                validPlanes.push_back(touchPlane);
                validTouchs.push_back(touch);
            }
        }

        return validTouchs;
    }

    bool translationService::isPlaneVisible(const plane& plane)
    {
        auto planeVisibility = getPlaneVisibility(plane);
        return planeVisibility > getExtinctionFactor(plane.normal);
    }

    float translationService::getPlaneVisibility(const plane& plane)
    {
        auto cameraTransform = _camera->getTransform();
        auto cameraPosition = cameraTransform->getPosition();
        auto toPlaneDir = -glm::normalize(plane.origin - cameraPosition);
        auto axis = mathUtils::getClosestAxisTo(plane.normal);

        float priority = glm::abs(glm::dot(toPlaneDir, vec3(0.0f, 1.0f, 0.0f)));

        if (mathUtils::isParallel(axis, vec3(0.0, 1.0, 0.0)))
            priority = 0.0f;

        auto visibility = glm::dot(plane.normal, toPlaneDir) * (1.0f - glm::pow(priority, 2.0f));
        return visibility;
    }

    float translationService::getExtinctionFactor(const vec3& normal)
    {
        return mathUtils::isParallel(vec3(0.0, 1.0, 0.0), normal) ? 0.3f : 0.4f;
    }

    void translationService::endTranslation()
    {
        _isTranslating = false;

        enqueuePlaneForDeletion(_currentTranslationPlane);

        for (auto& translationPlane : _clippingTranslationPlanes)
            enqueuePlaneForDeletion(translationPlane);

        _clippingTranslationPlanes.clear();
        _ghostTranslator->clear();
        _nodeTranslator->clear();
        _nodeTranslator->enableCollisions();
    }

    void translationService::update()
    {
        for (auto& planeToDelete : _planesToDelete)
            deletePlane(planeToDelete);

        _planesToDelete.clear();

        if (!_isTranslating)
            return;

        updatePlaneVisibility();
        deletePlaneIfNotVisible();
    }

    void translationService::deletePlane(translationPlane* translationPlane)
    {
        translationPlane->hideGrid();
        auto planeGridNode = translationPlane->getPlaneGridNode();
        planeGridNode->getParent()->removeChild(planeGridNode);
        delete translationPlane;
        translationPlane = nullptr;
    }

    void translationService::updatePlaneVisibility()
    {
        auto visibility = getPlaneVisibility(_currentTranslationPlane->getPlane());

        _currentTranslationPlane->updatePlaneGridVisibility(visibility);
    }

    void translationService::deletePlaneIfNotVisible()
    {
        if (!isPlaneVisible(_currentTranslationPlane->getPlane()))
        {
            enqueuePlaneForDeletion(_currentTranslationPlane);
            createPlanes();
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
        _canChangePlanes = false;
    }

    void translationService::enablePlaneChanges()
    {
        _canChangePlanes = true;
    }
}