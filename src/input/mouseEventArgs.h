#pragma once
#include <phi.h>

namespace phi
{
    struct mouseEventArgs
    {
    public:
        void* sender;
        int x;
        int y;
        uint clicks;
        bool leftButtonPressed;
        bool rightButtonPressed;
        bool middleButtonPressed;
        float wheelDelta;
        double elapsed;
        bool handled;

        mouseEventArgs() :
            sender(nullptr),
            x(0),
            y(0),
            clicks(0),
            leftButtonPressed(false),
            rightButtonPressed(false),
            middleButtonPressed(false),
            wheelDelta(0.0f),
            elapsed(0.0),
            handled(false)
        {
        }
    };
}