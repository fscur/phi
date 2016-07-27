#pragma once
#include <phi.h>
#include "coreApi.h"
#include "mouseEventHandler.h"
#include "keyboardEventHandler.h"

namespace phi
{
    class CORE_API input
    {
    private:
        static void raiseMouseMoveEvent(mouseEventArgs* e);
        static void raiseMouseDownEvent(mouseEventArgs* e);
        static void raiseMouseUpEvent(mouseEventArgs* e);
        static void raiseBeginMouseWheelEvent(mouseEventArgs* e);
        static void raiseMouseWheelEvent(mouseEventArgs* e);
        static void raiseEndMouseWheelEvent(mouseEventArgs* e);
        static void raiseKeyDownEvent(keyboardEventArgs* e);
        static void raiseKeyUpEvent(keyboardEventArgs* e);

    public:
        static void notifyLeftMouseDown(int x, int y);
        static void notifyRightMouseDown(int x, int y);
        static void notifyMiddleMouseDown(int x, int y);

        static void notifyLeftMouseUp(int x, int y);
        static void notifyRightMouseUp(int x, int y);
        static void notifyMiddleMouseUp(int x, int y);

        static void notifyMouseMove(int x, int y);

        static void notifyBeginMouseWheel(int delta, int x, int y);
        static void notifyMouseWheel(int delta, int x, int y);
        static void notifyEndMouseWheel(int delta, double elapsed);

        static void notifyKeyDown(int key);
        static void notifyKeyUp(int key);

        static void release();

        static mouseEventHandler* mouseMove;
        static mouseEventHandler* mouseDown;
        static mouseEventHandler* mouseUp;
        static mouseEventHandler* beginMouseWheel;
        static mouseEventHandler* mouseWheel;
        static mouseEventHandler* endMouseWheel;
        static keyboardEventHandler* keyDown;
        static keyboardEventHandler* keyUp;
    };

}