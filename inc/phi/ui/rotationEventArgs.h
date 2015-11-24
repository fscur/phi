#ifndef _PHI_ROTATION_EVENT_ARGS_H_
#define _PHI_ROTATION_EVENT_ARGS_H_

#include <phi/core/object3DEventArgs.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace phi
{
    class rotationEventArgs :
        public object3DEventArgs
    {
    public:
        glm::quat startOrientation;
        glm::quat endOrientation;
        bool cancel;

    public:
        rotationEventArgs(object3D* sender, glm::quat start, glm::quat end)
            : object3DEventArgs(sender), startOrientation(start), endOrientation(end), cancel(false)
        {
        }
    };
}

#endif