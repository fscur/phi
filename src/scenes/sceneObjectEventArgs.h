#pragma once
#include <precompiled.h>

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