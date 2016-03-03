#pragma once
#include <phi.h>

#include "controlEventArgs.h"

namespace phi
{
    class controlCancelEventArgs : public controlEventArgs
    {
    public:
        bool cancel;

    public:
        controlCancelEventArgs(eventSender* sender) : controlEventArgs(sender)
        {
            cancel = false;
        }
    };
}