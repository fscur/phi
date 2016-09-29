#include <precompiled.h>
#include "uiMouseController.h"

#include <common/mouseInteractionComponent.h>
#include <rendering/pickingFramebuffer.h>
#include <context/pickingId.h>
#include <ui/control.h>

namespace phi
{
    uiMouseController::uiMouseController(layer* layer) :
        _layer(layer),
        _clickedControl(nullptr),
        _lastMouseOverControl(nullptr)
    {
    }

    uiMouseController::~uiMouseController()
    {
    }

    control* uiMouseController::getControlUnderMouse(int x, int y)
    {
        auto idOnMousePosition = pickingFramebuffer::pick(x, y);
        auto clickComponent = pickingId::get(idOnMousePosition);

        if (clickComponent)
            return clickComponent->getNode()->getComponent<control>();

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
        auto mouseOverControl = getControlUnderMouse(e->x, e->y);

        if (_lastMouseOverControl != mouseOverControl)
        {
            if (_lastMouseOverControl)
                _lastMouseOverControl->getNode()->getComponent<mouseInteractionComponent>()->onMouseLeave();

            if (mouseOverControl)
                mouseOverControl->getNode()->getComponent<mouseInteractionComponent>()->onMouseEnter();
        }

        _lastMouseOverControl = mouseOverControl;

        return false;
    }
}