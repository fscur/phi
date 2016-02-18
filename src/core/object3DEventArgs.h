#pragma once

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