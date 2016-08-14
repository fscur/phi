#include <precompiled.h>

#include <ui\planeGrid.h>

#include <core\node.h>
#include <core\boxCollider.h>

#include "translationInputController.h"

namespace phi
{
    translationInputController::translationInputController(camera* camera, layer* planesLayer) :
        inputController(),
        _camera(camera),
        _collisionNodeTranslator(nullptr),
        _planesLayer(planesLayer),
        _dragging(false),
        _draggingRootNode(nullptr),
        _draggingCollider(nullptr),
        _initialObjectPosition(),
        _disableCollision(false),
        _lastMousePosition(vec2()),
        _lastTranslationTouchs(nullptr)
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

    vec3 translationInputController::getTranslationOffset(ivec2 mousePosition, translationPlane* translationPlane)
    {
        auto rayCastOnPlanePosition = _camera->castRayToPlane(mousePosition.x, mousePosition.y, translationPlane->plane);

        auto offsetOnPlane = rayCastOnPlanePosition - translationPlane->plane.origin;
        auto finalPosition = _initialObjectPosition + offsetOnPlane;

        return finalPosition - _draggingRootNode->getTransform()->getLocalPosition();
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
            initializeNodeTranslators();

            auto plane = phi::plane(obbCastPosition, obbCastNormal);
            _defaultTranslationPlane = createTranslationPlane(plane, _draggingCollider->getObb().getPositionAt(-obbCastNormal), nullptr);
            _planesLayer->add(_defaultTranslationPlane->planeGridNode);
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
        auto offset = getTranslationOffset(mousePosition, _defaultTranslationPlane);

        translateNode(offset);
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