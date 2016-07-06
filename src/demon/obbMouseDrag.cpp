#include <precompiled.h>
#include <apps\application.h>
#include <loader\importer.h>
#include "obbMouseDrag.h"

using namespace phi;

namespace demon
{
    obbMouseDrag::obbMouseDrag(scene* scene) :
        mouseDrag(scene),
        _dragging(false),
        _dragCollider(nullptr),
        _planeGridPass(scene->getRenderer()->getPlaneGridRenderPass())
    {
        auto texturePath = application::resourcesPath + "\\images\\grid.png";
        _gridImage = importer::importImage(texturePath);
        _planeGridPass->setImage(_gridImage);
    }

    obbMouseDrag::~obbMouseDrag()
    {
        safeDelete(_gridImage);
    }

    void obbMouseDrag::startDrag(int mouseX, int mouseY)
    {
        auto pickMesh = _scene->pick(mouseX, mouseY);
        if (!pickMesh)
            return;

        auto node = pickMesh->getNode();
        auto collider = node->getComponent<boxCollider>();
        auto obb = collider->getObb();

        vec3* positions;
        vec3* normals;
        size_t count;
        auto ray = _scene->getCamera()->screenPointToRay(static_cast<float>(mouseX), static_cast<float>(mouseY));
        if (ray.intersects(obb, positions, normals, count))
        {
            _dragCollider = collider;
            auto dragObject = node;
            while (dragObject->getParent()->getParent() != nullptr)
                dragObject = dragObject->getParent();

            auto normal = normals[0];
            auto rayCastOnPlanePosition = positions[0];

            phi::safeDeleteArray(normals);
            phi::safeDeleteArray(positions);

            auto dragPlane = plane(rayCastOnPlanePosition, normal);

            auto planePosition = obb.getPositionAt(-normal);
            planePosition = glm::normalize(planePosition) * (glm::length(planePosition) + DECIMAL_TRUNCATION);

            _planeGridPass->setPositionAndOrientation(planePosition, normal);
            _planeGridPass->projectAndSetFocusPosition(planePosition);
            _planeGridPass->show();

            _dragging = true;

            _object = dragObject;
            _plane = dragPlane;

            _colliders = vector<boxCollider*>();
            _transforms = vector<transform*>();
            _object->traverse<boxCollider>([this](boxCollider* b)
            {
                _colliders.push_back(b);
                _transforms.push_back(b->getNode()->getTransform());
            });

            _initialPlanePosition = dragPlane.getOrigin();
            _initialObjectPosition = _object->getTransform()->getLocalPosition();
        }

        mouseDrag::startDrag(mouseX, mouseY);
    }

    void obbMouseDrag::drag(int mouseX, int mouseY)
    {
        if (!_object || !_dragging)
            return;

        auto ray = _scene->getCamera()->screenPointToRay(static_cast<float>(mouseX), static_cast<float>(mouseY));
        float t;
        ray.intersects(_plane, t);
        auto rayCastOnPlanePosition = ray.getOrigin() + ray.getDirection() * t;

        auto offsetOnPlane = rayCastOnPlanePosition - _initialPlanePosition;
        auto finalPosition = _initialObjectPosition + offsetOnPlane;

        moveObject(finalPosition - _object->getTransform()->getLocalPosition());

        _planeGridPass->projectAndAnimateFocusPosition(_dragCollider->getObb().center);
    }

    void obbMouseDrag::endDrag()
    {
        if (!_dragging)
            return;

        _dragging = false;
        _planeGridPass->hide();

        mouseDrag::endDrag();
    }
}