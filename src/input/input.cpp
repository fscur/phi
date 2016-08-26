#include <precompiled.h>
#include "input.h"

namespace phi
{
    mouseEventHandler* input::mouseDown = new mouseEventHandler();
    mouseEventHandler* input::mouseUp = new mouseEventHandler();
    mouseEventHandler* input::mouseClick = new mouseEventHandler();
    mouseEventHandler* input::mouseDoubleClick = new mouseEventHandler();
    mouseEventHandler* input::mouseMove = new mouseEventHandler();
    mouseEventHandler* input::beginMouseWheel = new mouseEventHandler();
    mouseEventHandler* input::mouseWheel = new mouseEventHandler();
    mouseEventHandler* input::endMouseWheel = new mouseEventHandler();
    keyboardEventHandler* input::keyDown = new keyboardEventHandler();
    keyboardEventHandler* input::keyUp = new keyboardEventHandler();

    void input::raiseMouseDownEvent(mouseEventArgs* e)
    {
        mouseDown->raise(e);
    }

    void input::raiseMouseUpEvent(mouseEventArgs* e)
    {
        mouseUp->raise(e);
    }

    void input::raiseMouseDoubleClickEvent(mouseEventArgs* e)
    {
        mouseDoubleClick->raise(e);
    }

    void input::raiseMouseClickEvent(mouseEventArgs * e)
    {
        mouseClick->raise(e);
    }

    void input::raiseMouseMoveEvent(mouseEventArgs* e)
    {
        mouseMove->raise(e);
    }

    void input::raiseBeginMouseWheelEvent(mouseEventArgs* e)
    {
        beginMouseWheel->raise(e);
    }
    
    void input::raiseMouseWheelEvent(mouseEventArgs* e)
    {
        mouseWheel->raise(e);
    }
    
    void input::raiseEndMouseWheelEvent(mouseEventArgs* e)
    {
        endMouseWheel->raise(e);
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
    
    void input::notifyLeftMouseUp(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.leftButtonPressed = true;
        raiseMouseUpEvent(&mouseArgs);
    }

    void input::notifyLeftMouseClick(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.leftButtonPressed = true;
        raiseMouseClickEvent(&mouseArgs);
    }
    
    void input::notifyLeftMouseDoubleClick(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.leftButtonPressed = true;
        raiseMouseDoubleClickEvent(&mouseArgs);
    }
    
    void input::notifyRightMouseDown(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.rightButtonPressed = true;
        raiseMouseDownEvent(&mouseArgs);
    }

    void input::notifyRightMouseUp(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.rightButtonPressed = true;
        raiseMouseUpEvent(&mouseArgs);
    }
    
    void input::notifyRightMouseClick(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.rightButtonPressed = true;
        raiseMouseClickEvent(&mouseArgs);
    }

    void input::notifyRightMouseDoubleClick(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.rightButtonPressed = true;
        raiseMouseDoubleClickEvent(&mouseArgs);
    }

    void input::notifyMiddleMouseDown(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.middleButtonPressed = true;
        raiseMouseDownEvent(&mouseArgs);
    }

    void input::notifyMiddleMouseUp(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.middleButtonPressed = true;
        raiseMouseUpEvent(&mouseArgs);
    }
    
    void input::notifyMiddleMouseClick(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.middleButtonPressed = true;
        raiseMouseClickEvent(&mouseArgs);
    }

    void input::notifyMiddleMouseDoubleClick(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        mouseArgs.middleButtonPressed = true;
        raiseMouseDoubleClickEvent(&mouseArgs);
    }

    void input::notifyMouseMove(int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.x = x;
        mouseArgs.y = y;
        raiseMouseMoveEvent(&mouseArgs);
    }

    void input::notifyBeginMouseWheel(int delta, int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.wheelDelta = static_cast<float>(delta);
        mouseArgs.x = x;
        mouseArgs.y = y;
        raiseBeginMouseWheelEvent(&mouseArgs);
    }

    void input::notifyMouseWheel(int delta, int x, int y)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.wheelDelta = static_cast<float>(delta);
        mouseArgs.x = x;
        mouseArgs.y = y;
        raiseMouseWheelEvent(&mouseArgs);
    }

    void input::notifyEndMouseWheel(int delta, double elapsed)
    {
        auto mouseArgs = mouseEventArgs();
        mouseArgs.wheelDelta = static_cast<float>(delta);
        mouseArgs.elapsed = elapsed;
        raiseEndMouseWheelEvent(&mouseArgs);
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
        safeDelete(mouseDown);
        safeDelete(mouseUp);
        safeDelete(mouseClick);
        safeDelete(mouseDoubleClick);
        safeDelete(mouseMove);
        safeDelete(beginMouseWheel);
        safeDelete(mouseWheel);
        safeDelete(endMouseWheel);
        safeDelete(keyDown);
        safeDelete(keyUp);
    }
}