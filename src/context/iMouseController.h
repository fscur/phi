#pragma once
#include <phi.h>

#include <core\mouseEventArgs.h>
#include <core\keyboardEventArgs.h>

namespace phi
{
    class iMouseController
    {
    public:
        virtual void onMouseDown(phi::mouseEventArgs* e) = 0;
        virtual void onMouseMove(phi::mouseEventArgs* e) = 0;
        virtual void onMouseUp(phi::mouseEventArgs* e) = 0;
        virtual void onMouseWheel(phi::mouseEventArgs* e) = 0;
        virtual void onKeyDown(phi::keyboardEventArgs* e) = 0;
        virtual void onKeyUp(phi::keyboardEventArgs* e) = 0;
        virtual void update() = 0;
    };
}