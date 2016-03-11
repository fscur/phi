#pragma once
#include <phi.h>

namespace phi
{
    class sceneObject;

    class sceneObjectEventArgs
    {
    public:
        sceneObject* sender;

    public:
        sceneObjectEventArgs(sceneObject* sender) :
            sender(sender)
        {
        }
    };
}