#ifndef _PHI_CONTROL_EVENT_ARGS_H_
#define _PHI_CONTROL_EVENT_ARGS_H_

#include "phi/core/eventSender.h"

namespace phi
{
    class controlEventArgs
    {
    public:
        eventSender* sender;

    public:
        controlEventArgs(eventSender* sender)
        {
            this->sender = sender;
        }
    };
}

#endif