#pragma once
#include <phi.h>

#include <core/color.h>
#include <ui/control.h>

#include <input/inputController.h>

#include <context/layer.h>

namespace phi
{
    class uiMouseController :
        public inputController
    {
    private:
        layer* _layer;
        control* _clickedControl;

    public:
        uiMouseController(layer* layer);
        ~uiMouseController();

        virtual bool onMouseDown(mouseEventArgs* e) override;
        virtual bool onMouseUp(mouseEventArgs* e) override;
        virtual bool onMouseMove(mouseEventArgs* e) override;

    private:
        control * getControlUnderMouse(int x, int y);
    };
}