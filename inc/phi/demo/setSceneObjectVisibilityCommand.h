#ifndef _SET_SCENE_OBJECT_VISIBILITY_COMMAND_H_
#define _SET_SCENE_OBJECT_VISIBILITY_COMMAND_H_

#include "phi/scenes/sceneObject.h"

#include "phi/demo/command.h"

class setSceneObjectVisibilityCommand :
    public command
{
private:
    phi::sceneObject* _sceneObject;
    bool _visibility;

protected:
    void execute() override;
    void executeUndo() override;

public:
    setSceneObjectVisibilityCommand(phi::sceneObject* sceneObject, bool visibility);
};

#endif