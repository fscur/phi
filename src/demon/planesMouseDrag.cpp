#include <precompiled.h>

#include "planesMouseDrag.h"

using namespace phi;

namespace demon
{
    planesMouseDrag::planesMouseDrag(phi::scene* scene) :
        obbMouseDrag(scene),
        _lastMousePosition(),
        _planeSource(nullptr),
        _originalPlane(vec3(), vec3())
    {
    }

    void planesMouseDrag::startDrag(int mouseX, int mouseY)
    {
        obbMouseDrag::startDrag(mouseX, mouseY);
        _lastMousePosition = vec2(mouseX, mouseY);
        _planeSource = nullptr;
        _originalPlane = _plane;
        _planeOffsetToObject = _object->getTransform()->getLocalPosition() - _plane.getOrigin();
    }

    void planesMouseDrag::drag(int mouseX, int mouseY)
    {
        if (!_dragging)
            return;

        intersectionCollisionMultiTest touchTest;
        touchTest.colliders = &_colliders;
        touchTest.transforms = &_transforms;

        auto result = _scene->getPhysicsWorld()->touchs(touchTest);

        if (result.collided)
        {
            auto cameraTransform = _scene->getCamera()->getTransform();
            auto mouseDir = glm::normalize(vec2(static_cast<float>(mouseX - _lastMousePosition.x), static_cast<float>(_lastMousePosition.y - mouseY)));
            auto a = cameraTransform->getRight() * mouseDir.x + cameraTransform->getUp() * mouseDir.y;
            auto worldMouseDir = vec3(a.x, a.y, 0.0f);

            bool found = false;
            for (auto collision : result.collisions)
            {
                auto normal = collision.normal;
                auto dot = glm::dot(normal, worldMouseDir);
                if (dot <= 0.95f)
                {
                    if (collision.collider != _planeSource)
                    {
                        auto ray = _scene->getCamera()->screenPointToRay(static_cast<float>(mouseX), static_cast<float>(mouseY));
                        float t;
                        ray.intersects(_plane, t);
                        auto rayCastOnPlanePosition = ray.getOrigin() + ray.getDirection() * t;
                        _initialPlanePosition = rayCastOnPlanePosition;
                        _initialObjectPosition = _object->getTransform()->getLocalPosition();
                        _plane = plane(rayCastOnPlanePosition, normal);

                        auto planePosition = _dragCollider->getObb().getPositionAt(-normal);
                        planePosition = glm::normalize(planePosition) * (glm::length(planePosition) + DECIMAL_TRUNCATION);

                        _planeGridPass->setPositionAndOrientation(planePosition, normal);
                        _planeGridPass->projectAndSetFocusPosition(planePosition);
                        _planeGridPass->show();
                        _planeSource = collision.collider;
                    }

                    found = true;
                    break;
                }
            }

            if (!found && _planeSource != nullptr)
            {
                _plane = _originalPlane;
                auto ray = _scene->getCamera()->screenPointToRay(static_cast<float>(mouseX), static_cast<float>(mouseY));
                float t;
                ray.intersects(_plane, t);
                auto rayCastOnPlanePosition = ray.getOrigin() + ray.getDirection() * t;

                auto planePosition = _dragCollider->getObb().getPositionAt(-_plane.getNormal());
                planePosition = glm::normalize(planePosition) * (glm::length(planePosition) + DECIMAL_TRUNCATION);

                _initialPlanePosition = rayCastOnPlanePosition;
                _initialObjectPosition = _object->getTransform()->getLocalPosition();
                _plane = plane(_object->getTransform()->getLocalPosition() + _planeOffsetToObject, _originalPlane.getNormal());

                _planeGridPass->setPositionAndOrientation(planePosition, _plane.getNormal());
                _planeGridPass->projectAndSetFocusPosition(planePosition);
                _planeGridPass->show();
                _planeSource = nullptr;
            }
        }

        obbMouseDrag::drag(mouseX, mouseY);
        _lastMousePosition = vec2(mouseX, mouseY);
    }

    void planesMouseDrag::endDrag()
    {
        obbMouseDrag::endDrag();
    }
}