#ifndef _PHI_MOUSE_EVENT_HANDLER_H_
#define _PHI_MOUSE_EVENT_HANDLER_H_

#include "eventHandler.h"
#include "mouseEventArgs.h"

namespace phi
{
    class mouseEventHandler :
        public eventHandler<mouseEventArgs>
    {
    };
}

#endif