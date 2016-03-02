#pragma once

#include "core.h"
#include "mouseEventHandler.h"
#include "keyboardEventHandler.h"

namespace phi
{
    class input
    {
    public:
        CORE_API static void notifyMouseMove(mouseEventArgs* e);
        CORE_API static void notifyMouseDown(mouseEventArgs* e);
        CORE_API static void notifyMouseUp(mouseEventArgs* e);
        CORE_API static void notifyMouseWheel(mouseEventArgs* e);
        CORE_API static void notifyKeyDown(keyboardEventArgs e);
        CORE_API static void notifyKeyUp(keyboardEventArgs e);

        CORE_API static mouseEventHandler* mouseMove;
        CORE_API static mouseEventHandler* mouseDown;
        CORE_API static mouseEventHandler* mouseUp;
        CORE_API static mouseEventHandler* mouseWheel;
        CORE_API static keyboardEventHandler* keyDown;
        CORE_API static keyboardEventHandler* keyUp;
    };

}