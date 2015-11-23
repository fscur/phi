#ifndef _PHI_OBJECT_3D_EVENT_ARGS_H_
#define _PHI_OBJECT_3D_EVENT_ARGS_H_

namespace phi
{
    class object3D;

    class object3DEventArgs
    {
    public:
        object3D* sender;

    public:
        object3DEventArgs(object3D* sender)
        {
            this->sender = sender;
        }
    };
}

#endif