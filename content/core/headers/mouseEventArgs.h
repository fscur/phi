#ifndef _PHI_MOUSE_EVENT_ARGS_H_
#define _PHI_MOUSE_EVENT_ARGS_H_

#include "eventHandler.h"

namespace phi
{
    struct mouseEventArgs
    {
    public:
        int x;
        int y;
        unsigned int clicks;
        bool leftButtonPressed;
        bool rightButtonPressed;
        bool middleButtonPressed;
        float wheelDelta;
        bool handled;

        mouseEventArgs() : handled(false) {}
    };
}

#endif