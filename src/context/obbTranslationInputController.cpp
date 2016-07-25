#include <precompiled.h>

#include <core\node.h>
#include <core\boxCollider.h>

#include <io\path.h>
#include <loader\importer.h>
#include <ui\planeGrid.h>
#include <application\application.h>

#include "obbTranslationInputController.h"

namespace phi
{
    obbTranslationInputController::obbTranslationInputController(camera* camera, layer* planesLayer) :
        inputController(),
        _camera(camera),
        _nodeTranslator(new nodeTranslator()),
        _collisionNodeTranslator(nullptr),
        _planesLayer(planesLayer),
        _dragging(false),
        _draggingCollider(nullptr),
        _draggingRootNode(nullptr),
        _plane(vec3(), vec3()),
        _initialObjectPosition(),
        _disableCollision(false)
    {
        auto planeImagePath = path::combine(application::resourcesPath, "images\\grid.bmp");
        _planeImage = importer::importImage(planeImagePath);
    }

    obbTranslationInputController::~obbTranslationInputController()
    {
        safeDelete(_nodeTranslator);
        safeDelete(_collisionNodeTranslator);
        safeDelete(_planeImage);
    }

    void obbTranslationInputController::initializeDragData(node* node)
    {
        _draggingCollider = node->getComponent<boxCollider>();
        _draggingRootNode = node;
        while (_draggingRootNode->getParent()->getParent() != nullptr)
            _draggingRootNode = _draggingRootNode->getParent();

        _nodeTranslator->setNode(_draggingRootNode);
        if (_collisionNodeTranslator)
            _collisionNodeTranslator->setNode(_draggingRootNode);
    }

    void obbTranslationInputController::setPlane(plane plane)
    {
        _plane = plane;
        _initialObjectPosition = _draggingRootNode->getTransform()->getLocalPosition();
        _dragging = true;

        if (_collisionNodeTranslator)
            _collisionNodeTranslator->setPlane(_plane);
    }

    void obbTranslationInputController::clearPlaneGrids()
    {
        for (auto node : _planeNodes)
        {
            node->getParent()->removeChild(node);
            safeDelete(node);
        }

        _planeNodes.clear();
    }

    void obbTranslationInputController::addPlaneGrid(vec3 position, color color)
    {
        auto planeNode = new node("plane");
        auto planeTransform = planeNode->getTransform();
        planeTransform->setLocalPosition(position);
        planeTransform->setDirection(_plane.getNormal());
        
        auto planeGrid = new phi::planeGrid();
        planeGrid->setImage(_planeImage);
        planeNode->addComponent(planeGrid);
        _planeNodes.push_back(planeNode);
        _planesLayer->add(planeNode);

        //_planeGridPass->setPositionAndOrientation(position, _plane.getNormal());
        //_planeGridPass->setFocusPosition(phi::vec2());
        //_planeGridPass->setColor(color);
        //_planeGridPass->show();
    }

    void obbTranslationInputController::updatePlaneGrids()
    {
        for (auto planeNode : _planeNodes)
        {
            auto planeTransform = planeNode->getTransform();
            auto plane = phi::plane(planeTransform->getPosition(), planeTransform->getDirection());
            
            planeTransform->setLocalPosition(plane.projectPoint(_draggingCollider->getObb().center));
        }
    }

    bool obbTranslationInputController::onMouseDown(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed)
            return false;

        auto idOnMousePosition = pickingFramebuffer::pick(e->x, e->y);
        auto clickComponent = pickingId::get(idOnMousePosition);

        if (!clickComponent)
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
            addPlaneGrid(obb.getPositionAt(-_plane.getNormal()), color::white);

            phi::safeDeleteArray(normals);
            phi::safeDeleteArray(positions);

            _nodeTranslator->beginTranslations();
            if (_collisionNodeTranslator)
                _collisionNodeTranslator->beginTranslations();

            return true;
        }

        return false;
    }

    bool obbTranslationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        auto ray = _camera->screenPointToRay(static_cast<float>(e->x), static_cast<float>(e->y));
        float t;
        ray.intersects(_plane, t);
        auto rayCastOnPlanePosition = ray.getOrigin() + ray.getDirection() * t;

        auto offsetOnPlane = rayCastOnPlanePosition - _plane.getOrigin();
        auto finalPosition = _initialObjectPosition + offsetOnPlane;

        auto offset = finalPosition - _draggingRootNode->getTransform()->getLocalPosition();

        if (_collisionNodeTranslator && !_disableCollision)
            _collisionNodeTranslator->translateNode(offset);
        else
            _nodeTranslator->translateNode(offset);

        updatePlaneGrids();
        //_planeGridPass->projectAndAnimateFocusPosition(_dragCollider->getObb().center);

        return true;
    }

    bool obbTranslationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed || !_dragging)
            return false;

        _dragging = false;
        //_planeGridPass->hide();

        _nodeTranslator->endTranslations();
        if (_collisionNodeTranslator)
            _collisionNodeTranslator->endTranslations();

        clearPlaneGrids();

        return true;
    }

    bool obbTranslationInputController::onKeyDown(keyboardEventArgs* e)
    {
        if (e->key == PHIK_CTRL)
        {
            _disableCollision = true;
            return true;
        }

        return false;
    }

    bool obbTranslationInputController::onKeyUp(keyboardEventArgs* e)
    {
        if (e->key == PHIK_CTRL)
        {
            _disableCollision = false;
            return true;
        }

        return false;
    }
}