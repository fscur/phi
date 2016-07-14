#include <precompiled.h>

#include "planesMouseDrag.h"

using namespace phi;

namespace demon
{
    planesMouseDrag::planesMouseDrag(phi::scene* scene) :
        freeMouseDrag(scene),
        _lastMousePosition(),
        _currentTouchingCollider(nullptr),
        _originalPlane(vec3(), vec3())
    {
    }

    planesMouseDrag::touchingCollisionResult planesMouseDrag::findTouchingCollision(vec3 dragDirection)
    {
        auto cameraDirection = glm::normalize(_scene->getCamera()->getTransform()->getDirection());
        auto cameraPosition = _scene->getCamera()->getTransform()->getPosition();
        touchingCollisionResult touchResult;

        auto minDot = 1.0f;

        intersectionCollisionMultiTest touchTest;
        touchTest.colliders = &_colliders;
        touchTest.transforms = &_transforms;

        auto result = _scene->getPhysicsWorld()->touchs(touchTest);
        if (result.collided)
        {
            for (auto collision : result.collisions)
            {
                auto normal = collision.normal;
                auto planePosition = _dragCollider->getObb().getPositionAt(-collision.normal);
                auto toPlaneDir = glm::normalize(planePosition - cameraPosition);
                auto discardedPlaneDot = glm::abs(glm::dot(normal, toPlaneDir));

                if (_currentTouchingCollider != collision.collider && discardedPlaneDot <= 0.2f)
                    continue;

                auto chosenPlaneDot = glm::dot(normal, dragDirection);

                if (chosenPlaneDot < minDot)
                {
                    minDot = chosenPlaneDot;
                    touchResult.collider = collision.collider;
                    touchResult.normal = collision.normal;
                    touchResult.foundValidCollision = true;
                }
            }
        }

        touchResult.collisionsCount = static_cast<uint>(result.collisions.size());
        return touchResult;
    }

    void planesMouseDrag::startDrag(int mouseX, int mouseY)
    {
        freeMouseDrag::startDrag(mouseX, mouseY);

        if (!_dragging)
            return;

        _lastMousePosition = vec2(mouseX, mouseY);
        _currentTouchingCollider = nullptr;
        _originalPlane = _plane;
        _planeOffsetToObject = _object->getTransform()->getLocalPosition() - _plane.getOrigin();

        touchingCollisionResult touchCollision = findTouchingCollision(glm::vec3(1.0f, 0.0f, 0.0f));
        if (touchCollision.foundValidCollision)
        {
            _currentTouchingCollider = touchCollision.collider;
            _initialObjectPosition = _object->getTransform()->getLocalPosition();
            setPlane(plane(_plane.getOrigin(), touchCollision.normal));

            auto planePosition = _dragCollider->getObb().getPositionAt(-touchCollision.normal);
            showPlaneGrid(planePosition, color::fromRGBA(0.4f, 0.4f, 1.0f, 1.0f));
        }

        //moveObject(vec3(0.5f, -0.5f, 0.0f));
    }

    void planesMouseDrag::drag(int mouseX, int mouseY)
    {
        if (!_dragging)
            return;

        auto mouseDir = glm::normalize(vec2(static_cast<float>(_lastMousePosition.x - mouseX), static_cast<float>(_lastMousePosition.y - mouseY)));
        auto cameraTransform = _scene->getCamera()->getTransform();
        auto mouseOffsetWorld = glm::normalize(cameraTransform->getRight() * mouseDir.x + cameraTransform->getUp() * mouseDir.y);

        touchingCollisionResult touchCollision = findTouchingCollision(mouseOffsetWorld);
        if (touchCollision.foundValidCollision)
        {
            if (_currentTouchingCollider != touchCollision.collider)
            {
                auto castPosition = castRayToPlane(_lastMousePosition);

                _initialObjectPosition = _object->getTransform()->getLocalPosition();
                setPlane(plane(castPosition, touchCollision.normal));
                auto planePosition = _dragCollider->getObb().getPositionAt(-touchCollision.normal);
                showPlaneGrid(planePosition, color::fromRGBA(0.4f, 0.4f, 1.0f, 1.0f));

                _currentTouchingCollider = touchCollision.collider;
            }
        }
        else
        {
            if (_currentTouchingCollider != nullptr && touchCollision.collisionsCount > 0u)
            {
                auto castPosition = castRayToPlane(_lastMousePosition);

                _initialObjectPosition = _object->getTransform()->getLocalPosition();
                setPlane(plane(castPosition, _originalPlane.getNormal()));
                auto planePosition = _dragCollider->getObb().getPositionAt(-_originalPlane.getNormal());
                showPlaneGrid(planePosition, color::white);

                _currentTouchingCollider = nullptr;
            }
        }

        freeMouseDrag::drag(mouseX, mouseY);
        _lastMousePosition = vec2(mouseX, mouseY);
    }

    void planesMouseDrag::endDrag()
    {
        freeMouseDrag::endDrag();
        _currentTouchingCollider = nullptr;
    }
}