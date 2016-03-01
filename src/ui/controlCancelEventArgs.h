#ifndef _PHI_CONTROL_CANCEL_EVENT_ARGS_H_
#define _PHI_CONTROL_CANCEL_EVENT_ARGS_H_

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

#endif