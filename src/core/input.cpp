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

    void input::raiseKeyDownEvent(keyboardEventArgs e)
    {
        keyDown->raise(e);
    }

    void input::raiseKeyUpEvent(keyboardEventArgs e)
    {
        keyUp->raise(e);
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