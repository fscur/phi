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
        _currentRotationPlane(nullptr),
        _isRotating(false)
    {
    }

    rotationService::~rotationService()
    {
    }

    void rotationService::startRotation(ivec2 mousePosition)
    {
        _currentRotationPlane = nullptr;
        _isRotating = true;
        _lastMousePosition = mousePosition;
        _offsetPlaneOrigin = _camera->screenPointToWorld(mousePosition.x, mousePosition.y);

        createPlanes();
    }

    void rotationService::createPlanes()
    {
        auto rotationPlane = createAxisAlignedRotationPlane(_lastMousePosition);
        auto offsetPlane = plane(_offsetPlaneOrigin, rotationPlane->getPlane().normal);

        changePlanes(rotationPlane, offsetPlane);
        showRotationPlane();
    }

    rotationPlane* rotationService::createAxisAlignedRotationPlane(ivec2 mousePosition)
    {
        auto viewDirection = -_camera->screenPointToRay(mousePosition.x, mousePosition.y).getDirection();
        auto worldAxis = mathUtils::getClosestAxisTo(viewDirection);
        auto axisPlane = createPlaneFromAxis(worldAxis);

        auto worldPosition = _camera->screenPointToWorld(mousePosition.x, mousePosition.y);
        auto origin = axisPlane.projectPoint(worldPosition);

        return createRotationPlane(plane(origin, axisPlane.normal));
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

    void rotationService::changePlanes(rotationPlane* rotationPlane, const plane& offsetPlane)
    {
        if (_currentRotationPlane)
            enqueuePlaneForDeletion(_currentRotationPlane);

        _currentRotationPlane = rotationPlane;
        _offsetPlane = offsetPlane;
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
    }

    void rotationService::endRotation()
    {
        _isRotating = false;
        enqueuePlaneForDeletion(_currentRotationPlane);
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
            createPlanes();
        }
    }

    void rotationService::disableCollisions()
    {
        
    }

    void rotationService::enableCollisions()
    {
        
    }
}