#include <precompiled.h>

#include "translationInputController.h"

#include <animation\animator.h>
#include <rendering\pickingFramebuffer.h>

#include "pickingId.h"

namespace phi
{
    translationInputController::translationInputController(const vector<node*>* targetNodes, layer* layer) :
        inputController(),
        _targetNodes(targetNodes),
        _layer(layer),
        _camera(layer->getCamera()),
        _translationPlanes(vector<translationPlane*>()),
        _isTranslating(false),
        _planesToRemove(vector<translationPlane*>())
    {
    }

    translationInputController::~translationInputController()
    {
    }

    vec3 translationInputController::getClosestAxisNormal(vec3 direction)
    {
        vec3 axisNormals[] =
        {
            vec3(1.0f, 0.0f, 0.0f),
            vec3(0.0f, 1.0f, 0.0f),
            vec3(0.0f, 0.0f, 1.0f)
        };

        auto maxDot = -2.0f;
        auto maxNormal = vec3();
        for (uint_fast8_t i = 0; i < 3; ++i)
        {
            auto dot = glm::dot(direction, axisNormals[i]);
            if (dot > maxDot)
            {
                maxDot = dot;
                maxNormal = axisNormals[i];
            }
        }

        assert(maxNormal != vec3());

        return maxNormal;
    }

    bool translationInputController::canStartTranslation(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed)
            return false;

        if (_targetNodes->size() <= 0)
            return false;

        auto idOnMousePosition = pickingFramebuffer::pick(e->x, e->y);
        auto clickComponent = pickingId::get(idOnMousePosition);

        if (!clickComponent)
            return false;

        auto node = clickComponent->getNode();
        auto rootNode = node;

        while (rootNode->getParent()->getParent() != nullptr)
        {
            rootNode = rootNode->getParent();
            if (rootNode->isSelected())
                return true;
        }

        return false;
    }

    translationPlane* translationInputController::createTranslationPlane(vec3 origin, vec3 normal)
    {
        auto planeGridNode = new node("plane");
        auto planeTransform = planeGridNode->getTransform();
        planeTransform->setLocalPosition(origin);
        planeTransform->setDirection(normal);

        auto animator = new phi::animator();

        auto planeGrid = new phi::planeGrid();
        planeGrid->setColor(color::blue);
        planeGrid->setLineThickness(8.5f);

        planeGridNode->addComponent(planeGrid);
        planeGridNode->addComponent(animator);

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

        auto gridPlane = plane(origin, normal);

        auto translationPlane = new phi::translationPlane(origin, normal);
        translationPlane->setGridPlane(gridPlane);
        translationPlane->setCollidee(nullptr);
        translationPlane->setCollider(nullptr);
        translationPlane->setPlaneGridNode(planeGridNode);
        translationPlane->setDraggingAnimation(draggingAnimation);
        translationPlane->setFadeInAnimation(fadeInAnimation);
        translationPlane->setFadeOutAnimation(fadeOutAnimation);
        translationPlane->setClippingPlanesFadeAnimation(clippingPlanesFadeOutAnimation);

        return translationPlane;
    }

    void translationInputController::startTranslation(ivec2 mousePosition)
    {
        auto viewDirection = -_camera->screenPointToRay(mousePosition.x, mousePosition.y).getDirection();
        auto planeNormal = getClosestAxisNormal(viewDirection);

        auto minDot = std::numeric_limits<float>().max();
        auto planeOrigin = vec3();
        for (auto& targetNode : *_targetNodes)
        {
            auto collider = targetNode->getComponent<boxCollider>();
            assert(collider != nullptr);

            auto obb = collider->getObb();
            auto obbPosition = obb.getPositionAt(-planeNormal);

            auto dot = glm::dot(obbPosition, planeNormal);
            if (dot < minDot)
            {
                minDot = dot;
                planeOrigin = obbPosition;
            }
        }

        auto translationPlane = createTranslationPlane(planeOrigin, planeNormal);
        _layer->add(translationPlane->getPlaneGridNode());
        _translationPlanes.push_back(translationPlane);
        translationPlane->showGrid();

        _isTranslating = true;
    }

    void translationInputController::endTranslation()
    {
        _isTranslating = false;

        for (auto& translationPlane : _translationPlanes)
            enqueuePlaneForRemoval(translationPlane);

        _translationPlanes.clear();
    }

    void translationInputController::cancelTranslation()
    {
    }

    void translationInputController::enqueuePlaneForRemoval(translationPlane* planeToRemove)
    {
        auto fadeOutAnimationEnded = [=]
        {
            _planesToRemove.push_back(planeToRemove);
        };

        planeToRemove->fadeGridOpacityOut(fadeOutAnimationEnded);
    }

    bool translationInputController::onMouseDown(mouseEventArgs* e)
    {
        if (!canStartTranslation(e))
            return false;

        startTranslation(ivec2(e->x, e->y));

        _requestControlEvent->raise(this);

        return false;
    }

    bool translationInputController::onMouseMove(mouseEventArgs* e)
    {
        return false;
    }

    bool translationInputController::onMouseUp(mouseEventArgs* e)
    {
        if (!e->leftButtonPressed || !_isTranslating)
            return false;

        endTranslation();
        
        _resignControlEvent->raise(this);

        return true;
    }

    bool translationInputController::onKeyUp(keyboardEventArgs* e)
    {
        /*if (e->key == PHIK_ESCAPE)
        {
            cancelTranslation();
            endTranslation();
            return true;
        }
        */
        return false;
    }

    bool translationInputController::update()
    {
        if (!_isTranslating)
            return false;

        for (auto& translationPlane : _planesToRemove)
        {
            auto planeGridNode = translationPlane->getPlaneGridNode();
            planeGridNode->getParent()->removeChild(planeGridNode);
            safeDelete(translationPlane);
        }
        _planesToRemove.clear();

        return true;

        // Remove not valid anymore translation planes:
        // TODO: create valid plane if removed all
        //vector<translationPlane*> translationPlanesToRemove;

        //for (auto& translationPlane : _translationPlanes)
        //{
        //    if (!canTranslateAt(translationPlane->getMousePlane().normal))
        //        translationPlanesToRemove.push_back(translationPlane);
        //}

        //for (auto& planeToRemove : translationPlanesToRemove)
        //{
        //    removeTranslationPlane(planeToRemove);
        //    removeClippingPlanes(planeToRemove);
        //}
    }

    void translationInputController::cancel()
    {
        cancelTranslation();
        endTranslation();
        _resignControlEvent->raise(this);
    }
}