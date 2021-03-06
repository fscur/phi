#include <precompiled.h>
#include <core/boxCollider.h>
#include <core/ghostMesh.h>
#include <core/mesh.h>
#include <core/plane.h>
#include <core/string.h>
#include <animation/animator.h>
#include <physics/intersection/groupToSceneTest.h>
#include <physics/sweep/sweepCollision.h>
#include <physics/sweep/groupToSceneTest.h>

#include "rotationService.h"

namespace phi
{
    rotationService::rotationService(
        const vector<node*>* targetNodes,
        layer* layer,
        physicsWorld* physicsWorld) :
        _targetNodes(targetNodes),
        _layer(layer),
        _camera(layer->getCamera()),
        _physicsWorld(physicsWorld),
        _nodeRotator(new collisionNodeRotator(physicsWorld)),
        _ghostTranslator(new ghostNodeTranslator(layer)),
        _isRotating(false),
        _lastMousePosition(ivec2()),
        _rotationLastPosition(vec3()),
        _currentRotationPlane(nullptr),
        _planesToDelete(vector<rotationPlane*>()),
        _currentPlane(plane()),
        _lastAngle(0.0f),
        _lastCollidedAngle(0.0f),
        _usageMode(rotationUsageMode::ROTATE_AT_CENTROID)
    {
    }

    rotationService::~rotationService()
    {
        safeDelete(_nodeRotator);
        safeDelete(_ghostTranslator);
    }

    void rotationService::setUsageMode(rotationUsageMode value)
    {
        _usageMode = value;
        if (_usageMode == rotationUsageMode::ROTATE_AT_INDIVIDUAL_ORIGINS)
            _nodeRotator->enableSelfCollision();
        else
            _nodeRotator->disableSelfCollision();
    }

    void rotationService::startRotation(ivec2 mousePosition, node* clickedNode)
    {
        _clickedNode = clickedNode;
        _currentRotationPlane = nullptr;
        _isRotating = true;
        _lastMousePosition = mousePosition;
        _lastAngle = 0.0f;
        _lastCollidedAngle = 0.0f;
        _nodeRotator->addRange(*_targetNodes);
        _ghostTranslator->addRange(*_targetNodes);

        createPlane();

        _currentPlane = _currentRotationPlane->getPlane();
        _rotationLastPosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, _currentPlane);
    }

    void rotationService::createPlane()
    {
        auto rotationPlane = createAxisAlignedRotationPlane(_lastMousePosition);

        changePlane(rotationPlane);
        showRotationPlane();
    }

    rotationPlane* rotationService::createAxisAlignedRotationPlane(ivec2 mousePosition)
    {
        auto viewDirection = -_camera->screenPointToRay(mousePosition.x, mousePosition.y).getDirection();

        vec3 axis;
        if (_targetNodes->size() == 1)
        {
            auto targetNodeObb = _targetNodes->at(0)->getObb();
            auto obbAxes = vector<vec3>
            {
                targetNodeObb->axes[0],
                targetNodeObb->axes[1],
                targetNodeObb->axes[2],
                -targetNodeObb->axes[0],
                -targetNodeObb->axes[1],
                -targetNodeObb->axes[2]
            };

            axis = mathUtils::getClosestAxisTo(viewDirection, obbAxes);
        }
        else
            axis = mathUtils::getClosestAxisTo(viewDirection);

        auto axisPlane = createPlaneFromAxis(axis);

        vec3 worldPosition;
        switch (_usageMode)
        {
            case rotationUsageMode::ROTATE_AT_CENTROID:
                worldPosition = getTargetNodesCentroid(); break;
            case rotationUsageMode::ROTATE_AT_MOUSE_POSITION:
                worldPosition = _camera->screenPointToWorld(mousePosition); break;
            case rotationUsageMode::ROTATE_AT_INDIVIDUAL_ORIGINS:
                worldPosition = _clickedNode->getTransform()->getPosition(); break;
        }

        auto origin = axisPlane.projectPoint(worldPosition);
        return createRotationPlane(plane(origin, axisPlane.normal));
    }

    vec3 rotationService::getTargetNodesCentroid()
    {
        vector<vec3> targetNodesPositions;

        for (auto& node : *_targetNodes)
            targetNodesPositions.push_back(node->getTransform()->getPosition());

        return mathUtils::getCentroid(targetNodesPositions);
    }

    plane rotationService::createPlaneFromAxis(const vec3& axis)
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

    rotationPlane* rotationService::createRotationPlane(const plane& plane)
    {
        auto planeGridNode = new node("plane");
        auto planeTransform = planeGridNode->getTransform();

        planeTransform->setLocalPosition(plane.origin);
        planeTransform->setDirection(plane.normal);

        auto animator = new phi::animator();

        auto planeGrid = new phi::rotationPlaneGrid();
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

        auto translationPlane = new phi::rotationPlane(plane);
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

    void rotationService::changePlane(rotationPlane* rotationPlane)
    {
        if (_currentRotationPlane)
            enqueuePlaneForDeletion(_currentRotationPlane);

        _currentRotationPlane = rotationPlane;

        if (_targetNodes->size() == 1)
            setStartingRotatedAngleAndPlaneOrientation();
        else
            _lastAngle = _lastCollidedAngle = 0.0f;

        updateRotationPlaneFilledAngle();

        _currentPlane.normal = _currentRotationPlane->getPlane().normal;
        _currentPlane.origin = _currentPlane.projectPoint(_currentRotationPlane->getPlane().origin);
        _rotationLastPosition = _camera->castRayToPlane(_lastMousePosition.x, _lastMousePosition.y, _currentPlane);
    }

    void rotationService::setStartingRotatedAngleAndPlaneOrientation()
    {
        auto targetNodeTransform = _targetNodes->at(0)->getTransform();
        auto targetNodeOrientation = targetNodeTransform->getOrientation();

        auto planeNormal = _currentRotationPlane->getPlane().normal;
        auto planeNormalWorldAxis = glm::inverse(targetNodeOrientation) * planeNormal;
        planeNormalWorldAxis = glm::abs(mathUtils::getClosestAxisTo(planeNormalWorldAxis));

        vec3 planeRight = vec3();
        vec3 angleAxis = vec3();
        if (planeNormalWorldAxis == vec3(1.0f, 0.0f, 0.0f))
        {
            angleAxis = vec3(0.0f, 1.0f, 0.0f);
            planeRight = -targetNodeTransform->getDirection();
        }
        else if (planeNormalWorldAxis == vec3(0.0f, 1.0f, 0.0f))
        {
            angleAxis = vec3(0.0f, 0.0f, 1.0f);
            planeRight = targetNodeTransform->getRight();
        }
        else if (planeNormalWorldAxis == vec3(0.0f, 0.0f, 1.0f))
        {
            angleAxis = vec3(1.0f, 0.0f, 0.0f);
            planeRight = targetNodeTransform->getRight();
        }

        auto rotationToWorldAxis = mathUtils::rotationBetweenVectors(planeNormal, planeNormalWorldAxis);
        auto rotatedAngleAxis = rotationToWorldAxis * targetNodeOrientation * angleAxis;

        auto rotatedAngle = glm::orientedAngle(angleAxis, rotatedAngleAxis, planeNormalWorldAxis);

        auto planeUp = glm::cross(planeNormal, planeRight);
        auto planeOrientation = quat(mat3(planeRight, planeUp, planeNormal));

        auto angleRotation = glm::angleAxis(-rotatedAngle, planeNormal);
        planeOrientation = angleRotation * planeOrientation;

        _currentRotationPlane->getPlaneGridNode()->getTransform()->setLocalOrientation(planeOrientation);

        _lastAngle = _lastCollidedAngle = rotatedAngle;
    }

    void rotationService::enqueuePlaneForDeletion(rotationPlane* planeToRemove)
    {
        auto fadeOutAnimationEnded = [=]
        {
            _planesToDelete.push_back(planeToRemove);
        };

        planeToRemove->fadeGridOpacityOut(fadeOutAnimationEnded);
    }

    void rotationService::showRotationPlane()
    {
        _layer->add(_currentRotationPlane->getPlaneGridNode());
        _currentRotationPlane->showGrid();
    }

    void rotationService::rotate(ivec2 mousePosition)
    {
        if (!_isRotating)
            return;

        addMouseMovementDeltaAngle(mousePosition);
        rotateTargetNodes();
        rotateGhostNodes();
        updateRotationPlaneFilledAngle();
    }

    void rotationService::addMouseMovementDeltaAngle(ivec2 mousePosition)
    {
        auto endPosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, _currentPlane);
        auto originToStart = glm::normalize(_rotationLastPosition - _currentPlane.origin);
        auto originToEnd = glm::normalize(endPosition - _currentPlane.origin);
        auto deltaAngle = glm::orientedAngle(originToStart, originToEnd, _currentPlane.normal);

        _lastAngle += deltaAngle;

        _lastMousePosition = mousePosition;
        _rotationLastPosition = endPosition;
    }

    void rotationService::rotateTargetNodes()
    {
        float collidedDeltaAngle;
        float angleDifference = _lastAngle - _lastCollidedAngle;

        if (_usageMode == rotationUsageMode::ROTATE_AT_INDIVIDUAL_ORIGINS)
            collidedDeltaAngle = _nodeRotator->rotate(angleDifference, _currentPlane);
        else
            collidedDeltaAngle = _nodeRotator->orbit(angleDifference, _currentPlane);

        _lastCollidedAngle += collidedDeltaAngle;
    }

    void rotationService::rotateGhostNodes()
    {
        auto shouldShowGhost = _lastCollidedAngle != _lastAngle;
        if (shouldShowGhost)
        {
            _ghostTranslator->enable();

            auto ghostDeltaAngle = _lastAngle - _lastCollidedAngle;

            if (_usageMode == rotationUsageMode::ROTATE_AT_INDIVIDUAL_ORIGINS)
                _ghostTranslator->rotate(ghostDeltaAngle, _currentPlane);
            else
                _ghostTranslator->orbit(ghostDeltaAngle, _currentPlane);
        }
        else
            _ghostTranslator->disable();
    }

    void rotationService::updateRotationPlaneFilledAngle()
    {
        auto filledAngle = mathUtils::counterClockwiseAngle(mathUtils::normalizeAngle(_lastAngle));
        _currentRotationPlane->getPlaneGrid()->setFilledAngle(filledAngle);
    }

    void rotationService::endRotation()
    {
        _isRotating = false;
        enqueuePlaneForDeletion(_currentRotationPlane);

        _nodeRotator->clear();
        _ghostTranslator->clear();
    }

    void rotationService::update()
    {
        for (auto& planeToDelete : _planesToDelete)
            deletePlane(planeToDelete);

        _planesToDelete.clear();

        if (!_isRotating)
            return;

        updatePlaneVisibility(_currentRotationPlane);
        deletePlaneIfNotVisible();
    }

    void rotationService::deletePlane(rotationPlane* rotationPlane)
    {
        rotationPlane->hideGrid();
        auto planeGridNode = rotationPlane->getPlaneGridNode();
        planeGridNode->getParent()->removeChild(planeGridNode);
        safeDelete(rotationPlane);
    }

    void rotationService::updatePlaneVisibility(rotationPlane* rotationPlane)
    {
        auto visibility = getPlaneVisibility(rotationPlane->getPlane());

        rotationPlane->updatePlaneGridVisibilityFactor(visibility);
    }

    float rotationService::getPlaneVisibility(const plane& plane)
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

    bool rotationService::isPlaneVisible(const plane& plane)
    {
        float planeVisibility = getPlaneVisibility(plane);
        return planeVisibility > getExtinctionFactor(plane.normal);
    }

    float rotationService::getExtinctionFactor(const vec3& normal)
    {
        return mathUtils::isParallel(vec3(0.0, 1.0, 0.0), normal) ? 0.3f : 0.4f;
    }

    void rotationService::deletePlaneIfNotVisible()
    {
        if (!isPlaneVisible(_currentRotationPlane->getPlane()))
        {
            enqueuePlaneForDeletion(_currentRotationPlane);
            createPlane();
        }
    }

    void rotationService::disableCollisions()
    {
        _nodeRotator->disableCollisions();
    }

    void rotationService::enableCollisions()
    {
        _nodeRotator->enableCollisions();
    }
}