#pragma once
#include <phi.h>

#include <core\mouseEventArgs.h>
#include <core\keyboardEventArgs.h>

namespace phi
{
    class iMouseController
    {
    public:
        virtual void onMouseDown(mouseEventArgs* e) = 0;
        virtual void onMouseMove(mouseEventArgs* e) = 0;
        virtual void onMouseUp(mouseEventArgs* e) = 0;
        virtual void onMouseWheel(mouseEventArgs* e) = 0;
        virtual void onKeyDown(keyboardEventArgs* e) = 0;
        virtual void onKeyUp(keyboardEventArgs* e) = 0;
        virtual void update() = 0;
        virtual ~iMouseController() {}
    };
}