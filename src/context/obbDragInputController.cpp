#include <precompiled.h>

#include <core\node.h>
#include <core\boxCollider.h>

#include "obbDragInputController.h"

namespace phi
{
    obbDragInputController::obbDragInputController(camera* camera) :
        inputController(),
        _camera(camera),
        _draggingCollider(nullptr),
        _draggingRootNode(nullptr),
        _dragging(false),
        _plane(vec3(), vec3()),
        _initialObjectPosition()
    {
    }

    void obbDragInputController::initializeDragData(node* node)
    {
        _draggingCollider = node->getComponent<boxCollider>();
        _draggingRootNode = node;
        while (_draggingRootNode->getParent()->getParent() != nullptr)
            _draggingRootNode = _draggingRootNode->getParent();

        //_colliders = vector<boxCollider*>();
        //_transforms = vector<transform*>();
        //_object->traverse<boxCollider>([this](boxCollider* b)
        //{
        //    _colliders.push_back(b);
        //    _transforms.push_back(b->getNode()->getTransform());
        //});
    }

    void obbDragInputController::setPlane(plane plane)
    {
        _plane = plane;
        _initialObjectPosition = _draggingRootNode->getTransform()->getLocalPosition();
        _dragging = true;
    }

    void obbDragInputController::showPlaneGrid(vec3 position, color color)
    {
        //_planeGridPass->setPositionAndOrientation(position, _plane.getNormal());
        //_planeGridPass->setFocusPosition(phi::vec2());
        //_planeGridPass->setColor(color);
        //_planeGridPass->show();
    }

    bool obbDragInputController::onMouseDown(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed)
            return false;

        auto idOnMousePosition = pickingFramebuffer::pick(e->x, e->y);
        auto clickComponent = pickingId::get(idOnMousePosition);

        if (clickComponent == nullptr)
            return false;

        auto node = clickComponent->getNode();
        auto collider = node->getComponent<boxCollider>();
        auto obb = collider->getObb();

        vec3* positions;
        vec3* normals;
        size_t count;
        auto ray = _camera->screenPointToRay(static_cast<float>(e->x), static_cast<float>(e->y));
        if (ray.intersects(obb, positions, normals, count))
        {
            auto normal = normals[0];
            auto obbCastPosition = positions[0];

            initializeDragData(node);
            setPlane(plane(obbCastPosition, normal));
            showPlaneGrid(obb.getPositionAt(-_plane.getNormal()), color::white);

            phi::safeDeleteArray(normals);
            phi::safeDeleteArray(positions);
        }

        return true;
    }

    bool obbDragInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        auto ray = _camera->screenPointToRay(static_cast<float>(e->x), static_cast<float>(e->y));
        float t;
        ray.intersects(_plane, t);
        auto rayCastOnPlanePosition = ray.getOrigin() + ray.getDirection() * t;

        auto offsetOnPlane = rayCastOnPlanePosition - _plane.getOrigin();
        auto finalPosition = _initialObjectPosition + offsetOnPlane;

        //moveObject(finalPosition - _object->getTransform()->getLocalPosition());
        _draggingRootNode->getTransform()->setLocalPosition(finalPosition);

        //_planeGridPass->projectAndAnimateFocusPosition(_dragCollider->getObb().center);

        return true;
    }

    bool obbDragInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed || !_dragging)
            return false;

        _dragging = false;
        //_planeGridPass->hide();
        return true;
    }
}