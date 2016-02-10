#ifndef _PHI_TRANSLATION_EVENT_ARGS_H_
#define _PHI_TRANSLATION_EVENT_ARGS_H_

#include <phi/core/object3DEventArgs.h>


namespace phi
{
    class translationEventArgs :
        public object3DEventArgs
    {
    public:
        vec3 startPos;
        vec3 endPos;
        bool cancel;

    public:
        translationEventArgs(object3D* sender, vec3 start, vec3 end)
            : object3DEventArgs(sender), startPos(start), endPos(end), cancel(false)
        {
        }
    };
}

#endif