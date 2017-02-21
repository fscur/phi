#include <precompiled.h>
#include <core/boxCollider.h>
#include <core/ghostMesh.h>
#include <core/mesh.h>
#include <core/plane.h>
#include <core/string.h>
#include <animation/animator.h>
#include <physics/intersection/groupToSceneTest.h>
#include <physics/sweep/sweepCollision.h>
#include <physics/sweep/groupToSceneObbTest.h>

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
        _nodeTranslator(new collisionNodeTranslator(physicsWorld)),
        _ghostTranslator(new ghostNodeTranslator(layer)),
        _isTranslating(false),
        _canChangePlanes(true),
        _isSnapToGridEnabled(false),
        _clickedNode(nullptr),
        _offsetPlane(),
        _orientedOffsetPlane(),
        _currentTranslationPlane(nullptr),
        _planesToDelete(),
        _lastMousePosition(),
        _collidedDelta(),
        _snappedDelta(),
        _targetNodesDestinationObbs(),
        _snapGridSize(1.0f)
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
        _snappedDelta = vec3();
        _collidedDelta = vec3();
        _lastTranslationTouchingCollisions = vector<sweep::sweepObbCollision>();

        for (auto& targetNode : *_targetNodes)
            _targetNodesDestinationObbs.push_back(new obb(*targetNode->getObb()));

        changeToAxisAlignedPlane();
        changeToTouchingPlaneIfAble();
    }

    bool translationService::changeToTouchingPlaneIfAble()
    {
        auto touchCollisions = findTouchingCollisions();
        auto validCollisions = filterValidTouchCollisions(touchCollisions);

        if (validCollisions.size() == 0)
            return false;

        auto firstValidCollision = validCollisions[0];
        auto offsetPlane = plane(_offsetPlane.origin, firstValidCollision.normal);
        auto planeGridPlane = plane(firstValidCollision.collider->getNode()->getObb()->getPositionAt(-firstValidCollision.normal), firstValidCollision.normal);
        auto translationPlane = createTranslationPlane(planeGridPlane);
        translationPlane->setCollidee(firstValidCollision.collidee);
        translationPlane->setCollider(firstValidCollision.collider);

        changePlanes(translationPlane, offsetPlane);

        return true;
    }

    void translationService::changeToAxisAlignedPlane()
    {
        auto translationPlane = createAxisAlignedTranslationPlane(_lastMousePosition);
        auto offsetPlane = plane(_offsetPlane.origin, translationPlane->getPlane().normal);

        changePlanes(translationPlane, offsetPlane);
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
        planeGrid->setColor(color::white);
        planeGrid->setLineThickness(6.5f);
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

    void translationService::changePlanes(translationPlane* translationPlane, const plane& offsetPlane)
    {
        if (_currentTranslationPlane)
            enqueuePlaneForDeletion(_currentTranslationPlane);

        _currentTranslationPlane = translationPlane;
        _offsetPlane = offsetPlane;

        _nodeTranslator->setPlane(_offsetPlane);
        _collidedDelta = vec3();

        createOffsetOrientedPlane();
        deleteClippedPlanes();
        showTranslationPlane();
    }

    void translationService::createOffsetOrientedPlane()
    {
        auto planeGridNodeTransform = _currentTranslationPlane->getPlaneGridNode()->getTransform();
        auto origin = _offsetPlane.projectPoint(vec3());
        auto rightPoint = origin + planeGridNodeTransform->getRight();
        auto upPoint = origin + planeGridNodeTransform->getUp();
        _orientedOffsetPlane = orientedPlane(origin, rightPoint, upPoint);
    }

    void translationService::showTranslationPlane()
    {
        _layer->add(_currentTranslationPlane->getPlaneGridNode());
        _currentTranslationPlane->showGrid();
    }

    void translationService::translate(ivec2 mousePosition)
    {
        if (!_isTranslating)
            return;

        auto targetPosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, _offsetPlane);
        auto offset = targetPosition - _offsetPlane.origin;

        offset += _collidedDelta + _snappedDelta;
        offset += getSnapOffset(offset);
        offset += tryChangeToAttachedPlane(offset);
        translateTargetNodes(offset);

        _offsetPlane.origin = targetPosition;
        translatePlaneGrid(targetPosition);

        tryChangeToPlanesFromCollisions();
        addClippingPlanes();
        updateClippingPlanes();
        _lastMousePosition = mousePosition;
    }

    vec3 translationService::getSnapOffset(vec3 offset)
    {
        if (!_isSnapToGridEnabled)
            return vec3();

        auto snapOffset = snapToGrid(offset);

        auto tryingToLeaveSnap = glm::dot(offset - _collidedDelta, snapOffset) < 0.0f;
        if (snapOffset != vec3() && tryingToLeaveSnap)
        {
            auto inverseSnapDirection = -glm::normalize(snapOffset);
            _snappedDelta = inverseSnapDirection * glm::dot(inverseSnapDirection, offset - _collidedDelta);
        }
        else
            _snappedDelta = vec3();

        return snapOffset;
    }

    vec3 translationService::snapToGrid(vec3 offset)
    {
        vec2 minimum, maximum;
        getObbLimitsOnOrientedOffsetPlane(offset, minimum, maximum);
        vec2 projectedOffsetedLimits[2] = { minimum, maximum };

        float const SNAP_MARGIN_GRID_SIZE_PERCENT = 0.63f;

        auto snapMargin = _snapGridSize * SNAP_MARGIN_GRID_SIZE_PERCENT;
        auto highSnapMargin = _snapGridSize - snapMargin;
        vec2 amountSnapped;
        auto snappedAtX = false;
        auto snappedAtY = false;

        for (auto i = 0; i < 2; ++i)
        {
            auto mod = glm::sign(projectedOffsetedLimits[i]) * glm::mod(glm::abs(projectedOffsetedLimits[i]), vec2(_snapGridSize));
            auto absMod = glm::abs(mod);

            if (absMod.x < snapMargin || absMod.x > highSnapMargin)
            {
                auto snapDelta = mathUtils::round(mod.x, _snapGridSize) - mod.x;
                if (!snappedAtX)
                {
                    amountSnapped.x = snapDelta;
                    snappedAtX = true;
                }
                else if (glm::abs(snapDelta) < glm::abs(amountSnapped.x))
                    amountSnapped.x = snapDelta;
            }

            if (absMod.y < snapMargin || absMod.y > highSnapMargin)
            {
                auto snapDelta = mathUtils::round(mod.y, _snapGridSize) - mod.y;
                if (!snappedAtY)
                {
                    amountSnapped.y = snapDelta;
                    snappedAtY = true;
                }
                else if (glm::abs(snapDelta) < glm::abs(amountSnapped.y))
                    amountSnapped.y = snapDelta;
            }
        }

        auto snappedOffset =
            _orientedOffsetPlane.getXAxis() * amountSnapped.x +
            _orientedOffsetPlane.getYAxis() * amountSnapped.y;

        return snappedOffset;
    }

    void translationService::getObbLimitsOnOrientedOffsetPlane(vec3 offset, vec2& minimum, vec2& maximum)
    {
        auto destinationObb = _nodeTranslator->getNodeDestinationObb(_clickedNode);
        destinationObb.center += offset;
        auto obbCorners = destinationObb.getCorners();
        minimum = _orientedOffsetPlane.projectPoint(obbCorners[0]);
        maximum = minimum;
        for (auto i = 1; i < 8; ++i)
        {
            auto projectedCorner = _orientedOffsetPlane.projectPoint(obbCorners[i]);

            if (projectedCorner.x < minimum.x)
                minimum.x = projectedCorner.x;
            if (projectedCorner.y < minimum.y)
                minimum.y = projectedCorner.y;

            if (projectedCorner.x > maximum.x)
                maximum.x = projectedCorner.x;
            if (projectedCorner.y > maximum.y)
                maximum.y = projectedCorner.y;
        }
    }

    void translationService::addClippingPlanes()
    {
        _lastTranslationTouchingCollisions = findTouchingCollisionsOnDirection(-_offsetPlane.normal, DECIMAL_TRUNCATION * 1.5f);
        auto validCollisions = filterValidTouchCollisions(_lastTranslationTouchingCollisions);

        for (auto& touch : validCollisions)
        {
            auto collideePlanes = touch.collidee->getObb().getPlanes();
            for (auto& collideePlane : collideePlanes)
            {
                if (!isPlaneVisible(collideePlane))
                    continue;

                if (collideePlane.isParallel(_offsetPlane))
                    continue;

                auto colliderObb = touch.colliderObb;
                auto leavingPosition = colliderObb->getPositionAt(-collideePlane.normal);
                auto distanceFromPlane = collideePlane.distanceFrom(leavingPosition);
                if (distanceFromPlane < -0.4f)
                    continue;

                auto isAlreadyClipped = false;
                for (auto& clippedTranslationPlane : _clippedTranslationPlanes[colliderObb])
                {
                    if (clippedTranslationPlane->getCollidee() == touch.collidee &&
                        clippedTranslationPlane->getPlane().isParallel(collideePlane))
                    {
                        isAlreadyClipped = true;
                        break;
                    }
                }

                if (isAlreadyClipped)
                    continue;

                auto clippedTranslationPlane = createTranslationPlane(collideePlane);
                clippedTranslationPlane->setCollidee(touch.collidee);
                _clippedTranslationPlanes[colliderObb].push_back(clippedTranslationPlane);

                auto clippingPlane = _currentTranslationPlane->getClippingPlane();
                auto clippedPlaneGrid = clippedTranslationPlane->getPlaneGrid();
                clippedPlaneGrid->addClippingPlane(clippingPlane);
                clippedPlaneGrid->setClippingPlaneDistance(clippingPlane, clippingDistance::negative);
                clippedPlaneGrid->setClippingPlaneOpacity(clippingPlane, 0.0f);
                clippedPlaneGrid->setVisibilityFactor(1.0f);
                auto opacity = 1.0f - glm::clamp(distanceFromPlane / -0.4f, 0.0f, 1.0f);
                clippedPlaneGrid->setOpacity(opacity);
                _layer->add(clippedTranslationPlane->getPlaneGridNode());
                clippedPlaneGrid->show();

                auto currentPlaneGrid = _currentTranslationPlane->getPlaneGrid();
                auto currentClippingPlane = clippedTranslationPlane->getClippingPlane();
                currentPlaneGrid->addClippingPlane(currentClippingPlane);
                currentPlaneGrid->setClippingPlaneDistance(currentClippingPlane, clippingDistance::negative);
                currentPlaneGrid->setClippingPlaneOpacity(currentClippingPlane, 0.0f);
            }
        }
    }

    void translationService::updateClippingPlanes()
    {
        for (auto& pair : _clippedTranslationPlanes)
        {
            for (auto iterator = pair.second.begin(); iterator != pair.second.end();)
            {
                auto clippedTranslationPlane = *iterator;
                auto colliderObb = pair.first;
                auto collideePlane = clippedTranslationPlane->getPlane();
                auto leavingPosition = colliderObb->getPositionAt(-collideePlane.normal);
                auto distanceFromPlane = collideePlane.distanceFrom(leavingPosition);
                if (distanceFromPlane < -0.4f)
                {
                    auto currentClippingPlane = clippedTranslationPlane->getClippingPlane();
                    _currentTranslationPlane->getPlaneGrid()->removeClippingPlane(currentClippingPlane);
                    enqueuePlaneForDeletion(clippedTranslationPlane);
                    iterator = pair.second.erase(iterator);
                }
                else
                {
                    auto opacity = 1.0f - glm::clamp(distanceFromPlane / -0.4f, 0.0f, 1.0f);
                    clippedTranslationPlane->getPlaneGrid()->setOpacity(opacity);
                    ++iterator;
                }
            }
        }
    }

    vec3 translationService::tryChangeToAttachedPlane(vec3 offset)
    {
        if (!_canChangePlanes)
            return vec3();

        //auto touchCollisions = findTouchingCollisionsOnDirection(-_offsetPlane.normal, 2.5f * DECIMAL_TRUNCATION);
        auto validCollisions = filterValidTouchCollisions(_lastTranslationTouchingCollisions);

        for (auto& touch : validCollisions)
        {
            auto translatedColliderObb = obb(*touch.colliderObb);
            translatedColliderObb.center += offset;

            auto collideePlanes = touch.collidee->getObb().getPlanes();
            for (auto& collideePlane : collideePlanes)
            {
                if (!isPlaneVisible(collideePlane))
                    continue;

                if (collideePlane.isParallel(_offsetPlane))
                    continue;

                auto leavingPosition = translatedColliderObb.getPositionAt(-collideePlane.normal);
                auto leavingDistance = collideePlane.distanceFrom(leavingPosition);

                if (leavingDistance > 0.0f)
                {
                    auto projectedLeavingPosition = collideePlane.projectPoint(leavingPosition);
                    projectedLeavingPosition += collideePlane.normal * DECIMAL_TRUNCATION;

                    auto offsetPlane = plane(_offsetPlane.origin, collideePlane.normal);
                    auto translationPlane = createTranslationPlane(collideePlane);
                    translationPlane->setCollidee(touch.collidee);
                    translationPlane->setCollider(touch.collider);
                    changePlanes(translationPlane, offsetPlane);

                    auto returnToPlaneOffset = projectedLeavingPosition - leavingPosition;
                    return returnToPlaneOffset;
                }
            }
        }

        return vec3();
    }

    void translationService::tryChangeToPlanesFromCollisions()
    {
        if (!_canChangePlanes)
            return;

        auto translationResultedCollisions = _nodeTranslator->getLastTranslationTouchingCollisions();

        for (auto& touch : *translationResultedCollisions)
        {
            if (!isNormalValidForCollision(touch))
                continue;

            auto touchPlaneOrigin = touch.collidee->getObb().getPositionAt(touch.normal);
            auto touchPlane = phi::plane(touchPlaneOrigin, touch.normal);

            touchPlane.origin = touchPlane.projectPoint(_offsetPlane.origin);

            if (canChangeTo(touchPlane))
            {
                auto translationPlane = createTranslationPlane(touchPlane);
                translationPlane->setCollidee(touch.collidee);
                translationPlane->setCollider(touch.collider);
                auto offsetPlane = plane(_offsetPlane.origin, touchPlane.normal);
                changePlanes(translationPlane, offsetPlane);
            }
        }
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

    bool translationService::isNormalValidForCollision(const sweep::sweepCollision& touch)
    {
        for (auto& collideePlane : touch.collidee->getObb().getPlanes())
        {
            if (mathUtils::isClose(glm::dot(collideePlane.normal, touch.normal), 1.0f))
                return true;
        }

        for (auto& colliderPlane : touch.collider->getObb().getPlanes())
        {
            if (mathUtils::isClose(glm::dot(colliderPlane.normal, -touch.normal), 1.0f))
                return true;
        }

        return false;
    }

    void translationService::translateTargetNodes(const vec3 offset)
    {
        if (offset == vec3())
            return;

        auto resolvedOffset = _nodeTranslator->translate(offset);

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
            _collidedDelta = offset - resolvedOffset;
        else
            _collidedDelta = vec3();

        updateDestinationObbs();
    }

    void translationService::updateDestinationObbs()
    {
        auto targetNodesCount = _targetNodes->size();
        for (auto i = 0; i < targetNodesCount; ++i)
        {
            auto targetNode = _targetNodes->at(i);
            auto destinationObb = _nodeTranslator->getNodeDestinationObb(targetNode);
            _targetNodesDestinationObbs.at(i)->set(destinationObb);
        }
    }

    void translationService::updateSnapGridSize()
    {
//        const float planeDistances[] = { 0.0f, 0.60f, 1.5f, 30.0f, 200.0f, 5000.0f };
 //       const float gridSizes[] = { 0.01f, 0.1f, 1.0f, 10.0f, 100.0f, 1000.0f };
        const float planeDistances[] = {  0.0f, 1.00f, 4.0f, 30.0f, 200.0f, 2000.0f };
        const float gridSizes[] =      { 0.01f, 0.1f, 1.0f, 10.0f, 100.0f, 1000.0f };
        
        auto planeNodePosition = _currentTranslationPlane->getPlaneGridNode()->getTransform()->getPosition();
        auto planeNodePositionRelativeToCamera = mathUtils::multiply(_camera->getViewMatrix(), planeNodePosition);
        auto distance = glm::abs(planeNodePositionRelativeToCamera.z);

        if (distance > planeDistances[4])
            _snapGridSize = gridSizes[4];
        else if (distance > planeDistances[3])
            _snapGridSize = gridSizes[3];
        else if (distance > planeDistances[2])
            _snapGridSize = gridSizes[2];
        else if (distance > planeDistances[1])
            _snapGridSize = gridSizes[1];
        else
            _snapGridSize = gridSizes[0];
    }

    void translationService::translatePlaneGrid(const vec3& targetPosition)
    {
        auto planeGridPosition = _currentTranslationPlane->getPlane().projectPoint(targetPosition);
        _currentTranslationPlane->animatePlaneGridPosition(planeGridPosition);
    }

    vector<sweep::sweepCollision> translationService::findTouchingCollisions()
    {
        auto sweepTest = sweep::groupToSceneTest();
        sweepTest.colliders = _nodeTranslator->getPiledUpColliders();
        sweepTest.parameters.direction = vec3(1.0f, 0.0f, 0.0f);
        sweepTest.parameters.distance = 0.0f;
        sweepTest.parameters.disregardDivergentNormals = false;
        sweepTest.parameters.inflation = 2.5f * DECIMAL_TRUNCATION;

        auto sweepResult = _physicsWorld->sweep(sweepTest);
        if (!sweepResult.collided)
            return vector<sweep::sweepCollision>();

        return sweepResult.collisions;
    }

    vector<sweep::sweepObbCollision> translationService::findTouchingCollisionsOnDirection(const vec3& direction, float distance)
    {
        auto sweepTest = sweep::groupToSceneObbTest();
        sweepTest.colliders = &_targetNodesDestinationObbs;
        sweepTest.parameters.direction = direction;
        sweepTest.parameters.distance = distance;
        sweepTest.parameters.disregardDivergentNormals = false;

        auto sweepResult = _physicsWorld->sweep(sweepTest);
        if (!sweepResult.collided)
            return vector<sweep::sweepObbCollision>();

        return sweepResult.collisions;
    }

    vector<sweep::sweepObbCollision> translationService::filterValidTouchCollisions(vector<sweep::sweepObbCollision>& touchs)
    {
        auto validTouchs = vector<sweep::sweepObbCollision>();

        if (_currentTranslationPlane == nullptr)
            return validTouchs;

        for (auto& touch : touchs)
        {
            auto touchPlane = plane(_currentTranslationPlane->getPlane().origin, touch.normal);
            if (isPlaneVisible(touchPlane))
                validTouchs.push_back(touch);
        }

        return validTouchs;
    }

    vector<sweep::sweepCollision> translationService::filterValidTouchCollisions(vector<sweep::sweepCollision>& touchs)
    {
        auto validTouchs = vector<sweep::sweepCollision>();

        if (_currentTranslationPlane == nullptr)
            return validTouchs;

        for (auto& touch : touchs)
        {
            auto touchPlane = plane(_currentTranslationPlane->getPlane().origin, touch.normal);
            if (isPlaneVisible(touchPlane))
                validTouchs.push_back(touch);
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

        for (auto& obb : _targetNodesDestinationObbs)
            safeDelete(obb);

        _targetNodesDestinationObbs.clear();

        enqueuePlaneForDeletion(_currentTranslationPlane);

        deleteClippedPlanes();
        _ghostTranslator->clear();
        _nodeTranslator->clear();
    }

    void translationService::deleteClippedPlanes()
    {
        for (auto& pair : _clippedTranslationPlanes)
        {
            for (auto& translationPlane : pair.second)
            {
                _currentTranslationPlane->getPlaneGrid()->removeClippingPlane(translationPlane->getClippingPlane());
                enqueuePlaneForDeletion(translationPlane);
            }
        }

        _clippedTranslationPlanes.clear();
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
            for (auto& clippedTranslationPlane : pair.second)
                updatePlaneVisibility(clippedTranslationPlane);
        }

        updateSnapGridSize();
        updatePlaneVisibility(_currentTranslationPlane);
        deletePlaneIfNotVisible();
    }

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

    void translationService::disableSnapToGrid()
    {
        _isSnapToGridEnabled = false;
        _snappedDelta = vec3();
    }

    void translationService::enableSnapToGrid()
    {
        _isSnapToGridEnabled = true;
    }
}