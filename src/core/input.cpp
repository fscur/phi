#include <precompiled.h>
#include "input.h"

namespace phi
{
    mouseEventHandler* input::mouseDown = new mouseEventHandler();
    mouseEventHandler* input::mouseUp = new mouseEventHandler();
    mouseEventHandler* input::mouseMove = new mouseEventHandler();
    mouseEventHandler* input::mouseWheel = new mouseEventHandler();
    keyboardEventHandler* input::keyDown = new keyboardEventHandler();
    keyboardEventHandler* input::keyUp = new keyboardEventHandler();

    void input::raiseMouseMoveEvent(mouseEventArgs* e)
    {
        mouseMove->raise(e);
    }

    void input::raiseMouseDownEvent(mouseEventArgs* e)
    {
        mouseDown->raise(e);
    }

    void input::raiseMouseUpEvent(mouseEventArgs* e)
    {
        mouseUp->raise(e);
    }

    void input::raiseMouseWheelEvent(mouseEventArgs* e)
    {
        mouseWheel->raise(e);
    }

    void input::raiseKeyDownEvent(keyboardEventArgs* e)
    {
        keyDown->raise(e);
    }

    void input::raiseKeyUpEvent(keyboardEventArgs* e)
    {
        keyUp->raise(e);
    }

    void input::notifyLeftMouseDown(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.leftButtonPressed = true;
        raiseMouseDownEvent(&mouseArgs);
    }

    void input::notifyRightMouseDown(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.rightButtonPressed = true;
        raiseMouseDownEvent(&mouseArgs);
    }

    void input::notifyMiddleMouseDown(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.middleButtonPressed = true;
        raiseMouseDownEvent(&mouseArgs);
    }

    void input::notifyMouseMove(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        raiseMouseMoveEvent(&mouseArgs);
    }

    void input::notifyLeftMouseUp(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.leftButtonPressed = true;
        raiseMouseUpEvent(&mouseArgs);
    }

    void input::notifyRightMouseUp(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.rightButtonPressed = true;
        raiseMouseUpEvent(&mouseArgs);
    }

    void input::notifyMiddleMouseUp(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.middleButtonPressed = true;
        raiseMouseUpEvent(&mouseArgs);
    }

    void input::notifyMouseWheel(int delta, int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.wheelDelta = static_cast<float>(delta);
        mouseArgs.x = x;
        mouseArgs.y = y;
        raiseMouseWheelEvent(&mouseArgs);
    }

    void input::notifyKeyDown(int key)
    {
        auto keyboardArgs = keyboardEventArgs();
        keyboardArgs.key = key;
        raiseKeyDownEvent(&keyboardArgs);
    }

    void input::notifyKeyUp(int key)
    {
        auto keyboardArgs = keyboardEventArgs();
        keyboardArgs.key = key;
        raiseKeyUpEvent(&keyboardArgs);
    }

    void input::release()
    {
        safeDelete(keyDown);
        safeDelete(keyUp);
        safeDelete(mouseDown);
        safeDelete(mouseUp);
        safeDelete(mouseMove);
        safeDelete(mouseWheel);
    }
}