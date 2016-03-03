#pragma once
#include <phi.h>

#include <core\eventSender.h>

namespace phi
{
    class carouselItem;

    class carouselItemEventArgs
    {
    public:
        carouselItem* sender;

    public:
        carouselItemEventArgs(carouselItem* sender)
        {
            this->sender = sender;
        }
    };
}