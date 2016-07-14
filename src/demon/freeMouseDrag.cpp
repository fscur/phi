#include <precompiled.h>
#include <apps\application.h>
#include <loader\importer.h>
#include "freeMouseDrag.h"

using namespace phi;

namespace demon
{
    freeMouseDrag::freeMouseDrag(scene* scene) :
        mouseDrag(scene),
        _dragging(false),
        _dragCollider(nullptr),
        _planeGridPass(scene->getRenderer()->getPlaneGridRenderPass())
    {
        auto texturePath = application::resourcesPath + "\\images\\grid.png";
        _gridImage = importer::importImage(texturePath);
        _planeGridPass->setImage(_gridImage);
    }

    freeMouseDrag::~freeMouseDrag()
    {
        safeDelete(_gridImage);
    }

    void freeMouseDrag::showPlaneGrid(vec3 position, color color)
    {
        _planeGridPass->setPositionAndOrientation(position, _plane.getNormal());
        _planeGridPass->setFocusPosition(phi::vec2());
        _planeGridPass->setColor(color);
        _planeGridPass->show();
    }

    void freeMouseDrag::setPlane(plane plane)
    {
        _plane = plane;
        _initialObjectPosition = _object->getTransform()->getLocalPosition();
        _dragging = true;
    }

    void freeMouseDrag::initializeDragObjectData(node* node)
    {
        _dragCollider = node->getComponent<boxCollider>();
        _object = node;
        while (_object->getParent()->getParent() != nullptr)
            _object = _object->getParent();

        _colliders = vector<boxCollider*>();
        _transforms = vector<transform*>();
        _object->traverse<boxCollider>([this](boxCollider* b)
        {
            _colliders.push_back(b);
            _transforms.push_back(b->getNode()->getTransform());
        });
    }

    void freeMouseDrag::startDrag(int mouseX, int mouseY)
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
            auto normal = normals[0];
            auto obbCastPosition = positions[0];

            initializeDragObjectData(node);
            setPlane(plane(obbCastPosition, normal));
            //showPlaneGrid(obb.getPositionAt(-_plane.getNormal()), color::white);
            showPlaneGrid(obbCastPosition, color::white);

            phi::safeDeleteArray(normals);
            phi::safeDeleteArray(positions);
        }

        mouseDrag::startDrag(mouseX, mouseY);
    }

    void freeMouseDrag::drag(int mouseX, int mouseY)
    {
        if (!_object || !_dragging)
            return;

        auto ray = _scene->getCamera()->screenPointToRay(static_cast<float>(mouseX), static_cast<float>(mouseY));
        float t;
        ray.intersects(_plane, t);
        auto rayCastOnPlanePosition = ray.getOrigin() + ray.getDirection() * t;

        auto offsetOnPlane = rayCastOnPlanePosition - _plane.getOrigin();
        auto finalPosition = _initialObjectPosition + offsetOnPlane;

        moveObject(finalPosition - _object->getTransform()->getLocalPosition());

        _planeGridPass->projectAndAnimateFocusPosition(_dragCollider->getObb().center);
    }

    void freeMouseDrag::endDrag()
    {
        if (!_dragging)
            return;

        _dragging = false;
        _planeGridPass->hide();

        mouseDrag::endDrag();
    }
}