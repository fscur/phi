#ifndef _PHI_TRANSLATION_EVENT_ARGS_H_
#define _PHI_TRANSLATION_EVENT_ARGS_H_

#include "object3DEventArgs.h"
#include <glm\glm.hpp>

namespace phi
{
    class translationEventArgs :
        public object3DEventArgs
    {
    public:
        glm::vec3 startPos;
        glm::vec3 endPos;

    public:
        translationEventArgs(object3D* sender, glm::vec3 start, glm::vec3 end)
            : object3DEventArgs(sender)
        {
            startPos = start;
            endPos = end;
        }
    };
}

#endif