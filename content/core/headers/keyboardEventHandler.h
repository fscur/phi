#ifndef _PHI_KEYBOARD_EVENT_HANDLER_H_
#define _PHI_KEYBOARD_EVENT_HANDLER_H_

#include "eventHandler.h"
#include "keyboardEventArgs.h"

namespace phi
{
    class keyboardEventHandler :
        public eventHandler<keyboardEventArgs>
    {
    };
}

#endif