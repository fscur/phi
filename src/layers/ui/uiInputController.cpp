#include <precompiled.h>
#include <common/mouseInteractionComponent.h>
#include <rendering/pickingFramebuffer.h>
#include <context/pickingId.h>
#include <ui/control.h>
#include "uiInputController.h"

namespace phi
{
    uiInputController::uiInputController(layer* layer) :
        _layer(layer),
        _focusedControl(nullptr),
        _lastMouseOverControl(nullptr)
    {
    }

    uiInputController::~uiInputController()
    {
    }

    control* uiInputController::getControlUnderMouse(int x, int y)
    {
        auto idOnMousePosition = pickingFramebuffer::pick(x, y);
        auto clickComponent = pickingId::get(idOnMousePosition);

        if (clickComponent)
            return clickComponent->getNode()->getComponent<control>();

        return nullptr;
    }

    bool uiInputController::onMouseDown(mouseEventArgs * e)
    {
        _focusedControl = getControlUnderMouse(e->x, e->y);

        if (_focusedControl)
        {
            auto mouseInteraction = _focusedControl->getNode()->getComponent<mouseInteractionComponent>();
            if (mouseInteraction)
                mouseInteraction->onMouseDown();
        }

        return false;
    }

    bool uiInputController::onMouseUp(mouseEventArgs * e)
    {
        if (_focusedControl)
        {
            auto mouseInteraction = _focusedControl->getNode()->getComponent<mouseInteractionComponent>();
            if (mouseInteraction)
                mouseInteraction->onMouseUp();

            return true;
        }

        return false;
    }

    bool uiInputController::onMouseMove(mouseEventArgs* e)
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

        if (_focusedControl)
            _focusedControl->getNode()->getComponent<mouseInteractionComponent>()->onMouseMove(e);

        return false;
    }
}