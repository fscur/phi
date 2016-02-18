#pragma once

#include "eventHandler.h"
#include "mouseEventArgs.h"

namespace phi
{
    class mouseEventHandler :
        public eventHandler<mouseEventArgs*>
    {
    };
}