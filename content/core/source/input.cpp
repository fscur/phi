#include "input.h"

namespace phi
{
    mouseEventHandler* input::mouseDown = new mouseEventHandler();
    mouseEventHandler* input::mouseUp = new mouseEventHandler();
    mouseEventHandler* input::mouseMove = new mouseEventHandler();
    mouseEventHandler* input::mouseWheel = new mouseEventHandler();

    void input::notifyMouseMove(mouseEventArgs e)
    {
        if (mouseMove->isBound())
            mouseMove->invoke(e);
    }

    void input::notifyMouseDown(mouseEventArgs e)
    {
        if (mouseDown->isBound())
            mouseDown->invoke(e);
    }

    void input::notifyMouseUp(mouseEventArgs e)
    {
        if (mouseUp->isBound())
            mouseUp->invoke(e);
    }

    void input::notifyMouseWheel(mouseEventArgs e)
    {
        if (mouseWheel->isBound())
            mouseWheel->invoke(e);
    }
}