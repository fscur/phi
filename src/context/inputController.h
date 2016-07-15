#pragma once
#include <phi.h>

#include <core\mouseEventArgs.h>
#include <core\keyboardEventArgs.h>

namespace phi
{
    class inputController
    {
    protected:
        inputController() {}

    public:
        virtual void onMouseDown(mouseEventArgs* e) {}
        virtual void onMouseMove(mouseEventArgs* e) {}
        virtual void onMouseUp(mouseEventArgs* e) {}
        virtual void onMouseWheel(mouseEventArgs* e) {}
        virtual void onKeyDown(keyboardEventArgs* e) {}
        virtual void onKeyUp(keyboardEventArgs* e) {}
        virtual void update() {}
        virtual ~inputController() {}
    };
}