#include <precompiled.h>
#include "uiMouseController.h"

#include <ui\control.h>
#include <core\clickComponent.h>

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

    bool uiMouseController::onMouseDown(mouseEventArgs * e)
    {
        _clickedControl = nullptr;
        auto camera = _layer->getCamera();
        auto ray = camera->screenPointToRay(e->x, e->y);

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

                bool hasClicked = ray.intersects(transformedAabb);

                if (hasClicked)
                {
                    _clickedControl = control;
                    _clickedControlColor = control->getColor();

                    control->setColor(color::yellow);
                    return true;
                }
            }
        }

        return false;
    }

    bool uiMouseController::onMouseUp(mouseEventArgs * e)
    {
        if (_clickedControl)
        {
            _clickedControl->setColor(_clickedControlColor);

            auto click = _clickedControl->getNode()->getComponent<clickComponent>();
            if(click)
                click->onClick();

            return true;
        }

        return false;
    }
}