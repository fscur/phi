#pragma once
#include <phi.h>
#include "inputApi.h"
#include "mouseEventHandler.h"
#include "keyboardEventHandler.h"

namespace phi
{
    class input
    {
    public:
        INPUT_API static ivec2 getMousePosition();

        INPUT_API static void notifyLeftMouseDown(int x, int y);
        INPUT_API static void notifyLeftMouseUp(int x, int y);
        INPUT_API static void notifyLeftMouseClick(int x, int y);
        INPUT_API static void notifyLeftMouseDoubleClick(int x, int y);

        INPUT_API static void notifyRightMouseDown(int x, int y);
        INPUT_API static void notifyRightMouseUp(int x, int y);
        INPUT_API static void notifyRightMouseClick(int x, int y);
        INPUT_API static void notifyRightMouseDoubleClick(int x, int y);

        INPUT_API static void notifyMiddleMouseDown(int x, int y);
        INPUT_API static void notifyMiddleMouseUp(int x, int y);
        INPUT_API static void notifyMiddleMouseClick(int x, int y);
        INPUT_API static void notifyMiddleMouseDoubleClick(int x, int y);

        INPUT_API static void notifyMouseMove(int x, int y);

        INPUT_API static void notifyBeginMouseWheel(int delta, int x, int y);
        INPUT_API static void notifyMouseWheel(int delta, int x, int y);
        INPUT_API static void notifyEndMouseWheel(int delta, double elapsed);

        INPUT_API static void notifyKeyDown(int key);
        INPUT_API static void notifyKeyUp(int key);

        INPUT_API static void release();

        INPUT_API static mouseEventHandler* mouseDown;
        INPUT_API static mouseEventHandler* mouseUp;
        INPUT_API static mouseEventHandler* mouseClick;
        INPUT_API static mouseEventHandler* mouseDoubleClick;
        INPUT_API static mouseEventHandler* mouseMove;
        INPUT_API static mouseEventHandler* beginMouseWheel;
        INPUT_API static mouseEventHandler* mouseWheel;
        INPUT_API static mouseEventHandler* endMouseWheel;
        INPUT_API static keyboardEventHandler* keyDown;
        INPUT_API static keyboardEventHandler* keyUp;

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

    private:
        static ivec2 _mousePosition;
    };

}