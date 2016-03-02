#pragma once

//#include <core\object3DEventArgs.h>
#include <core\transform.h>

namespace phi
{
    class translationEventArgs /*:
        public object3DEventArgs*/
    {
    public:
        vec3 startPos;
        vec3 endPos;
        bool cancel;

    public:
        translationEventArgs(transform* sender, vec3 start, vec3 end)
            //: object3DEventArgs(sender), startPos(start), endPos(end), cancel(false)
        {
        }
    };
}