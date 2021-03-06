#pragma once
#include <phi.h>

#include <core/color.h>
#include <ui/control.h>

#include <input/inputController.h>

#include "../layer.h"

namespace phi
{
    class uiInputController :
        public inputController
    {
    public:
        uiInputController(layer* layer);
        ~uiInputController();

        virtual bool onMouseDown(mouseEventArgs* e) override;
        virtual bool onMouseUp(mouseEventArgs* e) override;
        virtual bool onMouseMove(mouseEventArgs* e) override;

    private:
        control * getControlUnderMouse(int x, int y);

    private:
        layer* _layer;
        control* _focusedControl;
        control* _lastMouseOverControl;
    };
}