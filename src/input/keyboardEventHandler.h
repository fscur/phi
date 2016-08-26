#pragma once
#include <phi.h>
#include <core\eventHandler.h>
#include "keyboardEventArgs.h"

namespace phi
{
    class keyboardEventHandler :
        public eventHandler<keyboardEventArgs*>
    {
    };
}