#ifndef _PHI_KEYBOARD_EVENT_ARGS_H_
#define _PHI_KEYBOARD_EVENT_ARGS_H_

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

#endif