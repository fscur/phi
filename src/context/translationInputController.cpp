#include <precompiled.h>

#include <core\node.h>
#include <core\boxCollider.h>

#include <io\path.h>
#include <loader\importer.h>
#include <ui\planeGrid.h>
#include <application\application.h>

#include "translationInputController.h"

namespace phi
{
    translationInputController::translationInputController(camera* camera, layer* planesLayer) :
        inputController(),
        _camera(camera),
        _nodeTranslator(new nodeTranslator()),
        _collisionNodeTranslator(nullptr),
        _planesLayer(planesLayer),
        _planeImage(nullptr),
        _dragging(false),
        _draggingRootNode(nullptr),
        _draggingCollider(nullptr),
        _translationPlane(nullptr),
        _initialObjectPosition(),
        _disableCollision(false),
        _translationPlanes(vector<translationPlane*>()),
        _lastMousePosition(vec2())
    {
        auto planeImagePath = path::combine(application::resourcesPath, "images\\grid.bmp");
        _planeImage = importer::importImage(planeImagePath);
    }

    translationInputController::~translationInputController()
    {
        safeDelete(_nodeTranslator);
        safeDelete(_collisionNodeTranslator);
        safeDelete(_planeImage);
    }

    void translationInputController::startDrag(node* node)
    {
        _dragging = true;
        _draggingCollider = node->getComponent<boxCollider>();
        _draggingRootNode = node;
        while (_draggingRootNode->getParent()->getParent() != nullptr)
            _draggingRootNode = _draggingRootNode->getParent();

        startNodeTranslators();
    }

    void translationInputController::startNodeTranslators()
    {
        _nodeTranslator->setNode(_draggingRootNode);
        _nodeTranslator->beginTranslations();
        if (_collisionNodeTranslator)
        {
            _collisionNodeTranslator->setNode(_draggingRootNode);
            _collisionNodeTranslator->beginTranslations();
        }
    }

    void translationInputController::endNodeTranslators()
    {
        _nodeTranslator->endTranslations();
        if (_collisionNodeTranslator)
            _collisionNodeTranslator->endTranslations();
    }

    void translationInputController::setPlane(translationPlane* translationPlane)
    {
        if (_translationPlane == translationPlane)
            return;

        if (_translationPlane)
            translationPlane->plane.origin = castRayToPlane(_lastMousePosition);

        _translationPlane = translationPlane;
        _initialObjectPosition = _draggingRootNode->getTransform()->getLocalPosition();

        if (_collisionNodeTranslator)
            _collisionNodeTranslator->setPlane(translationPlane->plane);
    }

    vec3 translationInputController::castRayToPlane(vec2 screenPosition)
    {
        auto ray = _camera->screenPointToRay(screenPosition.x, screenPosition.y);
        float t;
        ray.intersects(_translationPlane->plane, t);
        return ray.getOrigin() + ray.getDirection() * t;
    }

    void translationInputController::deletePlane(translationPlane* translationPlane)
    {
        auto planeNode = translationPlane->planeGridNode;

        removePlaneGrid(translationPlane);
        safeDelete(planeNode);

        //TODO:fix it
    }

    void translationInputController::clearPlanes()
    {
        for (auto& translationPlane : _translationPlanes)
            deletePlane(translationPlane);
        _translationPlanes.clear();

        deletePlane(_defaultTranslationPlane);
    }

    translationPlane* translationInputController::createTranslationPlane(plane plane, vec3 position, boxCollider* collider)
    {
        auto planeNode = new node("plane");
        auto planeTransform = planeNode->getTransform();
        planeTransform->setLocalPosition(position);
        planeTransform->setDirection(plane.normal);

        auto animator = new phi::animator();
        auto animation = new transformAnimation(planeTransform, easingFunctions::easeOutCubic);
        animator->addAnimation(animation);

        auto planeGrid = new phi::planeGrid();
        planeGrid->setImage(_planeImage);
        planeNode->addComponent(planeGrid);
        planeNode->addComponent(animator);

        auto translationPlane = new phi::translationPlane(plane);
        translationPlane->collider = collider;
        translationPlane->planeGridNode = planeNode;
        translationPlane->planeGridAnimation = animation;

        return translationPlane;
    }

    void translationInputController::addTranslationPlane(translationPlane* translationPlane)
    {
        _translationPlanes.push_back(translationPlane);
        if (_translationPlanes.size() == 1)
            removePlaneGrid(_defaultTranslationPlane);
    }

    void translationInputController::removeTranslationPlane(translationPlane* translationPlane)
    {
        phi::removeIfContains(_translationPlanes, translationPlane);
        if (_translationPlanes.size() == 0)
            addPlaneGrid(_defaultTranslationPlane);
    }

    void translationInputController::addPlaneGrid(translationPlane* translationPlane)
    {
        auto planeNode = translationPlane->planeGridNode;
        _planesLayer->add(planeNode);
    }

    void translationInputController::removePlaneGrid(translationPlane* translationPlane)
    {
        auto planeNode = translationPlane->planeGridNode;
        planeNode->getParent()->removeChild(planeNode);
    }

    vec3 translationInputController::getTranslationOffset(vec2 mousePosition)
    {
        auto rayCastOnPlanePosition = castRayToPlane(mousePosition);

        auto offsetOnPlane = rayCastOnPlanePosition - _translationPlane->plane.origin;
        auto finalPosition = _initialObjectPosition + offsetOnPlane;

        return finalPosition - _draggingRootNode->getTransform()->getLocalPosition();
    }

    void translationInputController::translateNode(vec3 offset)
    {
        if (_collisionNodeTranslator && !_disableCollision)
            _collisionNodeTranslator->translateNode(offset);
        else
            _nodeTranslator->translateNode(offset);
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

    void translationInputController::translatePlaneGrids()
    {
        for (auto translationPlane : _translationPlanes)
            translatePlaneGrid(translationPlane);

        translatePlaneGrid(_defaultTranslationPlane);
    }

    vec3 translationInputController::mouseOffsetToWorld(vec2 mousePosition)
    {
        auto mouseDir = glm::normalize(_lastMousePosition - mousePosition);
        auto cameraTransform = _camera->getTransform();
        return glm::normalize(cameraTransform->getRight() * mouseDir.x + cameraTransform->getUp() * mouseDir.y);
    }

    translationPlane* translationInputController::findBestPlaneToDrag(vec3 dragDirection)
    {
        auto minNormalOnDragDirection = 1.0f;
        translationPlane* chosenPlane = nullptr;

        for (auto& translationPlane : _translationPlanes)
        {
            if (translationPlane == _defaultTranslationPlane)
                continue;

            auto normalOnDragDirection = glm::dot(translationPlane->plane.normal, dragDirection);
            if (normalOnDragDirection < minNormalOnDragDirection)
            {
                minNormalOnDragDirection = normalOnDragDirection;
                chosenPlane = translationPlane;
            }
        }

        if (!chosenPlane)
            chosenPlane = _defaultTranslationPlane;

        return chosenPlane;
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
        auto mousePosition = vec2(static_cast<float>(e->x), static_cast<float>(e->y));

        vector<rayIntersection> rayIntersections;
        auto ray = _camera->screenPointToRay(mousePosition.x, mousePosition.y);
        if (ray.intersects(obb, rayIntersections))
        {
            auto firstIntersection = rayIntersections[0];
            auto obbCastNormal = firstIntersection.normal;
            auto obbCastPosition = firstIntersection.position;

            startDrag(node);

            auto plane = phi::plane(obbCastPosition, obbCastNormal);
            _defaultTranslationPlane = createTranslationPlane(plane, _draggingCollider->getObb().getPositionAt(-obbCastNormal), nullptr);

            _lastMousePosition = mousePosition;

            setPlane(_defaultTranslationPlane);
            addPlaneGrid(_defaultTranslationPlane);

            return true;
        }

        return false;
    }

    bool translationInputController::onMouseMove(mouseEventArgs* e)
    {
        if (!_dragging)
            return false;

        auto mousePosition = vec2(static_cast<float>(e->x), static_cast<float>(e->y));
        auto worldMouseOffset = mouseOffsetToWorld(mousePosition);
        auto translationPlane = findBestPlaneToDrag(worldMouseOffset);
        setPlane(translationPlane);

        auto offset = getTranslationOffset(mousePosition);

        translateNode(offset);
        translatePlaneGrids();

        _lastMousePosition = mousePosition;

        return true;
    }

    bool translationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed || !_dragging)
            return false;

        _dragging = false;
        _translationPlane = nullptr;

        endNodeTranslators();
        clearPlanes();

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