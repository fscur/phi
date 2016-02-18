#pragma once

namespace phi
{
    class sceneObject;

    class sceneObjectEventArgs
    {
    public:
        sceneObject* sender;

    public:
        sceneObjectEventArgs(sceneObject* sender)
        {
            this->sender = sender;
        }
    };
}