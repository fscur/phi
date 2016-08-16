#include <precompiled.h>

#include <ui\planeGrid.h>

#include <core\node.h>
#include <core\boxCollider.h>
#include <core\mesh.h>
#include <core\ghostMesh.h>
#include "translationInputController.h"

namespace phi
{
    translationInputController::translationInputController(camera* camera, layer* layer) :
        inputController(),
        _camera(camera),
        _collisionNodeTranslator(nullptr),
        _layer(layer),
        _dragging(false),
        _draggingRootNode(nullptr),
        _draggingCollider(nullptr),
        _initialObjectPosition(),
        _disableCollision(false),
        _lastTranslationTouchs(nullptr),
        _lastMousePosition(vec2()),
        _showingGhost(false)
    {
    }

    translationInputController::~translationInputController()
    {
        safeDelete(_collisionNodeTranslator);
    }

    void translationInputController::setNodeToTranslate(node* node)
    {
        _dragging = true;
        _draggingCollider = node->getComponent<boxCollider>();
        _draggingRootNode = node;
        while (_draggingRootNode->getParent()->getParent() != nullptr)
            _draggingRootNode = _draggingRootNode->getParent();
    }

    void translationInputController::initializeNodeTranslators()
    {
        if (_collisionNodeTranslator)
        {
            _collisionNodeTranslator->setNode(_draggingRootNode);
            _collisionNodeTranslator->beginTranslations();
        }
    }

    void translationInputController::endNodeTranslators()
    {
        if (_collisionNodeTranslator)
            _collisionNodeTranslator->endTranslations();
    }

    void translationInputController::setupTranslationPlane(translationPlane* translationPlane)
    {
        _initialObjectPosition = _draggingRootNode->getTransform()->getLocalPosition();

        if (_collisionNodeTranslator)
            _collisionNodeTranslator->setPlane(translationPlane->plane);
    }

    void translationInputController::deletePlane(translationPlane* translationPlane)
    {
        translationPlane->planeGridNode->getParent()->removeChild(translationPlane->planeGridNode);
        safeDelete(translationPlane->planeGridNode);
    }

    translationPlane* translationInputController::createTranslationPlane(plane plane, vec3 position, boxCollider* collider, color color)
    {
        auto planeNode = new node("plane");
        auto planeTransform = planeNode->getTransform();
        planeTransform->setLocalPosition(position);
        planeTransform->setDirection(plane.normal);

        auto animator = new phi::animator();
        auto animation = new transformAnimation(planeTransform, easingFunctions::easeOutCubic);
        animator->addAnimation(animation);

        auto planeGrid = new phi::planeGrid();
        planeGrid->setColor(color);
        planeGrid->setLineThickness(7.0f);
        planeGrid->setOpacity(0.4f);
        planeNode->addComponent(planeGrid);
        planeNode->addComponent(animator);

        auto translationPlane = new phi::translationPlane(plane);
        translationPlane->collider = collider;
        translationPlane->planeGridNode = planeNode;
        translationPlane->planeGridAnimation = animation;

        return translationPlane;
    }

    vec3 translationInputController::getTranslationPosition(ivec2 mousePosition, translationPlane* translationPlane)
    {
        auto rayCastOnPlanePosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, translationPlane->plane);

        auto offsetOnPlane = rayCastOnPlanePosition - translationPlane->plane.origin;
        return _initialObjectPosition + offsetOnPlane;
    }

    void translationInputController::translateNode(vec3 offset)
    {
        if (_collisionNodeTranslator && !_disableCollision)
        {
            _collisionNodeTranslator->translateNode(offset);
            _lastTranslationTouchs = _collisionNodeTranslator->getLastTranslationTouchingCollisions();
        }
        else
            _draggingRootNode->getTransform()->translate(offset);
    }

    void translationInputController::translatePlaneGrid(translationPlane* translationPlane)
    {
        auto planeNode = translationPlane->planeGridNode;
        auto animation = translationPlane->planeGridAnimation;

        auto planeTransform = planeNode->getTransform();
        auto plane = phi::plane(planeTransform->getPosition(), planeTransform->getDirection());

        auto fromPlaneTransform = new transform();
        auto fromPosition = planeTransform->getLocalPosition();
        fromPlaneTransform->setLocalPosition(fromPosition);

        auto toPlaneTransform = new transform();
        auto toPosition = plane.projectPoint(_draggingCollider->getObb().center);
        toPlaneTransform->setLocalPosition(toPosition);

        animation->start(fromPlaneTransform, toPlaneTransform, 0.33);
    }

    node* translationInputController::cloneNodeAsGhost(node* node)
    {
        auto nodeTransform = node->getTransform();
        auto position = nodeTransform->getLocalPosition();
        auto size = nodeTransform->getLocalSize();
        auto orientation = nodeTransform->getLocalOrientation();

        auto clonedNode = new phi::node(node->getName());
        auto clonedNodeTransform = clonedNode->getTransform();

        clonedNodeTransform->setLocalPosition(position);
        clonedNodeTransform->setLocalSize(size);
        clonedNodeTransform->setLocalOrientation(orientation);

        auto mesh = node->getComponent<phi::mesh>();

        if (mesh)
        {
            auto geometry = mesh->getGeometry();
            auto material = mesh->getMaterial();
            auto ghostMesh = new phi::ghostMesh(geometry, material);
            clonedNode->addComponent(ghostMesh);
        }

        for (auto& child : *node->getChildren())
        {
            auto clonedChild = cloneNodeAsGhost(child);
            clonedNode->addChild(clonedChild);
        }

        return clonedNode;
    }

    bool translationInputController::onMouseDown(mouseEventArgs* e)
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
        auto mousePosition = ivec2(e->x, e->y);

        vector<rayIntersection> rayIntersections;
        auto ray = _camera->screenPointToRay(mousePosition.x, mousePosition.y);
        if (ray.intersects(obb, rayIntersections))
        {
            auto firstIntersection = rayIntersections[0];
            auto obbCastNormal = firstIntersection.normal;
            auto obbCastPosition = firstIntersection.position;

            setNodeToTranslate(node);

            _draggingGhostNode = cloneNodeAsGhost(_draggingRootNode);

            initializeNodeTranslators();

            auto plane = phi::plane(obbCastPosition, obbCastNormal);
            _defaultTranslationPlane = createTranslationPlane(plane, _draggingCollider->getObb().getPositionAt(-obbCastNormal), nullptr);
            _layer->add(_defaultTranslationPlane->planeGridNode);
            _defaultTranslationPlane->showGrid();

            _lastMousePosition = mousePosition;
            setupTranslationPlane(_defaultTranslationPlane);

            //ShowCursor(false);

            return true;
        }

        return false;
    }

    bool translationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        auto mousePosition = ivec2(e->x, e->y);
        auto position = getTranslationPosition(mousePosition, _defaultTranslationPlane);
        auto offset = position - _draggingRootNode->getTransform()->getLocalPosition();

        translateNode(offset);

        if (!_showingGhost && position != _draggingRootNode->getTransform()->getLocalPosition())
        {
            _layer->add(_draggingGhostNode);
            _showingGhost = true;
            _draggingRootNode->traverse([](phi::node* node) {
                node->setIsTranslating(true);
            });
        }
        else if(_showingGhost && position == _draggingRootNode->getTransform()->getLocalPosition())
        {
            _draggingGhostNode->getParent()->removeChild(_draggingGhostNode);
            
            _draggingRootNode->traverse([](phi::node* node) {
                node->setIsTranslating(false);
            });

            _showingGhost = false;
        }

        if (_showingGhost)
        {
            _draggingGhostNode->traverse<phi::ghostMesh>([=](phi::ghostMesh* ghostMesh)
            {
                ghostMesh->setOffset(offset);
            });
            
            _draggingGhostNode->getTransform()->setLocalPosition(position);
        }

        translatePlaneGrid(_defaultTranslationPlane);

        _lastMousePosition = mousePosition;

        return true;
    }

    bool translationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed || !_dragging)
            return false;

        _dragging = false;

        deletePlane(_defaultTranslationPlane);
        _defaultTranslationPlane = nullptr;

        

        if (_showingGhost)
        {
            _draggingGhostNode->getParent()->removeChild(_draggingGhostNode);
            _showingGhost = false;
            safeDelete(_draggingGhostNode);
        }

        endNodeTranslators();

        //ShowCursor(true);

        return true;
    }

    bool translationInputController::onKeyDown(keyboardEventArgs* e)
    {
        if (e->key == PHIK_CTRL)
        {
            _disableCollision = true;
            return true;
        }

        return false;
    }

    bool translationInputController::onKeyUp(keyboardEventArgs* e)
    {
        if (e->key == PHIK_CTRL)
        {
            _disableCollision = false;
            return true;
        }

        return false;
    }
}