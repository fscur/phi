#ifndef _PHI_DRAG_DROP_EVENT_ARGS_H_
#define _PHI_DRAG_DROP_EVENT_ARGS_H_

#include <core\eventArgs.h>

#include <string>

namespace phi
{
    class dragDropEventArgs : public eventArgs
    {
    public:
        bool handled;
        std::string data;
        int x;
        int y;

    public:
        dragDropEventArgs(std::string data, int x, int y)
            : handled(false), data(data), x(x), y(y), eventArgs()
        {
        }
    };
}

#endif