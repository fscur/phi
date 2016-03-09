#pragma once
#include <phi.h>
#include "keycode.h"

namespace phi
{
    struct keyboardEventArgs
    {
    public:
        void* sender;
        keycode key;
        bool isCtrlPressed;
        bool isShiftPressed;
        bool isAltPressed;

        keyboardEventArgs() :
            sender(nullptr),
            key(PHIK_UNKNOWN),
            isCtrlPressed(false),
            isShiftPressed(false),
            isAltPressed(false)
        {}
    };
}