#pragma once
#include <phi.h>
#include "core.h"
#include "mouseEventHandler.h"
#include "keyboardEventHandler.h"

namespace phi
{
    class input
    {
    private:
        static void raiseMouseMoveEvent(mouseEventArgs* e);
        static void raiseMouseDownEvent(mouseEventArgs* e);
        static void raiseMouseUpEvent(mouseEventArgs* e);
        static void raiseMouseWheelEvent(mouseEventArgs* e);
        static void raiseKeyDownEvent(keyboardEventArgs* e);
        static void raiseKeyUpEvent(keyboardEventArgs* e);

    public:
        CORE_API static void notifyLeftMouseDown(int x, int y);
        CORE_API static void notifyRightMouseDown(int x, int y);
        CORE_API static void notifyMiddleMouseDown(int x, int y);

        CORE_API static void notifyMouseMove(int x, int y);

        CORE_API static void notifyLeftMouseUp(int x, int y);
        CORE_API static void notifyRightMouseUp(int x, int y);
        CORE_API static void notifyMiddleMouseUp(int x, int y);

        CORE_API static void notifyMouseWheel(int delta);

        CORE_API static void notifyKeyDown(int key);
        CORE_API static void notifyKeyUp(int key);

        CORE_API static mouseEventHandler* mouseMove;
        CORE_API static mouseEventHandler* mouseDown;
        CORE_API static mouseEventHandler* mouseUp;
        CORE_API static mouseEventHandler* mouseWheel;
        CORE_API static keyboardEventHandler* keyDown;
        CORE_API static keyboardEventHandler* keyUp;
    };

}