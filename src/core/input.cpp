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

    void input::notifyMouseMove(mouseEventArgs* e)
    {
        mouseMove->raise(e);
    }

    void input::notifyMouseDown(mouseEventArgs* e)
    {
        mouseDown->raise(e);
    }

    void input::notifyMouseUp(mouseEventArgs* e)
    {
        mouseUp->raise(e);
    }

    void input::notifyMouseWheel(mouseEventArgs* e)
    {
        mouseWheel->raise(e);
    }

    void input::notifyKeyDown(keyboardEventArgs e)
    {
        keyDown->raise(e);
    }

    void input::notifyKeyUp(keyboardEventArgs e)
    {
        keyUp->raise(e);
    }
}