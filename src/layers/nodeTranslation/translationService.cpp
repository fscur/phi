#include <precompiled.h>
#include <core/boxCollider.h>
#include <core/ghostMesh.h>
#include <core/mesh.h>
#include <core/plane.h>
#include <core/string.h>
#include <animation/animator.h>
#include <physics/intersectionCollisionMultiTest.h>
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

    void translationService::startTranslation(ivec2 mousePosition, node* clickedNode)
    {
        _clickedNode = clickedNode;
        _nodeTranslator->addRange(*_targetNodes);
        _ghostTranslator->addRange(*_targetNodes);
        _currentTranslationPlane = nullptr;
        _isTranslating = true;
        _lastMousePosition = mousePosition;
        _offsetPlane.origin = _camera->screenPointToWorld(mousePosition.x, mousePosition.y);
        _snappedAtX = _snappedAtY = _snappedAtZ = false;
        _snappedDelta = vec3();
        _collidedDelta = vec3();

        changeToAxisAlignedPlane();
        changeToTouchingPlaneIfAble();

        _lastOffsetPlane = _offsetPlane;
        _lastOffsetPlaneCollider = nullptr;
    }

    bool translationService::changeToTouchingPlaneIfAble()
    {
        auto touchCollisions = findTouchingCollisions(vec3(0.0f, 1.0f, 0.0f), 0.0f);
        auto validCollisions = getValidTouchCollisions(touchCollisions);

        if (validCollisions.size() == 0)
            return false;

        auto firstValidCollision = validCollisions[0];
        auto offsetPlane = plane(_offsetPlane.origin, firstValidCollision.normal);
        auto planeGridPlane = plane(firstValidCollision.collider->getObb().getPositionAt(-firstValidCollision.normal), firstValidCollision.normal);
        auto translationPlane = createTranslationPlane(planeGridPlane);
        translationPlane->setCollidee(firstValidCollision.collidee);

        changePlanes(translationPlane, offsetPlane);
        showTranslationPlane();

        return true;
    }

    void translationService::changeToAxisAlignedPlane()
    {
        auto translationPlane = createAxisAlignedTranslationPlane(_lastMousePosition);
        auto offsetPlane = plane(_offsetPlane.origin, translationPlane->getPlane().normal);

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

        auto planeGrid = new phi::translationPlaneGrid();
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

        auto fadeInAnimation = new phi::floatAnimation(easingFunctions::easeInQuad);

        fadeInAnimation->setUpdateFunction(fadeUpdadeFunction);
        animator->addAnimation(fadeInAnimation);

        auto fadeOutAnimation = new phi::floatAnimation(easingFunctions::easeOutQuad);
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
        {
            enqueuePlaneForDeletion(_currentTranslationPlane);
            _lastOffsetPlaneCollider = _currentTranslationPlane->getCollidee();
        }

        _lastOffsetPlane = _offsetPlane;
        _currentTranslationPlane = translationPlane;
        _offsetPlane = offsetPlane;
        _nodeTranslator->setPlane(_offsetPlane);
        _collidedDelta = vec3();

        deleteClippedPlanes();
    }

    void translationService::translate(ivec2 mousePosition)
    {
        if (!_isTranslating)
            return;

        auto endPosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, _offsetPlane);
        auto offset = endPosition - _offsetPlane.origin;

        //auto oldSnappedAtX = _snappedAtX;
        //auto oldSnappedAtY = _snappedAtY;
        //auto oldSnappedAtZ = _snappedAtZ;
        //auto snappedOffset = snapToGrid(offset + _collidedDelta + _snappedDelta);
        auto snappedOffset = offset;

        auto snappedAndCheckedOffset = tryChangeToAttachedPlane(snappedOffset);
        translateTargetNodes(snappedAndCheckedOffset);

        //if (oldSnappedAtX && _snappedAtX)
        //    _snappedDelta.x += offset.x;
        //else
        //    _snappedDelta.x = 0.0f;

        //if (oldSnappedAtY && _snappedAtY)
        //    _snappedDelta.y += offset.y;
        //else
        //    _snappedDelta.y = 0.0f;

        //if (oldSnappedAtZ && _snappedAtZ)
        //    _snappedDelta.z += offset.z;
        //else
        //    _snappedDelta.z = 0.0f;

        _offsetPlane.origin = endPosition;
        translatePlaneGrid(endPosition);
        //checkCollisionsAferTranslation();

        if (glm::length(_lastOffsetPlane.origin - endPosition) >= 0.2f)
        {
            _lastOffsetPlane = _offsetPlane;
            _lastOffsetPlaneCollider = nullptr;
        }

        tryChangingPlanes();
        checkForClippingPlanes();

        //updateClippedPlanes();

        _lastMousePosition = mousePosition;
    }

    vec3 translationService::snapToGrid(vec3 offset)
    {
        auto destinationObb = _nodeTranslator->getNodeDestinationObb(_clickedNode);
        vec3 minimum;
        vec3 maximum;
        destinationObb.getLimits(minimum, maximum);

        vec3 limits[2] = { minimum, maximum };
        vec3 offsetedLimits[2];

        float const SNAP_MARGIN_GRID_SIZE_PERCENT = 0.15f;
        auto gridSize = 1.0f;

        for (auto i = 0; i < 2; ++i)
            offsetedLimits[i] = limits[i] + offset;

        auto snapMargin = gridSize * SNAP_MARGIN_GRID_SIZE_PERCENT;
        auto highSnapMargin = gridSize - snapMargin;
        _snappedAtX = false;
        _snappedAtY = false;
        _snappedAtZ = false;
        vec3 amountSnapped;

        auto snappedOffset = offset;
        for (auto i = 0; i < 2; ++i)
        {
            auto mod = glm::abs(glm::modf(offsetedLimits[i], vec3(gridSize)));

            if (glm::abs(_offsetPlane.normal) != vec3(1.0f, 0.0f, 0.0f))
            {
                if (mod.x < snapMargin || mod.x > highSnapMargin)
                {
                    if (!_snappedAtX)
                    {
                        _snappedAtX = true;
                        auto roundedX = glm::round(offsetedLimits[i].x);
                        amountSnapped.x = glm::abs(roundedX - offsetedLimits[i].x);
                        snappedOffset.x = roundedX - limits[i].x;
                    }
                    else
                    {
                        auto roundedX = glm::round(offsetedLimits[i].x);
                        auto amountSnappedX = glm::abs(roundedX - offsetedLimits[i].x);
                        if (amountSnappedX < amountSnapped.x)
                        {
                            amountSnapped.x = amountSnappedX;
                            snappedOffset.x = roundedX - limits[i].x;
                        }
                    }
                }
            }

            if (glm::abs(_offsetPlane.normal) != vec3(0.0f, 1.0f, 0.0f))
            {
                if (mod.y < snapMargin || mod.y > highSnapMargin)
                {
                    if (!_snappedAtY)
                    {
                        _snappedAtY = true;
                        auto roundedY = glm::round(offsetedLimits[i].y);
                        amountSnapped.y = glm::abs(roundedY - offsetedLimits[i].y);
                        snappedOffset.y = roundedY - limits[i].y;
                    }
                    else
                    {
                        auto roundedY = glm::round(offsetedLimits[i].y);
                        auto amountSnappedY = glm::abs(roundedY - offsetedLimits[i].y);
                        if (amountSnappedY < amountSnapped.y)
                        {
                            amountSnapped.y = amountSnappedY;
                            snappedOffset.y = roundedY - limits[i].y;
                        }
                    }
                }
            }

            if (glm::abs(_offsetPlane.normal) != vec3(0.0f, 0.0f, 1.0f))
            {
                if (mod.z < snapMargin || mod.z > highSnapMargin)
                {
                    if (!_snappedAtZ)
                    {
                        _snappedAtZ = true;
                        auto roundedZ = glm::round(offsetedLimits[i].z);
                        amountSnapped.z = glm::abs(roundedZ - offsetedLimits[i].z);
                        snappedOffset.z = roundedZ - limits[i].z;
                    }
                    else
                    {
                        auto roundedZ = glm::round(offsetedLimits[i].z);
                        auto amountSnappedZ = glm::abs(roundedZ - offsetedLimits[i].z);
                        if (amountSnappedZ < amountSnapped.z)
                        {
                            amountSnapped.z = amountSnappedZ;
                            snappedOffset.z = roundedZ - limits[i].z;
                        }
                    }
                }
            }
        }

        return snappedOffset;
    }

    void translationService::checkForClippingPlanes()
    {
        //for (auto& touch : _currentCollisions)
        //{
        //    auto node = touch.collider->getNode();
        //    while (node->getParent()->getParent() != nullptr)
        //        node = node->getParent();

        //    auto colliderObb = node->getObb();
        //    auto collideePlanes = touch.collidee->getObb().getPlanes();

        //    //find suitable planes
        //    for (auto& collideePlane : collideePlanes)
        //    {
        //        if (collideePlane.isParallel(_offsetPlane))
        //            continue;

        //        auto collideePlaneVisibility = getPlaneVisibility(collideePlane);
        //        if (collideePlaneVisibility < 0.0f)
        //            continue;

        //        bool isParallelToValidPlane = false;
        //        for (auto& validPlane : _currentValidPlanes)
        //        {
        //            if (validPlane.plane.isParallel(collideePlane))
        //            {
        //                isParallelToValidPlane = true;
        //                break;
        //            }
        //        }

        //        for (auto& pair : _clippedTranslationPlanes)
        //        {
        //            auto clippedPlane = pair.first;
        //            if (clippedPlane.isParallel(collideePlane))
        //            {
        //                isParallelToValidPlane = true;
        //                break;
        //            }
        //        }

        //        if (isParallelToValidPlane)
        //            continue;

        //        auto plane = collisionObbPlane();
        //        plane.colliderObb = colliderObb;
        //        plane.plane = collideePlane;

        //        _currentValidPlanes.push_back(plane);
        //    }
        //}

        ////create planes
        //for (auto& collideePlane : _currentValidPlanes)
        //{
        //    if (collideePlane.plane.isParallel(_offsetPlane))
        //        continue;

        //    auto colliderObb = collideePlane.colliderObb;
        //    vec4 leavingPos = vec4(colliderObb->getPositionAt(-collideePlane.plane.normal), 1.0);
        //    float distanceFromPlane = glm::dot(collideePlane.plane.toVec4(), leavingPos) + 0.4f;

        //    bool shouldCreateClippedPlane = _clippedTranslationPlanes.find(collideePlane.plane) == _clippedTranslationPlanes.end();

        //    if (distanceFromPlane > 0.0f && shouldCreateClippedPlane)
        //    {
        //        auto clippedTranslationPlane = createTranslationPlane(collideePlane.plane);
        //        _clippedTranslationPlanes[collideePlane.plane] = clippedTranslationPlane;

        //        auto clippingPlane = _currentTranslationPlane->getClippingPlane();
        //        auto clippedPlaneGrid = clippedTranslationPlane->getPlaneGrid();
        //        clippedPlaneGrid->addClippingPlane(clippingPlane);
        //        clippedPlaneGrid->setClippingPlaneDistance(clippingPlane, clippingDistance::negative);
        //        clippedPlaneGrid->setClippingPlaneOpacity(clippingPlane, 0.0f);
        //        clippedPlaneGrid->setVisibilityFactor(1.0f);
        //        clippedPlaneGrid->setOpacity(0.0f);
        //        _layer->add(clippedTranslationPlane->getPlaneGridNode());
        //        clippedPlaneGrid->show();

        //        auto currentPlaneGrid = _currentTranslationPlane->getPlaneGrid();
        //        auto currentClippingPlane = clippedTranslationPlane->getClippingPlane();
        //        currentPlaneGrid->addClippingPlane(currentClippingPlane);
        //        currentPlaneGrid->setClippingPlaneDistance(currentClippingPlane, clippingDistance::negative);
        //        currentPlaneGrid->setClippingPlaneOpacity(currentClippingPlane, 0.0f);
        //    }
        //}
    }

    bool translationService::tryChangingPlanes()
    {
        if (_canChangePlanes)
        {
            if (tryChangeToPlanesFromCollisions())
                return true;
        }

        return false;
    }

    vec3 translationService::tryChangeToAttachedPlane(vec3 offset)
    {
        auto shouldLeavePlane = false;
        auto touchCollisions = findTouchingCollisions(-_offsetPlane.normal, 2.5f * DECIMAL_TRUNCATION);
        auto validCollisions = getValidTouchCollisions(touchCollisions);

        for (auto& touch : validCollisions)
        {
            auto node = touch.collider->getNode();
            while (node->getParent()->getParent() != nullptr)
                node = node->getParent();

            auto translatedColliderObb = _nodeTranslator->getNodeDestinationObb(node);
            translatedColliderObb.center += offset;

            auto collideePlanes = touch.collidee->getObb().getPlanes();
            for (auto& collideePlane : collideePlanes)
            {
                if (collideePlane.isParallel(_offsetPlane))
                    continue;

                if (!isPlaneVisible(collideePlane))
                    continue;

                if (mathUtils::isParallel(_lastOffsetPlane.normal, collideePlane.normal))
                    continue;

                auto leavingPosition = translatedColliderObb.getPositionAt(-collideePlane.normal);
                auto leavingDistance = collideePlane.distanceFrom(leavingPosition);

                if (leavingDistance > 0.0f)
                {
                    auto projectedLeavingPosition = collideePlane.projectPoint(leavingPosition);
                    projectedLeavingPosition += collideePlane.normal * DECIMAL_TRUNCATION;

                    auto offsetPlane = plane(_offsetPlane.origin, collideePlane.normal);
                    auto translationPlane = createTranslationPlane(collideePlane);

                    changePlanes(translationPlane, offsetPlane);
                    showTranslationPlane();

                    auto snapToPlaneOffset = projectedLeavingPosition - leavingPosition;
                    return offset + snapToPlaneOffset;
                }
            }

            if (shouldLeavePlane)
                break;
        }

        //deleteClippedPlanes();

        return offset;
    }

    bool translationService::tryChangeToPlanesFromCollisions()
    {
        auto resultedCollisions = _nodeTranslator->getLastTranslationTouchingCollisions();

        for (auto& touch : *resultedCollisions)
        {
            auto touchPlaneNormal = touch.normal;
            auto touchPlanePosition = touch.position;

            if (!isNormalValidForCollision(touch, touchPlaneNormal))
                continue;

            if (touch.collidee == _lastOffsetPlaneCollider)
                continue;

            auto touchPlaneOrigin = touch.collidee->getObb().getPositionAt(touchPlaneNormal);
            auto touchPlane = phi::plane(touchPlaneOrigin, touchPlaneNormal);

            touchPlane.origin = touchPlane.projectPoint(_offsetPlane.origin);

            if (canChangeTo(touchPlane))
            {
                auto translationPlane = createTranslationPlane(touchPlane);
                translationPlane->setCollidee(touch.collidee);
                auto offsetPlane = plane(_offsetPlane.origin, touchPlane.normal);
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

    void translationService::translateTargetNodes(const vec3 offset)
    {
        if (offset == vec3())
            return;

        auto resolvedOffset = _nodeTranslator->translate(offset);

        //_objectToPlaneDelta += resolvedOffset - offset;

        auto resultedCollisions = _nodeTranslator->getLastTranslationTouchingCollisions();
        auto hadAnyCollisions = resultedCollisions->size() > 0;
        auto shouldShowGhost = hadAnyCollisions && glm::length(resolvedOffset - offset) > 0.1f;

        if (shouldShowGhost)
        {
            _ghostTranslator->enable();
            _ghostTranslator->translate(offset);
        }
        else
            _ghostTranslator->disable();

        if (hadAnyCollisions)
        {
            _collidedDelta = offset - resolvedOffset;
            //resetCurrentCollisions();
        }
        else
            _collidedDelta = vec3();
    }

    void translationService::resetCurrentCollisions()
    {
        //_currentCollisions.clear();

        //float minDistance = std::numeric_limits<float>().max();
        //sweepCollision closestTouch;
        //auto resultedCollisions = _nodeTranslator->getLastTranslationTouchingCollisions();
        //for (auto& touch : *resultedCollisions)
        //{
        //    if (touch.distance < minDistance)
        //    {
        //        minDistance = touch.distance;
        //        closestTouch = touch;
        //    }
        //}

        //_currentCollisions.push_back(closestTouch);
    }

    void translationService::translatePlaneGrid(const vec3& targetPosition)
    {
        auto planeGridPosition = _currentTranslationPlane->getPlane().projectPoint(targetPosition);
        _currentTranslationPlane->animatePlaneGridPosition(planeGridPosition);
    }

    void translationService::checkCollisionsAferTranslation()
    {
        //auto resultedCollisions = _nodeTranslator->getLastTranslationTouchingCollisions();
        //if (resultedCollisions->size() > 0)
        //    return;

        //auto touchCollisions = findTouchingCollisions(-_offsetPlane.normal, 2.5f * DECIMAL_TRUNCATION);
        //if (touchCollisions.size() > 0)
        //    _currentCollisions = getValidTouchCollisions(touchCollisions);
    }

    void translationService::updateClippedPlanes()
    {
        //update opacity

        for (auto& collisionPlane : _currentValidPlanes)
        {
            auto collideePlane = collisionPlane.plane;
            auto colliderObb = collisionPlane.colliderObb;

            bool hasClippedPlane = _clippedTranslationPlanes.find(collideePlane) != _clippedTranslationPlanes.end();

            if (!hasClippedPlane)
                continue;

            auto clippedTranslationPlane = _clippedTranslationPlanes[collideePlane];

            if (collisionPlane.plane.isParallel(_currentTranslationPlane->getPlane()))
            {
                auto currentClippingPlane = clippedTranslationPlane->getClippingPlane();
                _currentTranslationPlane->getPlaneGrid()->removeClippingPlane(currentClippingPlane);
                enqueuePlaneForDeletion(_clippedTranslationPlanes[collideePlane]);
                _clippedTranslationPlanes.erase(collideePlane);
                continue;
            }

            //auto distanceFromPlane = glm::dot(collideePlane.toVec4(), vec4(colliderObb->center, 1.0f));
            vec4 leavingPos = vec4(colliderObb->getPositionAt(-collideePlane.normal), 1.0);
            float distanceFromPlane = glm::dot(collideePlane.toVec4(), leavingPos) + 0.4f;

            if (distanceFromPlane > 0.0f)
            {
                auto opacity = glm::clamp(distanceFromPlane / 0.4f, 0.0f, 1.0f);
                auto currentClippingPlane = clippedTranslationPlane->getClippingPlane();
                auto currentPlaneGrid = _currentTranslationPlane->getPlaneGrid();

                //currentPlaneGrid->setClippingPlaneOpacity(currentClippingPlane, 1.0f - opacity);

                if (isPlaneVisible(currentClippingPlane->plane))
                {
                    //debug("visible: " + std::to_string(1.0f - opacity));
                    clippedTranslationPlane->getPlaneGrid()->setOpacity(opacity);
                    currentPlaneGrid->setClippingPlaneOpacity(currentClippingPlane, 1.0f - opacity);
                }
                else
                {
                    clippedTranslationPlane->getPlaneGrid()->setOpacity(0.0f);
                    currentPlaneGrid->setClippingPlaneOpacity(currentClippingPlane, 1.0f);
                }
            }
            else
            {
                auto currentClippingPlane = clippedTranslationPlane->getClippingPlane();
                _currentTranslationPlane->getPlaneGrid()->removeClippingPlane(currentClippingPlane);
                _clippedTranslationPlanes.erase(collideePlane);
                enqueuePlaneForDeletion(clippedTranslationPlane);
            }
        }

        //debug("clippingPlanes: " + std::to_string(_clippedTranslationPlanes.size()));
        //debug("clippingPlanesa: " + std::to_string(_currentTranslationPlane->getPlaneGrid()->getClippingPlanes().size()));
        /*for (auto& pair : _clippedTranslationPlanes)
        {
            auto clippedPlane = pair.second;
            debug("normal: " + to_string(clippedPlane->getPlane().normal));
            debug("origin: " + to_string(clippedPlane->getPlane().origin));
        }*/
    }

    vector<sweepCollision> translationService::findTouchingCollisions(const vec3& direction, float distance)
    {
        auto sweepTest = sweepCollisionMultiTest();
        sweepTest.colliders = _nodeTranslator->getPiledUpColliders();
        sweepTest.transforms = &_nodeTranslator->createOffsetedTransforms(vec3()); // TODO: delete these transforms
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
        auto validTouchs = vector<sweepCollision>();

        if (_currentTranslationPlane == nullptr)
            return validTouchs;

        auto validPlanes = vector<plane>();
        for (auto& touch : touchs)
        {
            auto isTouchPlaneParallelToAnyOtherTouchPlane = false;
            for (auto& validPlane : validPlanes)
            {
                if (mathUtils::isParallel(touch.normal, validPlane.normal))
                {
                    isTouchPlaneParallelToAnyOtherTouchPlane = true;
                    break;
                }
            }

            if (isTouchPlaneParallelToAnyOtherTouchPlane)
                continue;

            auto touchPlane = plane(_currentTranslationPlane->getPlane().origin, touch.normal);
            if (isPlaneVisible(touchPlane))
            {
                validPlanes.push_back(touchPlane);
                validTouchs.push_back(touch);
            }
        }

        return validTouchs;
    }

    bool translationService::isPlaneVisible(const plane& plane)
    {
        float planeVisibility = getPlaneVisibility(plane);
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

        auto visibility = glm::dot(plane.normal, toPlaneDir) * (1.0f - glm::pow(priority, 2.0f) * 0.5f);
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

        deleteClippedPlanes();
        _ghostTranslator->clear();
        _nodeTranslator->clear();
        _nodeTranslator->enableCollisions();
    }

    void translationService::deleteClippedPlanes()
    {
        for (auto& pair : _clippedTranslationPlanes)
        {
            _currentTranslationPlane->getPlaneGrid()->removeClippingPlane(pair.second->getClippingPlane());
            enqueuePlaneForDeletion(pair.second);
        }

        _clippedTranslationPlanes.clear();
        _currentValidPlanes.clear();
    }

    void translationService::update()
    {
        for (auto& planeToDelete : _planesToDelete)
            deletePlane(planeToDelete);

        _planesToDelete.clear();

        if (!_isTranslating)
            return;

        for (auto& pair : _clippedTranslationPlanes)
        {
            auto clippedTranslationPlane = pair.second;
            updatePlaneVisibility(clippedTranslationPlane);
        }

        updateClippedPlanes();

        updatePlaneVisibility(_currentTranslationPlane);
        deletePlaneIfNotVisible();
    }

    /*
    translationPlane* clippedTranslationPlane = nullptr;

     auto shouldAddClippedPlane = _clippedTranslationPlanes.find(collideePlane) == _clippedTranslationPlanes.end();
     if (shouldAddClippedPlane)
     {

         //if (clippedTranslationPlane)
         //_clippedTranslationPlanes[collideePlane] = clippedTranslationPlane;
         /*auto clippingPlane = _currentTranslationPlane->getClippingPlane();
         auto clippedPlaneGrid = clippedTranslationPlane->getPlaneGrid();
         clippedPlaneGrid->addClippingPlane(clippingPlane);
         clippedPlaneGrid->setClippingPlaneDistance(clippingPlane, clippingDistance::negative);
         clippedPlaneGrid->setClippingPlaneOpacity(clippingPlane, 1.0f);
         clippedPlaneGrid->setVisibilityFactor(1.0f);
         clippedPlaneGrid->setOpacity(0.0f);
         _layer->add(clippedTranslationPlane->getPlaneGridNode());
         clippedPlaneGrid->show();

         auto currentPlaneGrid = _currentTranslationPlane->getPlaneGrid();
         auto currentClippingPlane = clippedTranslationPlane->getClippingPlane();
         currentPlaneGrid->addClippingPlane(currentClippingPlane);
         currentPlaneGrid->setClippingPlaneDistance(currentClippingPlane, clippingDistance::negative);
         currentPlaneGrid->setClippingPlaneOpacity(currentClippingPlane, 0.0f);
     }
     else
     {
         //auto opacity = glm::clamp(distanceFromPlane + 1.0f, 0.0f, 1.0f);

         //clippedTranslationPlane = _clippedTranslationPlanes[collideePlane];
         //auto clippedPlaneGrid = clippedTranslationPlane->getPlaneGrid();

         //if (isPlaneVisible(clippedTranslationPlane->getPlane()))
         //    clippedPlaneGrid->setOpacity(opacity);
         //else
         //    clippedPlaneGrid->setOpacity(0.2f);

         //auto currentClippingPlane = clippedTranslationPlane->getClippingPlane();
         //auto currentPlaneGrid = _currentTranslationPlane->getPlaneGrid();
         //currentPlaneGrid->setClippingPlaneOpacity(currentClippingPlane, 1.0f - opacity);
     }
     */

    void translationService::deletePlane(translationPlane* translationPlane)
    {
        translationPlane->hideGrid();
        auto planeGridNode = translationPlane->getPlaneGridNode();
        planeGridNode->getParent()->removeChild(planeGridNode);
        safeDelete(translationPlane);
    }

    void translationService::updatePlaneVisibility(translationPlane* translationPlane)
    {
        auto visibility = getPlaneVisibility(translationPlane->getPlane());

        translationPlane->updatePlaneGridVisibilityFactor(visibility);
    }

    void translationService::deletePlaneIfNotVisible()
    {
        if (!isPlaneVisible(_currentTranslationPlane->getPlane()))
        {
            deleteClippedPlanes();
            enqueuePlaneForDeletion(_currentTranslationPlane);
            changeToAxisAlignedPlane();
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