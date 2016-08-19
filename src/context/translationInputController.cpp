#include <precompiled.h>

#include <ui\planeGrid.h>

#include <core\node.h>
#include <core\boxCollider.h>
#include <core\mesh.h>
#include <core\ghostMesh.h>
#include <core\plane.h>

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
        _showingGhost(false),
        _shouldDeleteDefaultTranslationPlane(false)
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
    }

    void translationInputController::initializeNodeTranslators()
    {
        if (_collisionNodeTranslator)
        {
            _collisionNodeTranslator->setNode(_draggingRootNode);

            for (auto& collider : *_collisionNodeTranslator->getColliders())
                collider->disable();
        }
    }

    void translationInputController::endNodeTranslators()
    {
        if (_collisionNodeTranslator)
        {
            for (auto& collider : *_collisionNodeTranslator->getColliders())
                collider->enable();
        }
    }

    void translationInputController::setupTranslationPlane(translationPlane* translationPlane)
    {
        _initialObjectPosition = _draggingRootNode->getTransform()->getLocalPosition();

        if (_collisionNodeTranslator)
            _collisionNodeTranslator->setPlane(translationPlane->getMousePlane());
    }

    void translationInputController::deletePlane(translationPlane* translationPlane)
    {
        auto planeGridNode = translationPlane->getPlaneGridNode();
        planeGridNode->getParent()->removeChild(planeGridNode);
        safeDelete(planeGridNode);
    }

    translationPlane* translationInputController::createTranslationPlane(
        plane mousePlane,
        vec3 position,
        boxCollider* collidee,
        boxCollider* collider,
        color color)
    {
        auto planeNode = new node("plane");
        auto planeTransform = planeNode->getTransform();
        planeTransform->setLocalPosition(position);
        planeTransform->setDirection(mousePlane.normal);

        auto animator = new phi::animator();

        auto planeGrid = new phi::planeGrid();
        planeGrid->setColor(color);
        planeGrid->setLineThickness(8.5f);

        planeNode->addComponent(planeGrid);
        planeNode->addComponent(animator);

        auto draggingAnimation = new phi::transformAnimation(planeTransform, easingFunctions::easeOutCubic);
        animator->addAnimation(draggingAnimation);

        auto fadeUpdadeFunction = [=](float value)
        {
            planeGrid->setOpacity(value);
        };

        auto fadeInAnimation = new phi::floatAnimation();
        fadeInAnimation->setUpdateFunction(fadeUpdadeFunction);
        animator->addAnimation(fadeInAnimation);

        auto fadeOutAnimation = new phi::floatAnimation();
        fadeOutAnimation->setUpdateFunction(fadeUpdadeFunction);
        animator->addAnimation(fadeOutAnimation);

        auto clippingPlanesFadeOutAnimation = new phi::floatAnimation();
        animator->addAnimation(clippingPlanesFadeOutAnimation);

        auto gridPlane = plane(position, mousePlane.normal);

        auto translationPlane = new phi::translationPlane(mousePlane);
        translationPlane->setGridPlane(gridPlane);
        translationPlane->setCollidee(collidee);
        translationPlane->setCollider(collider);
        translationPlane->setPlaneGridNode(planeNode);
        translationPlane->setDraggingAnimation(draggingAnimation);
        translationPlane->setFadeInAnimation(fadeInAnimation);
        translationPlane->setFadeOutAnimation(fadeOutAnimation);
        translationPlane->setClippingPlanesFadeOutAnimation(clippingPlanesFadeOutAnimation);

        return translationPlane;
    }

    vec3 translationInputController::getTranslationPosition(ivec2 mousePosition, translationPlane* translationPlane)
    {
        auto rayCastOnPlanePosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, translationPlane->getMousePlane());

        auto offsetOnPlane = rayCastOnPlanePosition - translationPlane->getMousePlane().origin;
        return _initialObjectPosition + offsetOnPlane;
    }

    void translationInputController::translateNode(vec3 offset)
    {
        assert(offset != vec3());

        if (_collisionNodeTranslator && !_disableCollision)
        {
            _collisionNodeTranslator->translateNode(offset);

            _lastTranslationTouchs = _collisionNodeTranslator->getLastTranslationTouchingCollisions();
        }
        else
            _draggingRootNode->getTransform()->translate(offset);
    }

    void translationInputController::translatePlaneGrid(translationPlane* translationPlane, ivec2 mousePosition)
    {
        auto planeNode = translationPlane->getPlaneGridNode();

        auto planeTransform = planeNode->getTransform();
        auto plane = phi::plane(planeTransform->getPosition(), planeTransform->getDirection());
        auto castPosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, translationPlane->getMousePlane());

        auto fromPlaneTransform = new transform();
        auto fromPosition = planeTransform->getLocalPosition();
        fromPlaneTransform->setLocalPosition(fromPosition);

        auto toPlaneTransform = new transform();
        auto toPosition = plane.projectPoint(castPosition);
        toPlaneTransform->setLocalPosition(toPosition);

        auto draggingAnimation = translationPlane->getDraggingAnimation();
        draggingAnimation->start(fromPlaneTransform, toPlaneTransform, 0.33);
    }

    void translationInputController::translateGhost(vec3 position, vec3 offset)
    {
        auto isGhostOnTheSamePositionAsObject = position == _draggingRootNode->getTransform()->getLocalPosition();

        if (!_showingGhost && !isGhostOnTheSamePositionAsObject)
        {
            _showingGhost = true;
            _layer->add(_draggingGhostNode);
            
            _draggingRootNode->traverse([](phi::node* node) {
                node->setIsTranslating(true);
            });
        }
        else if (_showingGhost && isGhostOnTheSamePositionAsObject)
        {
            _showingGhost = false;
            _draggingGhostNode->getParent()->removeChild(_draggingGhostNode);

            _draggingRootNode->traverse([](phi::node* node) {
                node->setIsTranslating(false);
            });
        }

        if (_showingGhost)
        {
            _draggingGhostNode->traverse<phi::ghostMesh>([=](phi::ghostMesh* ghostMesh)
            {
                ghostMesh->setOffset(offset);
            });

            _draggingGhostNode->getTransform()->setLocalPosition(position);
        }
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
        auto rootNode = node;
        while (rootNode->getParent()->getParent() != nullptr)
            rootNode = rootNode->getParent();

        auto collider = rootNode->getComponent<boxCollider>();
        assert(collider != nullptr);

        auto obb = collider->getObb();
        auto mousePosition = ivec2(e->x, e->y);

        vector<rayIntersection> rayIntersections;
        auto ray = _camera->screenPointToRay(mousePosition.x, mousePosition.y);
        auto intersected = ray.intersects(obb, rayIntersections);
        assert(intersected);

        auto firstIntersection = rayIntersections[0];
        auto obbCastNormal = firstIntersection.normal;
        auto obbCastPosition = firstIntersection.position;

        setNodeToTranslate(rootNode);

        _draggingGhostNode = cloneNodeAsGhost(rootNode);

        initializeNodeTranslators();

        auto plane = phi::plane(obbCastPosition, obbCastNormal);
        auto gridPlane = phi::plane(_draggingCollider->getObb().getPositionAt(-obbCastNormal), obbCastNormal);
        auto gridPosition = gridPlane.projectPoint(obbCastPosition);

        _defaultTranslationPlane = createTranslationPlane(plane, gridPosition, nullptr, nullptr, color::fromRGBA(0.5f, 0.6f, 0.7f, 1.0f));
        _layer->add(_defaultTranslationPlane->getPlaneGridNode());

        _defaultTranslationPlane->showGrid();

        _lastMousePosition = mousePosition;
        setupTranslationPlane(_defaultTranslationPlane);

        //ShowCursor(false);

        return true;
    }

    bool translationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        auto mousePosition = ivec2(e->x, e->y);
        auto position = getTranslationPosition(mousePosition, _defaultTranslationPlane);
        auto offset = position - _draggingRootNode->getTransform()->getLocalPosition();

        if (offset == vec3())
        {
            return false; //ARRUMAR BUG LAST MOUSE POS
        }

        translateNode(offset);
        translatePlaneGrid(_defaultTranslationPlane, mousePosition);
        translateGhost(position, offset);

        _lastMousePosition = mousePosition;

        return true;
    }

    void translationInputController::startPlaneRemoval(translationPlane* planeToRemove, std::function<void(animation*)> fadeOutAnimationEnded)
    {
        auto fadeOutAnimationEndedEventHandler = planeToRemove->getFadeOutAnimation()->getAnimationEnded();
        fadeOutAnimationEndedEventHandler->assign(fadeOutAnimationEnded);
        planeToRemove->hideGrid();
    }

    bool translationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed || !_dragging)
            return false;

        _dragging = false;

        auto fadeOutAnimationEnded = [=](animation* animation)
        {
            _shouldDeleteDefaultTranslationPlane = true;
        };

        startPlaneRemoval(_defaultTranslationPlane, fadeOutAnimationEnded);

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

    bool translationInputController::update()
    {
        if (_shouldDeleteDefaultTranslationPlane)
        {
            deletePlane(_defaultTranslationPlane);
            _defaultTranslationPlane = nullptr;
            _shouldDeleteDefaultTranslationPlane = false;
        }

        return false;
    }
}