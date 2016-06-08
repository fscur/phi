#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\keyboardEventArgs.h>
#include <core\mouseEventArgs.h>

#include "layer.h"

namespace phi
{
    class context
    {
    private:
        phi::vector<layer*> _layers;

    private:
        void setLayersInputs();

    public:
        CONTEXT_API context(phi::vector<layer*>& layers);
        CONTEXT_API context(phi::vector<layer*>&& layers);
        CONTEXT_API ~context();

        CONTEXT_API void update();
        CONTEXT_API void render();

        CONTEXT_API void onMouseDown(phi::mouseEventArgs* e);
        CONTEXT_API void onMouseMove(phi::mouseEventArgs* e);
        CONTEXT_API void onMouseUp(phi::mouseEventArgs* e);
        CONTEXT_API void onMouseWheel(phi::mouseEventArgs* e);
        CONTEXT_API void onKeyDown(phi::keyboardEventArgs* e);
        CONTEXT_API void onKeyUp(phi::keyboardEventArgs* e);
    };
}