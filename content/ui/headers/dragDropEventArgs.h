#ifndef _PHI_DRAG_DROP_EVENT_ARGS_H_
#define _PHI_DRAG_DROP_EVENT_ARGS_H_

#include <string>
#include "eventArgs.h"

namespace phi
{
    class dragDropEventArgs : public eventArgs
    {
    public:
        bool handled;
        std::string data;
        float x;
        float y;

    public:
        dragDropEventArgs(std::string data, float x, float y)
            : handled(false), data(data), x(x), y(y), eventArgs()
        {
        }
    };
}

#endif