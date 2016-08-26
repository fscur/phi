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

        keyboardEventArgs() :
            sender(nullptr),
            key(PHIK_UNKNOWN)
        {
        }
    };
}