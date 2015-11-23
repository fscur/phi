#ifndef _PHI_CAROUSEL_ITEM_EVENT_ARGS_H_
#define _PHI_CAROUSEL_ITEM_EVENT_ARGS_H_

#include "phi/core/eventSender.h"

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

#endif