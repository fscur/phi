#include <precompiled.h>
#include "uiMouseController.h"

#include <ui/control.h>
#include <common/mouseInteractionComponent.h>

namespace phi
{
    uiMouseController::uiMouseController(layer* layer) :
        _layer(layer),
        _clickedControl(nullptr)
    {
    }

    uiMouseController::~uiMouseController()
    {
    }

    control* uiMouseController::getControlUnderMouse(int x, int y)
    {
        auto camera = _layer->getCamera();
        auto ray = camera->screenPointToRay(x, y);

        auto rootChildren = _layer->getRoot()->getChildren();
        for (auto& child : *rootChildren)
        {
            auto control = child->getComponent<phi::control>();
            if (control)
            {
                auto model = child->getTransform()->getModelMatrix();
                auto aabb = phi::aabb(glm::vec3(0.0f), glm::vec3(1.0f));

                auto transformedMin = mathUtils::multiply(model, aabb.min);
                auto transformedMax = mathUtils::multiply(model, aabb.max);

                auto transformedAabb = phi::aabb(transformedMin, transformedMax);

                if (ray.intersects(transformedAabb))
                    return control;
            }
        }

        return nullptr;
    }

    bool uiMouseController::onMouseDown(mouseEventArgs * e)
    {
        _clickedControl = getControlUnderMouse(e->x, e->y);

        if (_clickedControl)
        {
            auto mouseInteraction = _clickedControl->getNode()->getComponent<mouseInteractionComponent>();
            if (mouseInteraction)
                mouseInteraction->onMouseDown();
        }

        return false;
    }

    bool uiMouseController::onMouseUp(mouseEventArgs * e)
    {
        if (_clickedControl)
        {
            auto mouseInteraction = _clickedControl->getNode()->getComponent<mouseInteractionComponent>();
            if (mouseInteraction)
                mouseInteraction->onMouseUp();
        }

        return false;
    }

    bool uiMouseController::onMouseMove(mouseEventArgs * e)
    {
        auto camera = _layer->getCamera();
        auto ray = camera->screenPointToRay(e->x, e->y);
        auto aabb = phi::aabb(glm::vec3(0.0f), glm::vec3(1.0f));

        auto rootChildren = _layer->getRoot()->getChildren();
        for (auto& child : *rootChildren)
        {
            auto control = child->getComponent<phi::control>();
            auto mouseInteraction = child->getComponent<mouseInteractionComponent>();

            if (control && mouseInteraction)
            {
                auto model = child->getTransform()->getModelMatrix();

                auto transformedMin = mathUtils::multiply(model, aabb.min);
                auto transformedMax = mathUtils::multiply(model, aabb.max);
                auto transformedAabb = phi::aabb(transformedMin, transformedMax);

                bool isMouseOver = ray.intersects(transformedAabb);
                bool wasMouseOver = mouseInteraction->isMouseOver();

                if (isMouseOver && !wasMouseOver)
                {
                    mouseInteraction->onMouseEnter();
                    return true;
                }
                else if (!isMouseOver && wasMouseOver)
                {
                    mouseInteraction->onMouseLeave();
                    return true;
                }
            }
        }

        return false;
    }
}