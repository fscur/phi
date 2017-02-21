#pragma once
#include <phi.h>
#include <core\eventHandler.h>

#include "mouseEventArgs.h"

namespace phi
{
    class mouseEventHandler :
        public eventHandler<mouseEventArgs*>
    {
    };
}