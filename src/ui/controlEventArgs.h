#pragma once
#include <precompiled.h>

#include <core\eventSender.h>

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