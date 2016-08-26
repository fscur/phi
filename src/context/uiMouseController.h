#pragma once
#include <phi.h>

#include <core\color.h>
#include <ui\control.h>

#include "layer.h"
#include "inputController.h"

namespace phi
{
    class uiMouseController :
        public inputController
    {
    private:
        layer* _layer;

        color _clickedControlColor;
        control* _clickedControl;

    public:
        uiMouseController(layer* layer);
        ~uiMouseController();

        virtual bool onMouseDown(mouseEventArgs* e) override;
        virtual bool onMouseUp(mouseEventArgs* e) override;
    };
}