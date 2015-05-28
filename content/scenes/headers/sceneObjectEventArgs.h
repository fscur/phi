#ifndef _PHI_SCENE_OBJECT_EVENT_ARGS_H_
#define _PHI_SCENE_OBJECT_EVENT_ARGS_H_

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

#endif