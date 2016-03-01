#ifndef _PHI_ROTATION_EVENT_ARGS_H_
#define _PHI_ROTATION_EVENT_ARGS_H_

#include <core\object3DEventArgs.h>

namespace phi
{
    class rotationEventArgs :
        public object3DEventArgs
    {
    public:
        quat startOrientation;
        quat endOrientation;
        bool cancel;

    public:
        rotationEventArgs(object3D* sender, quat start, quat end)
            : object3DEventArgs(sender), startOrientation(start), endOrientation(end), cancel(false)
        {
        }
    };
}

#endif