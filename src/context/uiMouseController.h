#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\color.h>
#include <ui\control.h>

#include <input\inputController.h>

#include "layer.h"

namespace phi
{
    class uiMouseController :
        public inputController
    {
    private:
        layer* _layer;
        control* _clickedControl;

    public:
        CONTEXT_API uiMouseController(layer* layer);
        CONTEXT_API ~uiMouseController();

        virtual bool onMouseDown(mouseEventArgs* e) override;
        virtual bool onMouseUp(mouseEventArgs* e) override;
        virtual bool onMouseMove(mouseEventArgs* e) override;
    };
}