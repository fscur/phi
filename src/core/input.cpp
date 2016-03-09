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
        if (mouseMove->isBound())
            mouseMove->invoke(e);
    }

    void input::raiseMouseDownEvent(mouseEventArgs* e)
    {
        if (mouseDown->isBound())
            mouseDown->invoke(e);
    }

    void input::raiseMouseUpEvent(mouseEventArgs* e)
    {
        if (mouseUp->isBound())
            mouseUp->invoke(e);
    }

    void input::raiseMouseWheelEvent(mouseEventArgs* e)
    {
        if (mouseWheel->isBound())
            mouseWheel->invoke(e);
    }

    void input::raiseKeyDownEvent(keyboardEventArgs e)
    {
        if (keyDown->isBound())
            keyDown->invoke(e);
    }

    void input::raiseKeyUpEvent(keyboardEventArgs e)
    {
        if (keyUp->isBound())
            keyUp->invoke(e);
    }

    void input::notifyLeftMouseDown(int x, int y)
    {
        phi::debug("left mouse down notified: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    
    void input::notifyRightMouseDown(int x, int y)
    {
        phi::debug("right mouse down notified: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }

    void input::notifyMiddleMouseDown(int x, int y)
    {
        phi::debug("middle mouse down notified: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }

    void input::notifyMouseMove(int x, int y)
    {
        phi::debug("mouse move notified: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    
    void input::notifyLeftMouseUp(int x, int y)
    {
        phi::debug("left mouse up notified: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }

    void input::notifyRightMouseUp(int x, int y)
    {
        phi::debug("right mouse up notified: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }

    void input::notifyMiddleMouseUp(int x, int y)
    {
        phi::debug("middle mouse up notified: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }

    void input::notifyMouseWheel(int delta)
    {
        phi::debug("mouse wheel notified: (" + std::to_string(delta) + ")");
    }

    void input::notifyKeyDown(int key)
    {
        phi::debug("key down notified: (" + std::to_string(key) + ")");
    }

    void input::notifyKeyUp(int key)
    {
        phi::debug("key up notified: (" + std::to_string(key) + ")");
    }
}