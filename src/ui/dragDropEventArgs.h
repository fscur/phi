#pragma once
#include <phi.h>

#include <core\eventArgs.h>

namespace phi
{
    class dragDropEventArgs : 
        public eventArgs
    {
    public:
        bool handled;
        string data;
        int x;
        int y;

    public:
        dragDropEventArgs(string data, int x, int y) : 
            eventArgs(),
            handled(false), 
            data(data), 
            x(x), 
            y(y)
        {
        }
    };
}