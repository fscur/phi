#pragma once
#include <precompiled.h>

//#include <core\object3DEventArgs.h>

namespace phi
{
    class rotationEventArgs /*:
        public object3DEventArgs*/
    {
    public:
        quat startOrientation;
        quat endOrientation;
        bool cancel;

    public:
        rotationEventArgs(transform* sender, quat start, quat end)
            /*: object3DEventArgs(sender), startOrientation(start), endOrientation(end), cancel(false)*/
        {
        }
    };
}