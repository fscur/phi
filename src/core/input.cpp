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
        if (mouseMove->isBound())
            mouseMove->invoke(e);
    }

    void input::notifyMouseDown(mouseEventArgs* e)
    {
        if (mouseDown->isBound())
            mouseDown->invoke(e);
    }

    void input::notifyMouseUp(mouseEventArgs* e)
    {
        if (mouseUp->isBound())
            mouseUp->invoke(e);
    }

    void input::notifyMouseWheel(mouseEventArgs* e)
    {
        if (mouseWheel->isBound())
            mouseWheel->invoke(e);
    }

    void input::notifyKeyDown(keyboardEventArgs e)
    {
        if (keyDown->isBound())
            keyDown->invoke(e);
    }

    void input::notifyKeyUp(keyboardEventArgs e)
    {
        if (keyUp->isBound())
            keyUp->invoke(e);
    }
}