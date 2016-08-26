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
        virtual bool onMouseDown(mouseEventArgs* e) { return false; }
        virtual bool onMouseUp(mouseEventArgs* e) { return false; }
        virtual bool onMouseClick(mouseEventArgs* e) { return false; }
        virtual bool onMouseDoubleClick(mouseEventArgs* e) { return false; }
        virtual bool onMouseMove(mouseEventArgs* e) { return false; }
        virtual bool onBeginMouseWheel(mouseEventArgs* e) { return false; }
        virtual bool onMouseWheel(mouseEventArgs* e) { return false; }
        virtual bool onEndMouseWheel(mouseEventArgs* e) { return false; }
        virtual bool onKeyDown(keyboardEventArgs* e) { return false; }
        virtual bool onKeyUp(keyboardEventArgs* e) { return false; }
        virtual bool update() { return false; }
        virtual ~inputController() {}
    };
}