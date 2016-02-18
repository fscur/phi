#pragma once

#include "eventHandler.h"
#include "keyboardEventArgs.h"

namespace phi
{
    class keyboardEventHandler :
        public eventHandler<keyboardEventArgs>
    {
    };
}