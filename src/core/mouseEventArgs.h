#pragma once
#include <precompiled.h>
#include "eventHandler.h"

namespace phi
{
    struct mouseEventArgs
    {
    public:
        void* sender;
        int x;
        int y;
        unsigned int clicks;
        bool leftButtonPressed;
        bool rightButtonPressed;
        bool middleButtonPressed;
        float wheelDelta;
        bool handled;
        bool isCtrlPressed;
        bool isShiftPressed;
        bool isAltPressed;

        mouseEventArgs() : handled(false) {}
    };
}