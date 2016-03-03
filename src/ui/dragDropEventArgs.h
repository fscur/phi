#pragma once
#include <phi.h>

#include <core\eventArgs.h>

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