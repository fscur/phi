#pragma once
#include <precompiled.h>
#include "eventHandler.h"
#include "keyboardEventArgs.h"

namespace phi
{
    class keyboardEventHandler :
        public eventHandler<keyboardEventArgs>
    {
    };
}