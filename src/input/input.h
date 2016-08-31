#pragma once
#include <phi.h>
#include "inputApi.h"
#include "mouseEventHandler.h"
#include "keyboardEventHandler.h"

namespace phi
{
    class INPUT_API input
    {
    private:
        
        static void raiseMouseDownEvent(mouseEventArgs* e);
        static void raiseMouseUpEvent(mouseEventArgs* e);
        static void raiseMouseClickEvent(mouseEventArgs* e);
        static void raiseMouseDoubleClickEvent(mouseEventArgs* e);

        static void raiseMouseMoveEvent(mouseEventArgs* e);

        static void raiseBeginMouseWheelEvent(mouseEventArgs* e);
        static void raiseMouseWheelEvent(mouseEventArgs* e);
        static void raiseEndMouseWheelEvent(mouseEventArgs* e);
        static void raiseKeyDownEvent(keyboardEventArgs* e);
        static void raiseKeyUpEvent(keyboardEventArgs* e);

    public:
        static void notifyLeftMouseDown(int x, int y);
        static void notifyLeftMouseUp(int x, int y);
        static void notifyLeftMouseClick(int x, int y);
        static void notifyLeftMouseDoubleClick(int x, int y);

        static void notifyRightMouseDown(int x, int y);
        static void notifyRightMouseUp(int x, int y);
        static void notifyRightMouseClick(int x, int y);
        static void notifyRightMouseDoubleClick(int x, int y);

        static void notifyMiddleMouseDown(int x, int y);
        static void notifyMiddleMouseUp(int x, int y);
        static void notifyMiddleMouseClick(int x, int y);
        static void notifyMiddleMouseDoubleClick(int x, int y);

        static void notifyMouseMove(int x, int y);

        static void notifyBeginMouseWheel(int delta, int x, int y);
        static void notifyMouseWheel(int delta, int x, int y);
        static void notifyEndMouseWheel(int delta, double elapsed);

        static void notifyKeyDown(int key);
        static void notifyKeyUp(int key);

        static void release();

        static mouseEventHandler* mouseDown;
        static mouseEventHandler* mouseUp;
        static mouseEventHandler* mouseClick;
        static mouseEventHandler* mouseDoubleClick;
        static mouseEventHandler* mouseMove;
        static mouseEventHandler* beginMouseWheel;
        static mouseEventHandler* mouseWheel;
        static mouseEventHandler* endMouseWheel;
        static keyboardEventHandler* keyDown;
        static keyboardEventHandler* keyUp;
    };

}