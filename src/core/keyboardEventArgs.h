#pragma once

#include "eventHandler.h"
#include "keycode.h"

namespace phi
{
    struct keyboardEventArgs
    {
    public:
        keycode key;
        bool isCtrlPressed;
        bool isShiftPressed;
        bool isAltPressed;
    };
}