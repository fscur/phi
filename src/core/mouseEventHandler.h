#pragma once
#include <precompiled.h>
#include "eventHandler.h"
#include "mouseEventArgs.h"

namespace phi
{
    class mouseEventHandler :
        public eventHandler<mouseEventArgs*>
    {
    };
}